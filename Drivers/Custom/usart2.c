/* MIT License

Copyright (c) 2023 mnowell129
 
Charles M. Nowell Jr.
The Mouse Works, LLC 
mickeynowell@tmwfl.com 
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

// Generic uart driver
// Written a long time ago, ported many times.
// Charles M Nowell Jr.
// Uses RTOS and a semaphore and a counter that goes negative
// To tell the task when to wait on the semphore for transmit and receive.
// The original code dates back to Parallel C on the Transputer
// And had been ported to NXP, 6811, 6502, ST, MSP430 for multiple customers.
// The basic gist of this driver is that there are two fifo's with head and tail
// indices. The ISR pushes to the RX fifo, and pulls from the TX fifo and
// moves the respective indices.
// There are two counters indicating characters in each buffer.
// The RX counter has special meaning when it is at -1. This means the buffer
// is empty and a task is waiting on the next character.
// The ISR will, when a character comes, signal the synchonizing semaphore on which
// the task is waiting. The character is then read from the fifo.
// The task creates the -1 state on the counter in a critical region with interrupts 
// disabled to avoid a discontinuity in the counter value.
// The transmitter is symmetric. When the TX counter goes 1 bigger than the buffer size
// This means that a task is waiting to put something in the fifo, and is waiting on the semaphore.
// When the TX isr happens, space becomes available in the fifo and the ISR signals the semaphore.
// One might argue that an easier solution would be to just use RTOS queues between the task and the
// ISR, however this has characteristics I don't like. Namely that every character received incurs
// the overhead of a RTOS queue get call, where in this method received characters already in the buffer are
// received with very little overhead.
// Also the ISR on TX has to query the queue and the do a get on the queue for ever output character 
// (as the ISR can't stall on the queue), this being a lot of overhead in the ISR.
// Note that the origin of this driver predates being able to do a put or a get on a queue in an ISR
// when only semaphores could be used in an ISR, so that the natural solution nowdays (2023, using queues in
// the ISR) would not result in this design, even though this design is more efficient.


#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#include "rtos.h"
#include "ascii.h"

#include "stm32l4xx.h"
#include "stm32l432EveryPeripheral.h"
#include "SelectIO.h"
#include "gpiointerface.h"
#include "usart2.h"
#include "snprintf99.h"

// Attempted port to USART2 on nucleo STM32L432KC, to see how well the porting goes.

#define HARDWARE_UART                  USART2
#define HARDWARE_UART_BASE             USART2_BASE
#define HARDWARE_UART_IRQ              USART2_IRQn
#define USART_RCC_REG                  RCC->APB1ENR1
#define USART_RCC_BIT                  RCC_APB1ENR1_USART2EN

#define USARTINIT                      usart2Init
#define USARTISR                       _USART2_IRQHandler
#define USARTSTOP                      usart2Stop
#define USARTPUTCH                     usart2Putch
#define USARTSEND                      usart2Send
#define USARTPUTS                      usart2Puts
#define USARTGETCHWITHTIMEOUT          usart2GetchWithTimeout
#define USARTGETS                      usart2Gets
#define USARTGETCH                     usart2Getch
#define USARTTRANSMITTERIDLE           usart2TransmitterIdle
#define USARTCHARREADY                 usart2CharReady
#define USARTPRINTF                    usart2Printf
#define USARTRXSEMA                    usart2ReceiveTaskSyncSemaphore
#define USARTTXSEMA                    usart2TransmitTaskSyncSemaphore
#define USARTSHARESEMA                 usart2SharingSemaphore
#define USARTCLOCKSOURCE               LL_RCC_USART2_CLKSOURCE_PCLK1

#define usart2ReceiveTaskSyncSemaphore_NAME       "usart2Rx"
#define usart2TransmitTaskSyncSemaphore_NAME      "usart2Tx"
#define usart2SharingSemaphore_NAME               "usart2Lock"
 

RTOS_SEMA_OBJECT_STATIC(USARTRXSEMA);
RTOS_SEMA_OBJECT_STATIC(USARTTXSEMA);
RTOS_SEMA_OBJECT_STATIC(USARTSHARESEMA);


#define RECEIVE_BUFFER_SIZE  1024
#define RECEIVE_BUFFER_INDEX_MAX  (RECEIVE_BUFFER_SIZE-1)
#define TRANSMIT_BUFFER_SIZE  1024
#define TRANSMIT_BUFFER_INDEX_MAX  (TRANSMIT_BUFFER_SIZE-1)

// Make it all static so the names can be relatively generic
// so the file can be copied and used for another uart.

static volatile uint32_t usartRxWriteIndex;
static volatile uint32_t usartRxReadIndex;
static volatile uint32_t usartTxWriteIndex;
static volatile uint32_t usartTxReadIndex;

static volatile int32_t  usartTxCount;
static volatile int32_t  usartRxCount;


static void clearAllIndicies(void)
{
   usartRxWriteIndex = 0;
   usartRxReadIndex = 0;
   usartTxWriteIndex = 0;
   usartTxReadIndex = 0;
   usartTxCount = 0;
   usartRxCount = 0;
}

// All static so multiple uarts can be reused easily
static uint8_t usartRxBuffer[RECEIVE_BUFFER_SIZE];
static uint8_t usartTxBuffer[TRANSMIT_BUFFER_SIZE];

static bool usartSemaphoresCreated;


#define TRANSMITTER_IRQ_ENABLED(a)  ((a->CR1 & USART_CR1_TCIE)!= 0 ? true : false)
#define TRANSMIT_IS_COMPLETE(status)     (status & USART_ISR_TC)
#define RECEIVE_ERRORS (USART_ISR_PE | USART_ISR_FE | USART_ISR_NE | USART_ISR_ORE)
#define RECEIVE_DONE(status)   (((status) & (RECEIVE_ERRORS | USART_ISR_RXNE)) == 0)

/**
 * @brief Quick check to see if we have handled all interrupts.
 *
 * 
 * @author Charles "Mickey" Nowell (7/7/2023)
 * 
 * @param status           
 * @param usartHardwarePtr 
 * 
 * @return bool 
 */
static bool areInterruptsDone(uint32_t status,USART_TypeDef *usartHardwarePtr)
{
   // Interrupts are done when there is no receive character to process
   // When the TC enable is set and transmit complete is set we
   // stay in the interrupt. So the opposite is true when
   // the tc enable is set and we tranmit complete is NOT true
   // then we can leave.
   if((RECEIVE_DONE(status))
      && ((TRANSMITTER_IRQ_ENABLED(usartHardwarePtr)
           && (status & USART_ISR_TC)) == false)
     )
   {
      return(true);
   }
   return(false);
}

static bool readyToTransmitAgain(uint32_t status,USART_TypeDef *usartHardwarePtr)
{
   // We can transmit again if the TC flag is set
   // and the transmit complete interrupt is enabled
   // if not enabled then no transmit, regardless off TC status
   if((TRANSMIT_IS_COMPLETE(status))
      && (TRANSMITTER_IRQ_ENABLED(usartHardwarePtr)))
   {
      return(true);
   }
   return(false);
}

/**
 * @brief Not static, patched by the general handler in the
 *        interrupt function file created by cube.
 *
 * 
 * @author Charles "Mickey" Nowell (7/7/2023)
 */
void USARTISR(void)
{
   ISRCONTEXT;
   uint32_t statusRegister;
   volatile uint8_t currentRxValue;

   // Need to loop until both RX and TX interrupts are done
   while(statusRegister = HARDWARE_UART->ISR,!areInterruptsDone(statusRegister,HARDWARE_UART))
   {
      if(RECEIVE_ERRORS & statusRegister)
      {
         // Errors are cleard by reading the data register
         // then reading the ICR, per manual
         currentRxValue = HARDWARE_UART->RDR;
         HARDWARE_UART->ICR = RECEIVE_ERRORS;
         continue;
      }
      // Received value is available, read and put into fifo if space available
      // also start the task if it's waiting.
      if(statusRegister & USART_ISR_RXNE)
      {
         currentRxValue = HARDWARE_UART->RDR;
         if(usartRxCount < RECEIVE_BUFFER_SIZE)
         {
            usartRxBuffer[usartRxWriteIndex] = currentRxValue;
            usartRxWriteIndex++;
            if(usartRxWriteIndex > RECEIVE_BUFFER_INDEX_MAX)
            {
               usartRxWriteIndex = 0;
            }
            usartRxCount++;
            // Now see if someone was waiting on an empty buffer
            // signified by a negative count
            if(usartRxCount == 0)
            {
               ISR_SEMA_PUT(USARTRXSEMA);
            }
         }
      }
      if(readyToTransmitAgain(statusRegister,HARDWARE_UART))
      {
         // Note that this decrements after transmit complete
         // so when it goes to 0, the buffer is empty after the
         // last character is fully transmitted.
         // The index points to the character being transmitted!
         usartTxCount--;
         usartTxReadIndex++;
         if(usartTxReadIndex > (TRANSMIT_BUFFER_INDEX_MAX))
         {
            usartTxReadIndex = 0;
         }
         if(usartTxCount > 0)
         {
            HARDWARE_UART->TDR = usartTxBuffer[usartTxReadIndex];
         }
         else
         {
            // Buffer ran empty, so disable the transmit complete interrupt for now.
            HARDWARE_UART->CR1 &= ~(USART_CR1_TCIE);
         }
         // Now resume a task waiting to transmit.
         // The transmitting task increments the count to an illegal value
         // that being TRANSMIT_BUFFER_SIZE plus 1, but does not
         // enter the character in the fifo yet.
         // Once the isr runs and empties one character out of the fifo
         // The count comes DOWN to TRANSMIT_BUFFER_SIZE.
         // Note that this logic always derements the count so that the only
         // way the count can be buffer size if it was greater than buffer size
         // thus a task was waiting on the semaphore.
         if(usartTxCount == TRANSMIT_BUFFER_SIZE)
         {
            //If this is true a task is waiting to add a character to the fifo.
            ISR_SEMA_PUT(USARTTXSEMA);
         }
      }
   }
   ISR_EXIT();
}

/**
 * @brief Init usart1 to given baudrate, all other parameters
 *        internal to the module.
 *
 * 
 * @author Charles "Mickey" Nowell (7/7/2023)
 * 
 * @param baudRate 
 * 
 * @return uint32_t 
 */
bool USARTINIT(uint32_t baudRate)
{
   LL_USART_InitTypeDef      usartInit;
   uint32_t priority;

   clearAllIndicies();

   if(usartSemaphoresCreated == false)
   {
      SEMA_CREATE_COUNTING_LITERAL(USARTRXSEMA,RECEIVE_BUFFER_SIZE,0);
      SEMA_CREATE_COUNTING_LITERAL(USARTTXSEMA,TRANSMIT_BUFFER_SIZE,0);
      SEMA_CREATE_COUNTING_LITERAL(USARTSHARESEMA,1024,1);
      usartSemaphoresCreated = true;
   }

   // initialize the GPIOS dedicated to this usart
   GPIO_INIT_AS_ALTERNATE(VCP_TX);
   GPIO_INIT_AS_ALTERNATE(VCP_RX);

   LL_RCC_SetUSARTClockSource(USARTCLOCKSOURCE);

   /* Enable USARTx clock */
   SET_BIT(USART_RCC_REG,USART_RCC_BIT);
   // This system used basepri so we can have OS and NON OS using interrupts
   // the split for the OS is basepri 5, so encode the interrupt priority as priority 5
   priority = NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5,0); // magic
   NVIC_SetPriority(HARDWARE_UART_IRQ,priority);

   // Use defaults except for baudrate
   LL_USART_StructInit(&usartInit);
   usartInit.BaudRate = baudRate;

   if(LL_USART_DeInit(HARDWARE_UART) == ERROR)
   {
      return(false);
   }

   if(LL_USART_Init(HARDWARE_UART,&usartInit) == ERROR)
   {
      return(false);
   }
   // enable the usart
   HARDWARE_UART->CR1 |= USART_CR1_UE;

   // disable the interrupt for now
   HARDWARE_UART->CR1 &= ~(USART_CR1_TCIE | USART_CR1_TXEIE);

   // enable receive
   HARDWARE_UART->CR1 |= USART_CR1_RXNEIE;

   // Clear any transmit interrupt that might be pending at startup
   HARDWARE_UART->ICR = USART_ICR_TCCF;

   // Enable the overall usart
   NVIC_EnableIRQ(HARDWARE_UART_IRQ);

   return(true);
}

void USARTSTOP(void)
{
   volatile CONTEXT;

   ENTER_CRITICAL();
   NVIC_DisableIRQ(HARDWARE_UART_IRQ);
   HARDWARE_UART->CR1 &= ~(USART_CR1_TCIE | USART_CR1_TXEIE);
   LL_USART_DeInit(HARDWARE_UART);
   EXIT_CRITICAL();
   // Clear the semaphores back to 0's if we ever call this
   // Re-init them, when this function is called nothing should be holding
   // on the semaphores.
   SEMA_CREATE_COUNTING_LITERAL(USARTRXSEMA,RECEIVE_BUFFER_SIZE,0);
   SEMA_CREATE_COUNTING_LITERAL(USARTTXSEMA,TRANSMIT_BUFFER_SIZE,0);
   SEMA_CREATE_COUNTING_LITERAL(USARTSHARESEMA,1024,1);


}

void USARTPUTCH(uint8_t value)
{

   volatile    CONTEXT;

   SEMA_GET(USARTSHARESEMA,WAIT_FOREVER,error);
   ENTER_CRITICAL();
   if(usartTxCount == 0)
   {
      // Transmitter is completely idle, so we have to write the character in the register
      // directly, then enable the interrupt
      // Put the character in the buffer out of completness and for debug
      usartTxCount++;
      usartTxBuffer[usartTxWriteIndex] = value;
      HARDWARE_UART->CR1 |= USART_CR1_TCIE;
      HARDWARE_UART->TDR = usartTxBuffer[usartTxWriteIndex]; // USART_SendData((USART_TypeDef *)HARDWARE_UART_BASE,usartTxBuffer[usartTxWriteIndex]);
      usartTxWriteIndex++;
      if(usartTxWriteIndex > (TRANSMIT_BUFFER_INDEX_MAX))
      {
         usartTxWriteIndex = 0;
      }
         EXIT_CRITICAL();
   }
   else if(usartTxCount < TRANSMIT_BUFFER_SIZE)
   {
      usartTxCount++;
      usartTxBuffer[usartTxWriteIndex] = value;
      usartTxWriteIndex++;
      if(usartTxWriteIndex > TRANSMIT_BUFFER_INDEX_MAX)
      {
         usartTxWriteIndex = 0;
      }
      EXIT_CRITICAL();
   }
   else
   {
      // no room in buffer
      // wait on semaphore
      // after setting count to an "illegal" value as a signifier.
      usartTxCount++;
      EXIT_CRITICAL();
      SEMA_GET(USARTTXSEMA,WAIT_FOREVER,error);
      // Critical region to avoid issues
      ENTER_CRITICAL();
      usartTxBuffer[usartTxWriteIndex] = value;
      usartTxWriteIndex++;
      if(usartTxWriteIndex > TRANSMIT_BUFFER_INDEX_MAX)
      {
         usartTxWriteIndex = 0;
      }
      EXIT_CRITICAL();
   }
   SEMA_PUT(USARTSHARESEMA);
}

void USARTSEND(uint8_t *bufferPtr, uint32_t length)
{
   while(length != 0)
   {
      USARTPUTCH(*bufferPtr++);
      length--;
   }
}

void USARTPUTS(char *string)
{
   USARTSEND((uint8_t *)string,strlen(string));
}

int32_t  USARTGETCHWITHTIMEOUT(uint32_t timeout)
{
   int32_t receivedByte;

   CONTEXT;

   ENTER_CRITICAL();
   // Enter critical section to manipulate the count, shared with ISR
   // note this may go negative if the buffer is empty
   usartRxCount--;
   if(usartRxCount < 0)
   {
      // Buffer is empty
      // The count being negative signals the ISR to put on the semphore
      // on next RX interrupt.
      EXIT_CRITICAL();
      SEMA_GET(USARTRXSEMA,timeout,error);
      if(FAILTEST(error))
      {
         // No interrupts
         // need to restore the counter to 0
         // do with interrupts disabled
         ENTER_CRITICAL();
         if(usartRxCount < 0)
         {
            usartRxCount = 0;
            // now return the error code
            EXIT_CRITICAL();
            return(USART_TIMEOUT_FLAG);
         } //if
         else
         {
            /*
            This is a tricky case. The semaphore has timed out. 
            Yet the count is not -1 anymore. 
            This means we got the interrupt after the semaphore timed out. 
            So the ISR will have run and signalled the semaphore already. 
            Now in this critical region we are in a state where the semaphore 
            is signalled and the count is not -1, but we are no longer waiting 
            on the semaphore, so go ahead do the wait now to sync things back up. 
             
            First increment the rx count do "undo" our decrement that got us here. 
            */
            usartRxCount++;
            // Now we have to exit the critical section to wait on the semaphore.
            EXIT_CRITICAL();
            SEMA_GET(USARTRXSEMA,WAIT_FOREVER,error);
            return(USART_TIMEOUT_FLAG);
         }

      }
   }
   else
   {
      EXIT_CRITICAL();
   }

   // Any received byte is read from the fifo
   receivedByte = (int32_t)(usartRxBuffer[usartRxReadIndex++]);
   if(usartRxReadIndex > RECEIVE_BUFFER_INDEX_MAX)
   {
      usartRxReadIndex = 0;
   }
   return(receivedByte);
}
int32_t USARTGETCH(void)
{
   return(USARTGETCHWITHTIMEOUT(WAIT_FOREVER));
}

void USARTGETS(uint8_t *buffer)
{
   int32_t value;
   *buffer = 0;
   int32_t count = 0;
   while(1)
   {
      value = USARTGETCH();
      switch(value)
      {
         case ASCII_BS:
            if(count > 0)
            {
               count--;
               buffer--;
               *buffer = 0;
               USARTSEND((uint8_t*)"\x08 \x08",3);
            }
            break;
         case '\r':
         case '\n':
            return;
            break;
         case ASCII_NUL:
            *buffer++ = (uint8_t)(0xff & value);
            count++;
            *buffer = 0;
            return;

         default:
            if(isprint(value))
            {
               USARTPUTCH(value & 0xff);
               *buffer++ = (uint8_t)(0xff & value);
               count++;
               *buffer = 0;
            }
            break;
      }
   }
}

bool USARTTRANSMITTERIDLE(void)
{
   // if there are characters in the buffer, then no
   if(usartTxCount > 0)
   {
      return(false);
   }
   // none in buffer but might still be transmitting last
   if(HARDWARE_UART->ISR & USART_ISR_TC)
   {
      return(true);
   }
   return(false);
}  
uint8_t USARTCHARREADY(void)
{
   return((usartRxCount > 0) ? 1 : 0);
}

// Practical limit of 128 characters as a time
static char  usart1OutputBuffer[128];
void USARTPRINTF(char *format,...)
{
   va_list list;
   va_start(list,format);
   int32_t stringLength;
   if(!format)
   {
      return;
   }
   if(_rpl_vsnprintf(&stringLength,usart1OutputBuffer,128,format,list), stringLength > 0)
   {
      USARTSEND((uint8_t *)usart1OutputBuffer,stringLength);
   }
   va_end(list);
}

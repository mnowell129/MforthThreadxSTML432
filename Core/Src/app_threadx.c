/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "std.h"
#include "rtos.h"
#include "stm32l4xx.h"
#include "stm32l432EveryPeripheral.h"
#include "SelectIO.h"
#include "gpiointerface.h"
#include "usart1.h"
#include "usart2.h"
#include "forth.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TX_THREAD tx_app_thread;
TX_SEMAPHORE tx_app_semaphore;
TX_QUEUE tx_app_msg_queue;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */


// second forth instance attached to VCP

UserState _userVCP;
UserStatePtr userVCP = &(_userVCP);


const char startStringVCP[] =
   "1000 10 wordlist vcplist\n"\
   "vcplist add-to-order definitions\n"\
   "quiet\n"\
   "\x04";


#define VCP_TIB_SIZE     256
#define VCP_PAD_SIZE     32


char      tibBufferVCP[VCP_TIB_SIZE];
uint32_t  padBufferVCP[VCP_PAD_SIZE];


#define forthVCP_PRIORITY    5
#define forthVCP_STACK_SIZE  512
TASK_DATA(forthVCP);


void forthVCPFunction(ULONG threadInput)
{
   cloneRootUser(userVCP,startStringVCP,2);

#if 0
   while(1)
   {
	   printf2("hello\r\n");
	   RTOS_MSEC_DELAY(400);
	      if(usart2CharReady())
	      {
	         printf2("%c",usart2Getch());
	      }

   }
#endif
   userVCP->userVariables.namedVariables.userPrintf = usart2Printf;
   userVCP->userVariables.namedVariables.userPuts =  (PutSType)usart2Puts;
   userVCP->userVariables.namedVariables.userPutch = (PutchType)usart2Putch;
   userVCP->userVariables.namedVariables.userGetch = (GetchType)usart2Getch;
   userVCP->userVariables.namedVariables.userKeyHit = usart2CharReady;
   userVCP->userVariables.namedVariables.userGets = (GetsType)usart2Gets;
   userVCP->userVariables.namedVariables.getsVector = (GetsVector)plainGets;

   userVCP->tibBuffer = tibBufferVCP;
   userVCP->padBuffer = padBufferVCP;
   userVCP->userVariables.namedVariables.tibSize = VCP_TIB_SIZE;
   userVCP->userVariables.namedVariables.padSize = VCP_PAD_SIZE;

   runForth(userVCP);
}

// Forth startup stuff.
const char startString1[] =
   "1000 10 wordlist stuff\n"\
   "stuff add-to-order definitions\n"\
   ": tillkey [COMPILE] begin ['] ?key , ['] 0= , [COMPILE] while ; immediate\n"\
   ": tillesc [COMPILE] begin ['] key , ['] doLIT , 27 , ['] = , ['] 0= , [COMPILE] while  ; immediate\n"\
   "add_extern\n"\
   "startall\n"\
   "quiet\n"\
   "\x04";

void addExternalWords(FCONTEXT)
{
}

void startOtherThreads(FCONTEXT)
{
   CREATE_THREAD(forthVCP,1,forthVCPFunction);
}
#define USER_TIB_SIZE     256
#define USER_PAD_SIZE     32

char      tibBuffer1[USER_TIB_SIZE];
uint32_t  padBuffer1[USER_PAD_SIZE];


/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

  /* USER CODE END App_ThreadX_MEM_POOL */
  CHAR *pointer;

  /* Allocate the stack for tx app thread  */
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
                       TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /* Create tx app thread.  */
  if (tx_thread_create(&tx_app_thread, "tx app thread", tx_app_thread_entry, 0, pointer,
                       TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO, TX_APP_THREAD_PREEMPTION_THRESHOLD,
                       TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }
  /* Allocate the stack for tx app queue.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       TX_APP_MSG_QUEUE_FULL_SIZE * sizeof(ULONG), TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /* Create tx app queue.  */
  if (tx_queue_create(&tx_app_msg_queue, "tx app queue", TX_APP_SINGLE_MSG_SIZE,
                      pointer, TX_APP_MSG_QUEUE_FULL_SIZE * sizeof(ULONG)) != TX_SUCCESS)
  {
    return TX_QUEUE_ERROR;
  }

  /* Create tx app semaphore.  */
  if (tx_semaphore_create(&tx_app_semaphore, "tx app semaphore", 0) != TX_SUCCESS)
  {
    return TX_SEMAPHORE_ERROR;
  }

  /* USER CODE BEGIN App_ThreadX_Init */

  /* USER CODE END App_ThreadX_Init */

  return ret;
}
/**
  * @brief  Function implementing the tx_app_thread_entry thread.
  * @param  thread_input: Hardcoded to 0.
  * @retval None
  */
void tx_app_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN tx_app_thread_entry */
   // Make cycles counter available
   CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
   CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
   DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
   DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
   DWT->CYCCNT = 0;


   usart1Init(115200);
   usart2Init(115200);

   usart2Printf("Hello world\r\n");
   GPIO_INIT_AS_OUT(GREEN_LED);

   initCoreForth(startString1);
   // user1->userVariables.namedVariables.setEcho = uart1SetEcho;
   user1->tibBuffer = tibBuffer1;
   user1->padBuffer = padBuffer1;
   user1->userVariables.namedVariables.tibSize = USER_TIB_SIZE;
   user1->userVariables.namedVariables.padSize = USER_PAD_SIZE;

   runForth(user1);


   while(1)
   {
      RTOS_MSEC_DELAY(100);
      SETHIGH(GREEN_LED);
      RTOS_MSEC_DELAY(100);
      SETLOW(GREEN_LED);
      printf("Blink\r\n");
      if(usart1CharReady())
      {
         printf("%c",usart1Getch());
      }

   }

  /* USER CODE END tx_app_thread_entry */
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

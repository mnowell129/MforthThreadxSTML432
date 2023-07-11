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

/*
Public Domain Open Source
Written by Charles M Nowell, Jr.
For muliple uses.
Free and open to all for use. 
 */
#ifndef _RTOS_H
#define _RTOS_H


#define THREADX


#ifdef THREADX

   #include "tx_api.h"
   #include "tx_port.h"

   #define WAIT_FOREVER                              TX_WAIT_FOREVER
   #define WAIT_NOT_AT_ALL                           TX_NO_WAIT
   #define OS_ERROR_NONE                             TX_SUCCESS
   #define OS_ERROR_TIMEOUT                          errQUEUE_EMPTY
                                                    
   #define CONTEXT                                   volatile UINT interrupt_save;volatile uint8_t error
   #define ISRCONTEXT                                volatile UINT interrupt_save;volatile uint8_t error


__attribute__( ( always_inline ) ) static inline unsigned int __get_interrupt_posture(void)
{
   unsigned int posture;
#ifdef TX_PORT_USE_BASEPRI
    __asm__ volatile ("MRS  %0, BASEPRI ": "=r" (posture));
#else
    __asm__ volatile ("MRS  %0, PRIMASK ": "=r" (posture));
#endif
    return(posture);
}

#ifdef TX_PORT_USE_BASEPRI
__attribute__( ( always_inline ) ) static inline void __set_basepri_value(unsigned int basepri_value)
{
    __asm__ volatile ("MSR  BASEPRI,%0 ": : "r" (basepri_value));
}
#else
__attribute__( ( always_inline ) ) static inline void __enable_interrupts(void)
{
    __asm__ volatile ("CPSIE  i": : : "memory");
}
#endif

__attribute__( ( always_inline ) ) static inline unsigned int __disable_interrupts(void)
{
unsigned int int_posture;

    int_posture = __get_interrupt_posture();

#ifdef TX_PORT_USE_BASEPRI
    __set_basepri_value(TX_PORT_BASEPRI);
#else
    __asm__ volatile ("CPSID i" : : : "memory");
#endif
    return(int_posture);
}

__attribute__( ( always_inline ) ) static inline void __restore_interrupt(unsigned int int_posture)
{
#ifdef TX_PORT_USE_BASEPRI
    __set_basepri_value(int_posture);
    //__asm__ volatile ("MSR  BASEPRI,%0": : "r" (int_posture): "memory");
#else
    __asm__ volatile ("MSR  PRIMASK,%0": : "r" (int_posture): "memory");
#endif
}

   #define ENTER_CRITICAL()                          interrupt_save =   __disable_interrupts()
   #define EXIT_CRITICAL()                           __restore_interrupt(interrupt_save)
   #define ISR_CRITICAL_CONTEXT                      UINT interrupt_save;volatile Byte error
   #define ENTER_CRITICAL_ISR()                      interrupt_save =  __disable_interrupts()
   #define EXIT_CRITICAL_ISR()                       __restore_interrupt(interrupt_save)

   #define RTOS_QUEUE                                TX_QUEUE
   #define RTOS_QUEUE_PTR                            TX_QUEUE *
   #define RTOS_SEMA_PTR                             TX_SEMAPHORE *
   #define RTOS_SEMA                                 TX_SEMAPHORE
                                                    

   #define RTOS_SEMA_OBJECT(a)                       TX_SEMAPHORE a##Data;TX_SEMAPHORE *a = &a##Data
   #define RTOS_SEMA_OBJECT_STATIC(a)                static TX_SEMAPHORE a##Data;static TX_SEMAPHORE *a = &a##Data
   #define RTOS_COUNTING_SEMA_OBJECT(a,b,c)          TX_SEMAPHORE a##Data;TX_SEMAPHORE *a = &a##Data;\
                                                     const uint32_t a##_INITIAL = b;const uint32_t a##_MAX = c
   #define EXTERN_RTOS_SEMA_OBJECT(a)                extern TX_SEMAPHORE *a;extern TX_SEMAPHORE a##Data
   #define RTOS_SEMA_DATA(a)                         TX_SEMAPHORE  a##Data
                                                    
   #define SEMA_CREATE(a)                            tx_semaphore_create(a, a##_NAME, a##_INITIAL)
   #define SEMA_CREATE_STATIC(a)                     tx_semaphore_create(a, a##_NAME, a##_INITIAL)
   #define SEMA_CREATE_COUNTING(a,max,initial)       tx_semaphore_create(a, a##_NAME, a##_INITIAL)
//   #define 
   #define _NAME(a)                                  a##_NAME
   #define NAME(a)                                   _NAME(a)
   #define SEMA_CREATE_COUNTING_LITERAL(a,max,initial)       tx_semaphore_create(a,NAME(a),initial)
   #define SEMA_CREATE_BINARY(a)                     tx_semaphore_create(a, a##_NAME, 1)
   #define SEMA_CREATE_BINARY_STATIC(a)              tx_semaphore_create(a, a##_NAME, 1)
   #define SEMA_CREATE_BINARY_STATIC_NO_GIVE(a)      tx_semaphore_create(a, a##_NAME, 0)

   #define SEMA_GET(a,t,e)                           e = tx_semaphore_get(a,t)
   #define SEMA_GET_WAIT_FOREVER(a)                  tx_semaphore_get(a,WAIT_FOREVER) 
   //    #define SEMA_TRY(a)       ??
   #define SEMA_PUT(a)                               tx_semaphore_put(a)
   #define ISR_SEMA_GET(a,t,e)                       e = tx_semaphore_get(a,t)
   #define ISR_SEMA_PUT(a)                           tx_semaphore_put(a)
   #define ISR_EXIT()                                // nothing needed for Threadx

   #define FAILTEST(e)                               ((e)!= TX_SUCCESS)
   #define PASSTEST(e)                               ((e) == TX_SUCCESS)


   #define RTOS_QUEUE_OBJECT(a)                      uint32_t a##Array[a##_DEPTH]; TX_QUEUE a##Data;TX_QUEUE *a = &a##Data;
   #define EXTERN_RTOS_QUEUE_OBJECT(a)               extern uint32_t a##Array[a##_DEPTH];extern TX_QUEUE a##Data;extern TX_QUEUE *a;

   #define COMMON_QUEUE_SIZE 4


   #define QUEUE_CREATE(a)                           tx_queue_create(a, a##_NAME, 1,a##Array, a##_DEPTH * sizeof(ULONG))
   #define QUEUE_GET(a,pData,t,e)       e = tx_queue_receive(a,(void *)&(pData),t)
   //    #define QUEUE_QUERY(a,pData,t,e)  ???   e = xQueuePeek(a,(void *)&(pData),t)
   #define QUEUE_PUT(a,pData,t,e)       e = tx_queue_send(a,(void*)&(pData),t)
   #define ISR_QUEUE_PUT(a,pdata)       e = tx_queue_send(a,(void*)&(pData),TX_NO_WAIT)

   #define FAILTEST(e)      ((e)!= TX_SUCCESS)
   #define PASSTEST(e)      ((e) == TX_SUCCESS)

   #define OSTIME()  ((Int32)(tx_time_get()))


   #define TASK_STACK_DATA(a)    uint32_t     a##StackData[a##_STACK_SIZE]
   #define TASK_TCB_DATA(a)      TX_THREAD    a##TaskTCB
   #define TASK_PTR(a)           TX_THREAD    *a##TaskPtr
   #define TASK_DATA(a)          TX_THREAD    a##TaskTCB;\
                                 TX_THREAD    *a##TaskPtr = &a##TaskTCB;\
                                 uint32_t     a##StackData[a##_STACK_SIZE/4]

   #define EXTERN_TASK_DATA(a)   extern TX_THREAD a##TaskPtr;\
                                 extern TX_THREAD a##TaskTCB;\
                                 extern uint32_t  a##StackData[a##_STACK_SIZE/4]



   #define CREATE_THREAD_WITH_CHECK(name,params,function)   (tx_thread_create(name##TaskPtr, name##_NAME, function, params, (uint8_t*)name##StackData,\
                       name##_STACK_SIZE, name##_PRIORITY, name##_PRIORITY,\
                       TX_NO_TIME_SLICE, TX_AUTO_START)!= TX_SUCCESS)

   #define CREATE_THREAD(name,params,function)   tx_thread_create(name##TaskPtr, name##_NAME, function, params, (uint8_t*)name##StackData,\
                       name##_STACK_SIZE, name##_PRIORITY, name##_PRIORITY,\
                       TX_NO_TIME_SLICE, TX_AUTO_START)

   #define RTOS_DELAY(a)            tx_thread_sleep(a)
   // All signed math
   #define RTOS_TICKS_PER_SECOND    ((int32_t)TX_TIMER_TICKS_PER_SECOND)
   #define MSECS_TO_TICKS(a)        (a == WAIT_FOREVER ? WAIT_FOREVER : ((((int32_t)a))*1000 / (1000000/RTOS_TICKS_PER_SECOND)))
   #define RTOS_MSEC_DELAY(a)       tx_thread_sleep((uint32_t)(MSECS_TO_TICKS(a)))



#endif
#ifdef MICRIUM

   #include <ucos_ii.h>
   #include <cpu.h>
   #include <lib_def.h>
   #include <lib_mem.h>
   #include <lib_str.h>
   #include "bsp.h"
   #include "bsp_int.h"
   #include "processor.h"

   #define RTOS_TICKS_PER_SECOND OS_TICKS_PER_SEC


   #define CONTEXT                  OS_CPU_SR  cpu_sr = 0;Byte error
   #define ISRCONTEXT
   #define ENTER_CRITICAL()         OS_ENTER_CRITICAL()
   #define EXIT_CRITICAL()          OS_EXIT_CRITICAL()
typedef  OS_EVENT             *OS_EVENT_PTR;

   #define RTOS_QUEUE            OS_EVENT_PTR
   #define RTOS_SEMA_PTR         OS_EVENT_PTR
   #define RTOS_SEMA             OS_EVENT

   #define SEMA_GET(a,t,e)          OSSemPend(a,t,&e)
   #define SEMA_TRY(a)              OSSemAccept(a)
   #define SEMA_PUT(a)              OSSemPost(a)
   #define ISR_SEMA_GET(a,t,e)      OSSemPend(a,t,&e)
   #define ISR_SEMA_PUT(a)          OSSemPost(a)

   #define SEMA_CREATE(a,i)        a=OSSemCreate(i)

   #define QUEUE_CREATE(array,queueLength,itemSize,handle) handle = OSQCreate(array,queueLength)
   #define ISR_QUEUE_PUT(a,pdata)       e = OSQPost(a,(void *)(pdata))
   #define QUEUE_GET(a,pData,t,e)       pData = OSQPend(a,t,&e)
   #define QUEUE_PUT(a,pData,t,e)       e = OSQPost(a,(void *)(pData))

   #define WAIT_FOREVER 0
   #define ISR_EXIT()

   #define WAIT_NOT_AT_ALL       0

   #define FAILTEST(e)      ((e)!= OS_ERR_NONE)
   #define PASSTEST(e)      ((e) == OS_ERR_NONE)
   #define RTOS_DELAY(a)    RTOS_DELAY(MSECS_TO_TICKS(a))

   #define RTOS_TICKS_PER_SECOND    OS_TICKS_PER_SEC


   #define OSTIME()  ((Int32)(OSTimeGet()))

   #define CREATE_THREAD(name,params,id)  OSTaskCreateExt(name##Thread,\
   (void *)0,\
   (OS_STK *)(&(name##ThreadStack[params##_THREAD_STACK_SIZE - 1])),\
   params##_THREAD_PRIORITY,\
   params##_THREAD_PRIORITY,\
   (OS_STK *)(&(name##ThreadStack[0])),\
   params##_THREAD_STACK_SIZE,\
   (void *)0,\
   OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP);\
      if(error != OS_ERR_NONE)\
      {\
         while(1);\
      }\
   OSTaskNameSet(params##_THREAD_PRIORITY,id,&error)

#endif

#ifdef FREE_RTOS

   #include "freertos.h"
   #include "semphr.h"
   #include "task.h"
   #include "queue.h"



// user needs to define name_STACK_SIZE
// and name
   #define TASK_STACK_DATA(a)    StackType_t  a##StackData[a##_STACK_SIZE]
   #define TASK_TCB_DATA(a)      StaticTask_t a##TaskTCB
   #define TASK_PTR(a)           TaskHandle_t a##TaskPtr
   #define TASK_DATA(a)          TaskHandle_t a##TaskPtr;\
                                 StaticTask_t a##TaskTCB;\
                                 StackType_t  a##StackData[a##_STACK_SIZE]

   #define EXTERN_TASK_DATA(a)   extern TaskHandle_t a##TaskPtr;\
                                 extern StaticTask_t a##TaskTCB;\
                                 extern StackType_t  a##StackData[a##_STACK_SIZE]

// note for FreeRTOS V.9x or later, with static task data
   #define CREATE_THREAD(name,params,function)  name##TaskPtr = \
    xTaskCreateStatic(function,\
    #name,\
    name##_STACK_SIZE,\
    (void *)(params),\
    name##_PRIORITY,\
    (StackType_t * const)(name##StackData),\
    (StaticTask_t * const)&(name##TaskTCB));\
    if(name##TaskPtr == NULL)while(1)


   #define CONTEXT               BaseType_t error
//  #define CONTEXT
   #define ERROR_VALUE
   #define ISRCONTEXT            BaseType_t    preemptYes = pdFALSE
   #define ENTER_CRITICAL()      taskENTER_CRITICAL()
   #define EXIT_CRITICAL()       taskEXIT_CRITICAL()

   #define ISR_CRITICAL_CONTEXT     uint32_t  basePriorityValue

   #define ENTER_CRITICAL_ISR()     basePriorityValue = taskENTER_CRITICAL_FROM_ISR()
   #define EXIT_CRITICAL_ISR()       taskEXIT_CRITICAL_FROM_ISR( basePriorityValue )

   #define RTOS_SEMA             void
   #define RTOS_SEMA_PTR         SemaphoreHandle_t
   #define RTOS_SEMA_OBJECT(a)   SemaphoreHandle_t a;StaticSemaphore_t a##Data
   #define RTOS_COUNTING_SEMA_OBJECT(a,b,c)   SemaphoreHandle_t a;StaticSemaphore_t a##Data;\
   const uint32_t a##_INITIAL = b;const uint32_t a##_MAX = c
   #define EXTERN_RTOS_SEMA_OBJECT(a)   extern SemaphoreHandle_t a;extern StaticSemaphore_t a##Data
   #define RTOS_SEMA_DATA(a)     StaticSemaphore_t  a##Data


//   #define SEMA_CREATE(a)                        a=xSemaphoreCreateBinary()
//   #define SEMA_CREATE_COUNTING(a,max,initial)   a = xSemaphoreCreateCounting(max,initial)
//   #define SEMA_CREATE_BINARY(a)                 a=xSemaphoreCreateBinary();xSemaphoreGive(a)

//    #define SEMA_CREATE(a)                        a=xSemaphoreCreateBinary()
   #define SEMA_CREATE(a)               a = xSemaphoreCreateCountingStatic(a##_MAX,a##_INITIAL,&a##Data)
   #define SEMA_CREATE_STATIC(a)               a = xSemaphoreCreateCountingStatic(a##_MAX,a##_INITIAL,&a##Data)
   #define SEMA_CREATE_COUNTING(ptr,max,initial)               ptr = xSemaphoreCreateCounting(max,initial)
   #define SEMA_CREATE_BINARY(a)                 a=xSemaphoreCreateBinary();xSemaphoreGive(a)
//   #define SEMA_CREATE_BINARY_STATIC(a)                 a=xSemaphoreCreateBinaryStatic();xSemaphoreGive(a)
   #define SEMA_CREATE_BINARY_STATIC(a)                 a=xSemaphoreCreateBinaryStatic(&a##Data);xSemaphoreGive(a)
   #define SEMA_CREATE_BINARY_STATIC_NO_GIVE(a)         a=xSemaphoreCreateBinaryStatic(&a##Data)
   #define SEMA_GET(a,t,e)       e = xSemaphoreTake(a,t)
   #define SEMA_GET_WAIT_FOREVER(a)       xSemaphoreTake(a,portMAX_DELAY)
   #define SEMA_PUT(a)           xSemaphoreGive(a)
   #define ISR_SEMA_GET(a,t,e)   e = xSemaphoreTakeFromISR(a,t)
   #define ISR_SEMA_PUT(a)       xSemaphoreGiveFromISR(a,((BaseType_t * const)(&preemptYes)))
   #define ISR_EXIT()            portEND_SWITCHING_ISR(preemptYes)


   #define RTOS_QUEUE_PTR        QueueHandle_t
   #define RTOS_QUEUE            void
   #define RTOS_QUEUE_OBJECT(a)  QueueHandle_t a;StaticQueue_t a##Data;uint32_t a##Array[a##_DEPTH]
   #define EXTERN_RTOS_QUEUE_OBJECT(a)  extern QueueHandle_t a;extern StaticQueue_t a##Data; extern uint32_t a##Array[a##_DEPTH]


   #define COMMON_QUEUE_SIZE 4

//   #define QUEUE_CREATE(array,queueLength,itemSize,handle)          handle = xQueueCreate(queueLength,itemSize)
//   #define QUEUE_CREATE(array,queueLength,itemSize,handle)          handle = xQueueCreateStatic(queueLength,itemSize)
   #define QUEUE_CREATE(symbol)         symbol = xQueueCreateStatic(symbol##_DEPTH,4,(uint8_t *)(symbol##Array),&symbol##Data);\
   vQueueAddToRegistry(symbol,#symbol)
   #define QUEUE_GET(a,pData,t,e)       e = xQueueReceive(a,(void *)&(pData),t)
   #define QUEUE_QUERY(a,pData,t,e)     e = xQueuePeek(a,(void *)&(pData),t)
   #define QUEUE_PUT(a,pData,t,e)       e = xQueueSend(a,(void*)&(pData),t)
   #define ISR_QUEUE_PUT(a,pdata)       xQueueSendFromISR(a,(void *)&(pdata),(BaseType_t * const)&preemptYes)


   #define WAIT_FOREVER                 portMAX_DELAY
   #define WAIT_NOT_AT_ALL       0
   #define OS_ERROR_NONE         pdPASS
   #define OS_ERROR_TIMEOUT      errQUEUE_EMPTY
   #define ISR_EXIT()            portEND_SWITCHING_ISR(preemptYes)

   #define FAILTEST(e)           ((e) == pdFALSE)
   #define PASSTEST(e)           ((e) != pdFALSE)
   #define OSTIME()                 ((Int32)(xTaskGetTickCount()))

   #define RTOS_DELAY(a)            vTaskDelay(a)
// All signed math now
   #define RTOS_TICKS_PER_SECOND ((int32_t)configTICK_RATE_HZ)
   #define MSECS_TO_TICKS(a)        (a == WAIT_FOREVER ? WAIT_FOREVER : ((((int32_t)a))*1000 / (1000000/RTOS_TICKS_PER_SECOND)))
   #define RTOS_MSEC_DELAY(a)       vTaskDelay((uint32_t)(MSECS_TO_TICKS(a)))


#endif

///*************************************************************
///*************************************************************
#define SECONDS_FROM_MSEC(v)            ((v)/1000)
#define MINUTES_FROM_SECONDS(v)         ((v)/60)
#define HOURS_FROM_MINUTES(v)           ((v)/60)
#define SECONDS_FROM_MINUTES(v)         ((v)*60)
#define MINUTES_FROM_HOURS(v)           ((V)*60)
#define SECONDS_PER_MINUTE              (60)
#define MINUTES_PER_HOUR                (60)
#define HOURS_PER_DAY                   (24)
#define MINUTES_PER_DAY                 (MINUTES_PER_HOUR * HOURS_PER_DAY)
#define SECONDS_PER_HOUR                (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)
#define MSEC_PER_SEC                    (1000)
#define MSEC_PER_MINUTE                 (MSEC_PER_SEC * SECONDS_PER_MINUTE)
#define MSEC_PER_HOUR                   (MSEC_PER_MINUTE * MINUTES_PER_HOUR)
#define MSEC_PER_DAY                    (HOURS_PER_DAY*MSEC_PER_HOUR)
#define TICKS_PER_MINUTE                (RTOS_TICKS_PER_SECOND * SECONDS_PER_MINUTE)

#define MINUTES_TO_TICKS(v)             ((v) * TICKS_PER_MINUTE)
#define SECONDS_TO_TICKS(v)             ((v) * RTOS_TICKS_PER_SECOND)
#define HOURS_TO_TICKS(v)               ((v) * MINUTES_PER_HOUR * RTOS_TICKS_PER_SECOND * SECONDS_PER_MINUTE)

#define TICKS_TO_MINUTES(v)             ((int32_t)((v)/ TICKS_PER_MINUTE))
#define TICKS_TO_SECONDS(v)             ((int32_t)((v)/ RTOS_TICKS_PER_SECOND))

///*************************************************************
///*************************************************************

#endif

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

#ifndef __USART2_H
#define __USART2_H

///*************************************************************
///*************************************************************
#ifndef USART_TIMEOUT_FLAG 
#define USART_TIMEOUT_FLAG -1
#endif
#ifndef ASCII_BS
#include "ascii.h"
#endif
#include "stm32l4xx_ll_usart.h"



void _USART2_IRQHandler(void);
bool usart2Init(uint32_t baudRate);
void usart2Stop(void);
void usart2Putch(uint8_t value);
void usart2Send(uint8_t *bufferPtr, uint32_t length);
void usart2Puts(char *string);
int32_t  usart2GetchWithTimeout(uint32_t timeout);
int32_t usart2Getch(void);
void usart2Gets(uint8_t *buffer);
bool usart2TransmitterIdle(void);
uint8_t usart2CharReady(void);
void usart2Printf(char *format,...);
#define printf2 usart2Printf
#endif



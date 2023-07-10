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


#ifndef _FORTHCONSOLE_H
#define _FORTHCONSOLE_H

#include "usart1.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>


int vsprintf(char *buffer,const char *format,va_list list);
int sprintf(char *buffer,const char *format,...);
int snprintf(char *, size_t, const char *, ...);
int vsnprintf(char *, size_t, const char *, va_list);

/*
void _USART1_IRQHandler(void);
bool usart1Init(uint32_t baudRate);
void usart1Stop(void);
void usart1Putch(uint8_t value);
void usart1Send(uint8_t *bufferPtr, uint32_t length);
void usart1Puts(char *string);
int32_t  usart1GetchWithTimeout(uint32_t timeout);
int32_t usart1Getch(void);
void usart1Gets(uint8_t *buffer);
bool usart1TransmitterIdle(void);
uint8_t usart1CharReady(void);
*/


#define forthPrintf     usart1Printf
#define forthPuts       usart1Puts
#define forthPutch      usart1Putch
#define forthGetch      usart1Getch
#define forthCharReady  usart1CharReady
#define forthGets       usart1Gets

#endif

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

#ifndef GPIOINTERFACE_H
#define GPIOINTERFACE_H

// A set of macros to make dealing with GPIOS simple
// The goal is to name the pin with a meaningful name
// Then be able to init the pin function with macro
// and have it be the default value

// Basic functions needed
// 1) enable the clock for the port
// 2) Setup the pin function as out,in,AF, etc
// 3) set the nominal pin state
// 4) provide macros to set the value.

/*
Sample data for GPIO pin on Nucleo STM32L432KC pin 
 
#define GREEN_LED                        GREEN_LED
#define GREEN_LED_ENABLE_FN              LL_AHB2_GRP1_EnableClock 
#define GREEN_LED_ENABLE_SELECT          LL_AHB2_GRP1_PERIPH_GPIOB
#define GREEN_LED_PORT_NAME              A
#define GREEN_LED_PORT_BASE              GPIOB
#define GREEN_LED_PIN_MASK               LL_GPIO_PIN_3
#define GREEN_LED_PIN_MODE               LL_GPIO_MODE_INPUT
#define GREEN_LED_AF_NUMBER              LL_GPIO_AF_DOES_NOT_APPLY
#define GREEN_LED_PIN_INIT_STATE         SIGNAL_LOW
#define GREEN_LED_PIN_ACTIVE_STATE       SIGNAL_HIGH
#define GREEN_LED_PIN_INACTIVE_STATE     SIGNAL_LOW
#define GREEN_LED_PIN_DRIVE              LL_GPIO_OUTPUT_PUSHPULL
#define GREEN_LED_PIN_PULLSTATE          LL_GPIO_PULL_NO
#define GREEN_LED_PIN_SPEED              LL_GPIO_SPEED_FREQ_VERY_HIGH
 
LL_AHB2_GRP1_EnableClock 
 
*/

void gpioWriteBits(GPIO_TypeDef *portBase, uint32_t pinMask,bool newState);

void gpioInitializePin(GPIO_TypeDef *portBase, 
                       uint32_t pinMode, 
                       uint32_t pinMask,
                       uint32_t driveType, 
                       uint32_t pullMode, 
                       uint32_t pinSpeed,
                       bool     defaultValue,
                       uint32_t alternateFunction);


// Enable the clock
#define GPIO_CLOCK_ENABLE(name)     name##_ENABLE_FN(name##_ENABLE_SELECT)

#define GPIO_INIT(name,mode,alternate)      GPIO_CLOCK_ENABLE(name);\
                                            gpioInitializePin(name##_PORT_BASE,\
                                            mode,\
                                            name##_PIN_MASK,\
                                            name##_PIN_DRIVE,\
                                            name##_PIN_PULLSTATE,\
                                            name##_PIN_SPEED,\
                                            name##_PIN_INIT_STATE,alternate)

#define GPIO_INIT_AS_OUT(name)              GPIO_INIT(name,LL_GPIO_MODE_OUTPUT,0)

#define GPIO_INIT_AS_IN(name)               GPIO_INIT(name,LL_GPIO_MODE_INPUT,0)

#define GPIO_INIT_AS_ALTERNATE(name)        GPIO_INIT(name,LL_GPIO_MODE_ALTERNATE,name##_AF_NUMBER)




#define SETACTIVE(name)          gpioWriteBits(name##_PORT_BASE,name##_PIN_MASK, name##_PIN_ACTIVE_STATE)
#define SETINACTIVE(name)        gpioWriteBits(name##_PORT_BASE,name##_PIN_MASK, name##_PIN_INACTIVE_STATE)

#define SETHIGH(name)            gpioWriteBits(name##_PORT_BASE,name##_PIN_MASK, SIGNAL_HIGH)
#define SETLOW(name)             gpioWriteBits(name##_PORT_BASE,name##_PIN_MASK, SIGNAL_LOW)

#define CHECK_FOR_HIGH(name)     (((name##_PORT_BASE->IDR)&(name##_PIN_MASK)) == name##_PIN_MASK)
#define CHECK_FOR_LOW(name)      ((((name##_PORT_BASE->IDR)&(name##_PIN_MASK)) == 0))

#endif

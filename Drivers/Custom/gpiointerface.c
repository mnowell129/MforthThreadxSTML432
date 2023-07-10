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

#include "std.h"
#include "stm32l4xx.h"
#include "stm32l432EveryPeripheral.h"

#include "gpiointerface.h"


/**
 * @brief Simple function to control GPIOS using macros
 *
 * 
 * @author Charles "Mickey" Nowell (7/8/2023)
 * 
 * @param portBase 
 * @param pinMask  
 * @param newState 
 */
void gpioWriteBits(GPIO_TypeDef *portBase, uint32_t pinMask,bool newState)
{
   if(newState)
   {
      LL_GPIO_SetOutputPin(portBase,pinMask);
   }
   else
   {
      LL_GPIO_ResetOutputPin(portBase,pinMask);
   }
}

/**
 * @brief Common parameterized GPIO init function. Writes the
 *        GPIO to the correct state before initializing to avoid
 *        glitches.
 *
 * 
 * @author Charles "Mickey" Nowell (7/8/2023)
 * 
 * @param portBase          
 * @param pinMode           
 * @param pinMask           
 * @param driveType         
 * @param pullMode          
 * @param pinSpeed          
 * @param defaultValue      
 * @param alternateFunction 
 */
void gpioInitializePin(GPIO_TypeDef *portBase, 
                 uint32_t pinMode, 
                 uint32_t pinMask,
                 uint32_t driveType, 
                 uint32_t pullMode, 
                 uint32_t pinSpeed,
                 bool     defaultValue,
                 uint32_t alternateFunction)
{
   LL_GPIO_InitTypeDef gpioInitParameters;
   // write default value ahead of time
   // this is so the pin won't glitch when set to output.
   gpioWriteBits(portBase,pinMask,defaultValue);
   gpioInitParameters.Mode  = pinMode;
   gpioInitParameters.Pin   = pinMask;
   gpioInitParameters.OutputType = driveType;
   gpioInitParameters.Pull = pullMode;
   gpioInitParameters.Speed = pinSpeed;
   gpioInitParameters.Alternate = alternateFunction;
   LL_GPIO_Init(portBase,&gpioInitParameters);
   gpioWriteBits(portBase,pinMask,defaultValue);
}


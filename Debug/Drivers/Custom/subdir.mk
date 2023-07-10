################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Custom/gpiointerface.c \
../Drivers/Custom/snprintf99.c \
../Drivers/Custom/usart1.c \
../Drivers/Custom/usart2.c 

OBJS += \
./Drivers/Custom/gpiointerface.o \
./Drivers/Custom/snprintf99.o \
./Drivers/Custom/usart1.o \
./Drivers/Custom/usart2.o 

C_DEPS += \
./Drivers/Custom/gpiointerface.d \
./Drivers/Custom/snprintf99.d \
./Drivers/Custom/usart1.d \
./Drivers/Custom/usart2.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Custom/%.o Drivers/Custom/%.su Drivers/Custom/%.cyclo: ../Drivers/Custom/%.c Drivers/Custom/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -DUSE_FULL_LL_DRIVER -DTX_PORT_USE_BASEPRI -DTX_PORT_BASEPRI=80 -DTX_INCLUDE_USER_DEFINE_FILE -c -I../Core/Inc -I../Middlewares/mForth -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/Custom -I../AZURE_RTOS/App -I../Middlewares/ST/threadx/common/inc/ -I../Middlewares/ST/threadx/ports/cortex_m4/gnu/inc/ -O0 -ffunction-sections -fdata-sections -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wa,-adhlns="$(@:%.o=%.lst)" -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Custom

clean-Drivers-2f-Custom:
	-$(RM) ./Drivers/Custom/gpiointerface.cyclo ./Drivers/Custom/gpiointerface.d ./Drivers/Custom/gpiointerface.o ./Drivers/Custom/gpiointerface.su ./Drivers/Custom/snprintf99.cyclo ./Drivers/Custom/snprintf99.d ./Drivers/Custom/snprintf99.o ./Drivers/Custom/snprintf99.su ./Drivers/Custom/usart1.cyclo ./Drivers/Custom/usart1.d ./Drivers/Custom/usart1.o ./Drivers/Custom/usart1.su ./Drivers/Custom/usart2.cyclo ./Drivers/Custom/usart2.d ./Drivers/Custom/usart2.o ./Drivers/Custom/usart2.su

.PHONY: clean-Drivers-2f-Custom


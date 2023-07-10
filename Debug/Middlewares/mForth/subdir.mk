################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/mForth/asswords.c \
../Middlewares/mForth/corewords.c \
../Middlewares/mForth/float.c \
../Middlewares/mForth/forth.c \
../Middlewares/mForth/words.c 

OBJS += \
./Middlewares/mForth/asswords.o \
./Middlewares/mForth/corewords.o \
./Middlewares/mForth/float.o \
./Middlewares/mForth/forth.o \
./Middlewares/mForth/words.o 

C_DEPS += \
./Middlewares/mForth/asswords.d \
./Middlewares/mForth/corewords.d \
./Middlewares/mForth/float.d \
./Middlewares/mForth/forth.d \
./Middlewares/mForth/words.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/mForth/%.o Middlewares/mForth/%.su Middlewares/mForth/%.cyclo: ../Middlewares/mForth/%.c Middlewares/mForth/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -DUSE_FULL_LL_DRIVER -DTX_PORT_USE_BASEPRI -DTX_PORT_BASEPRI=80 -DTX_INCLUDE_USER_DEFINE_FILE -c -I../Core/Inc -I../Middlewares/mForth -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/Custom -I../AZURE_RTOS/App -I../Middlewares/ST/threadx/common/inc/ -I../Middlewares/ST/threadx/ports/cortex_m4/gnu/inc/ -O0 -ffunction-sections -fdata-sections -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wa,-adhlns="$(@:%.o=%.lst)" -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-mForth

clean-Middlewares-2f-mForth:
	-$(RM) ./Middlewares/mForth/asswords.cyclo ./Middlewares/mForth/asswords.d ./Middlewares/mForth/asswords.o ./Middlewares/mForth/asswords.su ./Middlewares/mForth/corewords.cyclo ./Middlewares/mForth/corewords.d ./Middlewares/mForth/corewords.o ./Middlewares/mForth/corewords.su ./Middlewares/mForth/float.cyclo ./Middlewares/mForth/float.d ./Middlewares/mForth/float.o ./Middlewares/mForth/float.su ./Middlewares/mForth/forth.cyclo ./Middlewares/mForth/forth.d ./Middlewares/mForth/forth.o ./Middlewares/mForth/forth.su ./Middlewares/mForth/words.cyclo ./Middlewares/mForth/words.d ./Middlewares/mForth/words.o ./Middlewares/mForth/words.su

.PHONY: clean-Middlewares-2f-mForth


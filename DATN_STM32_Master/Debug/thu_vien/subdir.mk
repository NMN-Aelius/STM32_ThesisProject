################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../thu_vien/Nhut.c 

OBJS += \
./thu_vien/Nhut.o 

C_DEPS += \
./thu_vien/Nhut.d 


# Each subdirectory must supply rules for building sources it contributes
thu_vien/%.o thu_vien/%.su thu_vien/%.cyclo: ../thu_vien/%.c thu_vien/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32/DATN_STM32_Master/Thu_vien" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-thu_vien

clean-thu_vien:
	-$(RM) ./thu_vien/Nhut.cyclo ./thu_vien/Nhut.d ./thu_vien/Nhut.o ./thu_vien/Nhut.su

.PHONY: clean-thu_vien


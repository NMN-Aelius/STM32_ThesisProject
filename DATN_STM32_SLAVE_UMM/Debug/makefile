################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := DATN_STM32_SLAVE_UMM
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
DATN_STM32_SLAVE_UMM.elf \

MAP_FILES += \
DATN_STM32_SLAVE_UMM.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
DATN_STM32_SLAVE_UMM.list \

OBJCOPY_HEX += \
DATN_STM32_SLAVE_UMM.hex \


# All Target
all: main-build

# Main-build Target
main-build: DATN_STM32_SLAVE_UMM.elf secondary-outputs

# Tool invocations
DATN_STM32_SLAVE_UMM.elf DATN_STM32_SLAVE_UMM.map: $(OBJS) $(USER_OBJS) D:\ROBOT_RICLAB\4_Chuong_trinh_do_an_STM_DCK\DATN_STM32_SLAVE_UMM\STM32F103C8TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "DATN_STM32_SLAVE_UMM.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\ROBOT_RICLAB\4_Chuong_trinh_do_an_STM_DCK\DATN_STM32_SLAVE_UMM\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="DATN_STM32_SLAVE_UMM.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

DATN_STM32_SLAVE_UMM.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "DATN_STM32_SLAVE_UMM.list"
	@echo 'Finished building: $@'
	@echo ' '

DATN_STM32_SLAVE_UMM.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "DATN_STM32_SLAVE_UMM.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) DATN_STM32_SLAVE_UMM.elf DATN_STM32_SLAVE_UMM.hex DATN_STM32_SLAVE_UMM.list DATN_STM32_SLAVE_UMM.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets

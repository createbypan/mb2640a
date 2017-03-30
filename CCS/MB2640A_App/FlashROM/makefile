################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt"
GEN_CMDS__FLAG := -l"configPkg/linker.cmd"

ORDERED_OBJS += \
"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/CCS/Config/ccsLinkerDefines.cmd" \
"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/common/cc26xx/CCS/cc26xx_ble_app.cmd" \
"./Application/CMD/cmd.obj" \
"./Application/MB2640A.obj" \
"./Application/MB2640A_util.obj" \
"./Application/util.obj" \
"./Application/Ringbuff/ringbuff.obj" \
"./Drivers/PIN/PINCC26XX.obj" \
"./Drivers/UART/UART.obj" \
"./Drivers/UART/UARTCC26XX.obj" \
"./ICall/ICall.obj" \
"./ICall/ICallCC2650.obj" \
"./ICallBLE/ICallBleAPI.obj" \
"./ICallBLE/bleUserConfig.obj" \
"./PROFILES/devinfoservice.obj" \
"./PROFILES/gatt_uuid.obj" \
"./PROFILES/gattservapp_util.obj" \
"./PROFILES/peripheral.obj" \
"./Startup/Board.obj" \
"./Startup/ccfg_appBLE.obj" \
"./Startup/main.obj" \
$(GEN_CMDS__FLAG) \
-llibc.a \
-l"C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600/driverlib/bin/ccs/driverlib.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include Application/CMD/subdir_vars.mk
-include Application/subdir_vars.mk
-include Application/Ringbuff/subdir_vars.mk
-include Drivers/PIN/subdir_vars.mk
-include Drivers/UART/subdir_vars.mk
-include ICall/subdir_vars.mk
-include ICallBLE/subdir_vars.mk
-include PROFILES/subdir_vars.mk
-include Startup/subdir_vars.mk
-include TOOLS/subdir_vars.mk
-include Application/CMD/subdir_rules.mk
-include Application/subdir_rules.mk
-include Application/Ringbuff/subdir_rules.mk
-include Drivers/PIN/subdir_rules.mk
-include Drivers/UART/subdir_rules.mk
-include ICall/subdir_rules.mk
-include ICallBLE/subdir_rules.mk
-include PROFILES/subdir_rules.mk
-include Startup/subdir_rules.mk
-include TOOLS/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
MB2640A_App.out \

EXE_OUTPUTS__QUOTED += \
"MB2640A_App.out" \

BIN_OUTPUTS += \
MB2640A_App.hex \

BIN_OUTPUTS__QUOTED += \
"MB2640A_App.hex" \


# All Target
all: MB2640A_App.out

# Tool invocations
MB2640A_App.out: $(OBJS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/bin/armcl" --cmd_file="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/CCS/MB2640A_App/../../IAR/Application/CC2640/../../../CCS/Config/ccsCompilerDefines.bcfg"  -mv7M3 --code_state=16 -me -Ooff --opt_for_speed=0 --define=USE_ICALL --define=GL_LOG=TRUE --define=MB2640A --define=POWER_SAVING --define=SBP_TASK_STACK_SIZE=700 --define=CMD_TASK_STACK_SIZE=400 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=2672 --define=MAX_PDU_SIZE=27 --define=xTI_DRIVERS_SPI_DMA_INCLUDED --define=xTI_DRIVERS_LCD_INCLUDED --define=ICALL_MAX_NUM_TASKS=5 --define=ICALL_MAX_NUM_ENTITIES=8 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=MAX_NUM_BLE_CONNS=1 --define=CC26XXWARE --define=CC26XX --define=ccs --define=DEBUG -g --gcc --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi -z -m"MB2640A_App.map" --heap_size=0 --stack_size=256 -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/lib" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/include" --reread_libs --diag_suppress=10247-D --diag_wrap=off --display_error_number --warn_sections --xml_link_info="MB2640A_App_linkInfo.xml" --rom_model --compress_dwarf=on --unused_section_elimination=on -o "MB2640A_App.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

MB2640A_App.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/bin/armhex" --memwidth=8 --romwidth=8 --intel -o "MB2640A_App.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)$(GEN_CMDS__QUOTED)
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "Application\CMD\cmd.d" "Application\MB2640A.d" "Application\MB2640A_util.d" "Application\util.d" "Application\Ringbuff\ringbuff.d" "Drivers\PIN\PINCC26XX.d" "Drivers\UART\UART.d" "Drivers\UART\UARTCC26XX.d" "ICall\ICall.d" "ICall\ICallCC2650.d" "ICallBLE\ICallBleAPI.d" "ICallBLE\bleUserConfig.d" "PROFILES\devinfoservice.d" "PROFILES\gatt_uuid.d" "PROFILES\gattservapp_util.d" "PROFILES\peripheral.d" "Startup\Board.d" "Startup\ccfg_appBLE.d" "Startup\main.d" 
	-$(RM) "Application\CMD\cmd.obj" "Application\MB2640A.obj" "Application\MB2640A_util.obj" "Application\util.obj" "Application\Ringbuff\ringbuff.obj" "Drivers\PIN\PINCC26XX.obj" "Drivers\UART\UART.obj" "Drivers\UART\UARTCC26XX.obj" "ICall\ICall.obj" "ICall\ICallCC2650.obj" "ICallBLE\ICallBleAPI.obj" "ICallBLE\bleUserConfig.obj" "PROFILES\devinfoservice.obj" "PROFILES\gatt_uuid.obj" "PROFILES\gattservapp_util.obj" "PROFILES\peripheral.obj" "Startup\Board.obj" "Startup\ccfg_appBLE.obj" "Startup\main.obj" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/bin/armhex" -order MS --memwidth=8 --romwidth=8 --intel -o "MB2640A_App.hex" "MB2640A_App.out"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Drivers/UART/UART.obj: C:/ti/tirtos_simplelink_2_13_00_06/packages/ti/drivers/UART.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/bin/armcl" --cmd_file="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/CCS/MB2640A_App/../../IAR/Application/CC2640/../../../CCS/Config/ccsCompilerDefines.bcfg"  -mv7M3 --code_state=16 -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cmd" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Common" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board/M2640A_7ID" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Stack" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles/CC26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/DevInfo" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/SimpleProfile/CC26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/SimpleProfile" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/applib/heap" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/hci" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/controller/CC26xx/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/host" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/CC2650TIRTOS" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/_common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/osal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/sdata" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/saddr" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/icall/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/include" --include_path="C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600" --define=USE_ICALL --define=MB2640A --define=POWER_SAVING --define=SBP_TASK_STACK_SIZE=700 --define=CMD_TASK_STACK_SIZE=400 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=2672 --define=MAX_PDU_SIZE=27 --define=xTI_DRIVERS_SPI_DMA_INCLUDED --define=xTI_DRIVERS_LCD_INCLUDED --define=ICALL_MAX_NUM_TASKS=4 --define=ICALL_MAX_NUM_ENTITIES=8 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=MAX_NUM_BLE_CONNS=1 --define=CC26XXWARE --define=CC26XX --define=ccs --define=DEBUG -g --gcc --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="Drivers/UART/UART.d" --obj_directory="Drivers/UART" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/UART/UARTCC26XX.obj: C:/ti/tirtos_simplelink_2_13_00_06/packages/ti/drivers/uart/UARTCC26XX.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/bin/armcl" --cmd_file="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/CCS/MB2640A_App/../../IAR/Application/CC2640/../../../CCS/Config/ccsCompilerDefines.bcfg"  -mv7M3 --code_state=16 -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cmd" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Common" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board/M2640A_7ID" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Stack" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles/CC26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/DevInfo" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/SimpleProfile/CC26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/SimpleProfile" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/applib/heap" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/hci" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/controller/CC26xx/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/host" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/CC2650TIRTOS" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/_common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/osal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/sdata" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/saddr" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/icall/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/include" --include_path="C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600" --define=USE_ICALL --define=MB2640A --define=POWER_SAVING --define=SBP_TASK_STACK_SIZE=700 --define=CMD_TASK_STACK_SIZE=400 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=2672 --define=MAX_PDU_SIZE=27 --define=xTI_DRIVERS_SPI_DMA_INCLUDED --define=xTI_DRIVERS_LCD_INCLUDED --define=ICALL_MAX_NUM_TASKS=4 --define=ICALL_MAX_NUM_ENTITIES=8 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=MAX_NUM_BLE_CONNS=1 --define=CC26XXWARE --define=CC26XX --define=ccs --define=DEBUG -g --gcc --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="Drivers/UART/UARTCC26XX.d" --obj_directory="Drivers/UART" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



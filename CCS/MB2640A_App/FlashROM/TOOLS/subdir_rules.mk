################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
build-2106180575: C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/CCS/Config/appBLE.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_31_01_33_core/xs" --xdcpath="C:/ti/tirtos_simplelink_2_13_00_06/packages;C:/ti/tirtos_simplelink_2_13_00_06/products/bios_6_42_00_08/packages;C:/ti/tirtos_simplelink_2_13_00_06/products/uia_2_00_02_39/packages;C:/ti/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC2640F128 -r release -c "C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS" --compileOptions "-mv7M3 --code_state=16 -me -Ooff --opt_for_speed=0 --include_path=\"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/include\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cmd\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/ringbuff\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cust\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/doorlock\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/keypad\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/gpio\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Common\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board/M2640A_7ID\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Application\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Include\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Stack\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/include\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles/CC26xx\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/common/cc26xx\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/applib/heap\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/hci\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/controller/CC26xx/include\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/host\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/CC2650TIRTOS\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/_common/cc26xx\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/include\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/osal/include\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/sdata\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/saddr\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/icall/include\" --include_path=\"C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/include\" --include_path=\"C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600\" --define=USE_ICALL --define=GL_LOG=TRUE --define=MB2640A --define=POWER_SAVING --define=SBP_TASK_STACK_SIZE=800 --define=CMD_TASK_STACK_SIZE=800 --define=CMD_RXTASK_STACK_SIZE=400 --define=KP_TASK_STACK_SIZE=800 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=2672 --define=MAX_PDU_SIZE=27 --define=xTI_DRIVERS_SPI_DMA_INCLUDED --define=xTI_DRIVERS_LCD_INCLUDED --define=ICALL_MAX_NUM_TASKS=6 --define=ICALL_MAX_NUM_ENTITIES=7 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=MAX_NUM_BLE_CONNS=1 --define=CC26XXWARE --define=CC26XX --define=ccs --define=DEBUG -g --gcc --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-2106180575
configPkg/compiler.opt: build-2106180575
configPkg/: build-2106180575



#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.arm.elf.M3{1,0,16.12,0
#
ifeq (,$(MK_NOGENDEPS))
-include package/cfg/appBLE_pem3.oem3.dep
package/cfg/appBLE_pem3.oem3.dep: ;
endif

package/cfg/appBLE_pem3.oem3: | .interfaces
package/cfg/appBLE_pem3.oem3: package/cfg/appBLE_pem3.c package/cfg/appBLE_pem3.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clem3 $< ...
	$(ti.targets.arm.elf.M3.rootDir)/bin/armcl -c  -mv7M3 --code_state=16 -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cmd" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/ringbuff" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cust" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/doorlock" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/keypad" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/gpio" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Common" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board/M2640A_7ID" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Stack" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles/CC26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/applib/heap" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/hci" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/controller/CC26xx/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/host" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/CC2650TIRTOS" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/_common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/osal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/sdata" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/saddr" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/icall/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/include" --include_path="C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600" --define=USE_ICALL --define=GL_LOG=TRUE --define=MB2640A --define=POWER_SAVING --define=SBP_TASK_STACK_SIZE=800 --define=CMD_TASK_STACK_SIZE=800 --define=CMD_RXTASK_STACK_SIZE=400 --define=KP_TASK_STACK_SIZE=800 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=2672 --define=MAX_PDU_SIZE=27 --define=xTI_DRIVERS_SPI_DMA_INCLUDED --define=xTI_DRIVERS_LCD_INCLUDED --define=ICALL_MAX_NUM_TASKS=6 --define=ICALL_MAX_NUM_ENTITIES=7 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=MAX_NUM_BLE_CONNS=1 --define=CC26XXWARE --define=CC26XX --define=ccs --define=DEBUG -g --gcc --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  -qq -pdsw225 -ms --fp_mode=strict --endian=little -mv7M3 --abi=eabi -eo.oem3 -ea.sem3   -Dxdc_cfg__xheader__='"xconfig_appBLE/package/cfg/appBLE_pem3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_16_12_0 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M3.rootDir)/include/rts -I$(ti.targets.arm.elf.M3.rootDir)/include  -fs=./package/cfg -fr=./package/cfg -fc $<
	$(MKDEP) -a $@.dep -p package/cfg -s oem3 $< -C   -mv7M3 --code_state=16 -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cmd" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/ringbuff" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cust" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/doorlock" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/keypad" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/gpio" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Common" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board/M2640A_7ID" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Stack" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles/CC26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/applib/heap" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/hci" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/controller/CC26xx/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/host" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/CC2650TIRTOS" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/_common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/osal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/sdata" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/saddr" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/icall/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/include" --include_path="C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600" --define=USE_ICALL --define=GL_LOG=TRUE --define=MB2640A --define=POWER_SAVING --define=SBP_TASK_STACK_SIZE=800 --define=CMD_TASK_STACK_SIZE=800 --define=CMD_RXTASK_STACK_SIZE=400 --define=KP_TASK_STACK_SIZE=800 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=2672 --define=MAX_PDU_SIZE=27 --define=xTI_DRIVERS_SPI_DMA_INCLUDED --define=xTI_DRIVERS_LCD_INCLUDED --define=ICALL_MAX_NUM_TASKS=6 --define=ICALL_MAX_NUM_ENTITIES=7 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=MAX_NUM_BLE_CONNS=1 --define=CC26XXWARE --define=CC26XX --define=ccs --define=DEBUG -g --gcc --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  -qq -pdsw225 -ms --fp_mode=strict --endian=little -mv7M3 --abi=eabi -eo.oem3 -ea.sem3   -Dxdc_cfg__xheader__='"xconfig_appBLE/package/cfg/appBLE_pem3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_16_12_0 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M3.rootDir)/include/rts -I$(ti.targets.arm.elf.M3.rootDir)/include  -fs=./package/cfg -fr=./package/cfg
	-@$(FIXDEP) $@.dep $@.dep
	
package/cfg/appBLE_pem3.oem3: export C_DIR=
package/cfg/appBLE_pem3.oem3: PATH:=$(ti.targets.arm.elf.M3.rootDir)/bin/;$(PATH)
package/cfg/appBLE_pem3.oem3: Path:=$(ti.targets.arm.elf.M3.rootDir)/bin/;$(PATH)

package/cfg/appBLE_pem3.sem3: | .interfaces
package/cfg/appBLE_pem3.sem3: package/cfg/appBLE_pem3.c package/cfg/appBLE_pem3.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clem3 -n $< ...
	$(ti.targets.arm.elf.M3.rootDir)/bin/armcl -c -n -s --symdebug:none -mv7M3 --code_state=16 -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cmd" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/ringbuff" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cust" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/doorlock" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/keypad" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/gpio" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Common" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board/M2640A_7ID" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Stack" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles/CC26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/applib/heap" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/hci" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/controller/CC26xx/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/host" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/CC2650TIRTOS" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/_common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/osal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/sdata" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/saddr" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/icall/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/include" --include_path="C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600" --define=USE_ICALL --define=GL_LOG=TRUE --define=MB2640A --define=POWER_SAVING --define=SBP_TASK_STACK_SIZE=800 --define=CMD_TASK_STACK_SIZE=800 --define=CMD_RXTASK_STACK_SIZE=400 --define=KP_TASK_STACK_SIZE=800 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=2672 --define=MAX_PDU_SIZE=27 --define=xTI_DRIVERS_SPI_DMA_INCLUDED --define=xTI_DRIVERS_LCD_INCLUDED --define=ICALL_MAX_NUM_TASKS=6 --define=ICALL_MAX_NUM_ENTITIES=7 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=MAX_NUM_BLE_CONNS=1 --define=CC26XXWARE --define=CC26XX --define=ccs --define=DEBUG -g --gcc --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  -qq -pdsw225 --endian=little -mv7M3 --abi=eabi -eo.oem3 -ea.sem3   -Dxdc_cfg__xheader__='"xconfig_appBLE/package/cfg/appBLE_pem3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_16_12_0 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M3.rootDir)/include/rts -I$(ti.targets.arm.elf.M3.rootDir)/include  -fs=./package/cfg -fr=./package/cfg -fc $<
	$(MKDEP) -a $@.dep -p package/cfg -s oem3 $< -C  -n -s --symdebug:none -mv7M3 --code_state=16 -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_16.12.0.STS/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cmd" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/ringbuff" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/cust" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/doorlock" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/keypad" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Application/gpio" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Common" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/Board/M2640A_7ID" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Application" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/SimpleBLEPeripheral/CC26xx/Source/ICall/Stack" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles/CC26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/Profiles/Roles" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Projects/ble/common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/applib/heap" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/hci" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/controller/CC26xx/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/host" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/CC2650TIRTOS" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/target/_common/cc26xx" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/hal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/osal/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/sdata" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/services/saddr" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/icall/include" --include_path="C:/ti/simplelink/ble_cc26xx_2_01_00_44423/Components/ble/include" --include_path="C:/ti/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600" --define=USE_ICALL --define=GL_LOG=TRUE --define=MB2640A --define=POWER_SAVING --define=SBP_TASK_STACK_SIZE=800 --define=CMD_TASK_STACK_SIZE=800 --define=CMD_RXTASK_STACK_SIZE=400 --define=KP_TASK_STACK_SIZE=800 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=2672 --define=MAX_PDU_SIZE=27 --define=xTI_DRIVERS_SPI_DMA_INCLUDED --define=xTI_DRIVERS_LCD_INCLUDED --define=ICALL_MAX_NUM_TASKS=6 --define=ICALL_MAX_NUM_ENTITIES=7 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=MAX_NUM_BLE_CONNS=1 --define=CC26XXWARE --define=CC26XX --define=ccs --define=DEBUG -g --gcc --diag_suppress=48 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  -qq -pdsw225 --endian=little -mv7M3 --abi=eabi -eo.oem3 -ea.sem3   -Dxdc_cfg__xheader__='"xconfig_appBLE/package/cfg/appBLE_pem3.h"'  -Dxdc_target_name__=M3 -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_16_12_0 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M3.rootDir)/include/rts -I$(ti.targets.arm.elf.M3.rootDir)/include  -fs=./package/cfg -fr=./package/cfg
	-@$(FIXDEP) $@.dep $@.dep
	
package/cfg/appBLE_pem3.sem3: export C_DIR=
package/cfg/appBLE_pem3.sem3: PATH:=$(ti.targets.arm.elf.M3.rootDir)/bin/;$(PATH)
package/cfg/appBLE_pem3.sem3: Path:=$(ti.targets.arm.elf.M3.rootDir)/bin/;$(PATH)

clean,em3 ::
	-$(RM) package/cfg/appBLE_pem3.oem3
	-$(RM) package/cfg/appBLE_pem3.sem3

appBLE.pem3: package/cfg/appBLE_pem3.oem3 package/cfg/appBLE_pem3.mak

clean::
	-$(RM) package/cfg/appBLE_pem3.mak
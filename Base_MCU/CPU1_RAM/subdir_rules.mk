################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
epwm_up_aq_cpu01.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_examples_Cpu1/epwm_up_aq/cpu01/epwm_up_aq_cpu01.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="Base_MCU.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_CodeStartBranch.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/source/F2837xS_CodeStartBranch.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_CodeStartBranch.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_CpuTimers.obj: C:/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_common/source/F2837xS_CpuTimers.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_CpuTimers.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_DefaultISR.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/source/F2837xS_DefaultISR.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_DefaultISR.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_EPwm.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/source/F2837xS_EPwm.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_EPwm.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_GlobalVariableDefs.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/source/F2837xS_GlobalVariableDefs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_GlobalVariableDefs.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_Gpio.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/source/F2837xS_Gpio.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_Gpio.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_PieCtrl.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/source/F2837xS_PieCtrl.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_PieCtrl.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_PieVect.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/source/F2837xS_PieVect.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_PieVect.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_Spi.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/source/F2837xS_Spi.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_Spi.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_SysCtrl.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/source/F2837xS_SysCtrl.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_SysCtrl.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

F2837xS_usDelay.obj: C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/source/F2837xS_usDelay.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="F2837xS_usDelay.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

metaTait_HighLevel.obj: ../metaTait_HighLevel.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="metaTait_HighLevel.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

metaTait_I2C.obj: ../metaTait_I2C.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="metaTait_I2C.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

metaTait_ISR.obj: ../metaTait_ISR.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="metaTait_ISR.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

metaTait_PWM.obj: ../metaTait_PWM.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="metaTait_PWM.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

metaTait_SCI.obj: ../metaTait_SCI.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="metaTait_SCI.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

metaTait_SDCard.obj: ../metaTait_SDCard.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="C:/Users/camil/Google Drive/My Capstone/Git/Base_MCU" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_headers/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v200/F2837xS_common/include" --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="metaTait_SDCard.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



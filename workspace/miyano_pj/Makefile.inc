NEWFILE_COBJS = M_CTL/ctl_main.o M_CTL/linetrace_run.o M_CTL/const_run.o D_DEVICE/comm.o D_DEVICE/drive_mtr.o D_DEVICE/arm_mtr.o D_DEVICE/color_snc.o D_DEVICE/sonic_snc.o D_DEVICE/button.o
APPL_COBJS += $(addprefix , $(NEWFILE_COBJS))

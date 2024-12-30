CROSS?= 
KERNEL_DIR?= /usr/src/linux-headers-5.15.0-124-generic
CURRENT_PATH:= $(shell pwd)
MODULE_NAME1= hw_break
MODULE_NAME2= jump_table

src_dir?= $(shell pwd)
export src_dir


includedir:= -I$(src_dir)/include
EXTRA_CFLAGS+= $(includedir) -g


obj-m:= $(MODULE_NAME1).o
$(MODULE_NAME1)-objs+= 	hw_breakpoint.o \
						hw_breakpoint_manage.o \
    					hw_breakpoint_proc.o \
    					hw_breakpoint_smp.o \
						hw_breakpoint_until.o \
    
obj-m+= $(MODULE_NAME2).o
# $(MODULE_NAME2)-objs+= jump_table.o

all: ko
# 编译驱动
ko:
	make -C $(KERNEL_DIR) M=$(CURRENT_PATH) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" CROSS_COMPILE=${CROSS} ARCH=arm64 modules


clean:
	make -C $(KERNEL_DIR) M=$(CURRENT_PATH) clean
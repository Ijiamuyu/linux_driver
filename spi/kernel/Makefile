
KDIR := $(KERNEL_DIR)

# 模块名
obj-m += spi_master.o

# 编译目标
all:
	make -C $(KDIR) M=$(PWD) modules

# 清理目标
clean:
	make -C $(KDIR) M=$(PWD) clean

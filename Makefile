KERNEL_SRC = /lib/modules/$(shell uname -r)/build

obj-m += my_module.o

all:
make -C $(KERNEL_SRC) M=$(PWD) modules

clean:
make -C $(KERNEL_SRC) M=$(PWD) clean
obj-m += servo_driver_default.o
KDIR = ~/working/kernel

#obj-m += led_button_driver.o
#KDIR = ~/working/kernel

RESULT = servo_example
SRC = $(RESULT).c
#RESULT2 = seg_example2
#SRC2 = $(RESULT2).c

CCC = arm-linux-gnueabihf-gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT) $(SRC)
#	$(CCC) -o $(RESULT2) $(SRC2)

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT)
#	rm -f $(RESULT2)

obj-m += step_driver.o
obj-m += led_button_driver.o
KDIR = ~/kernel
RESULT = program
SRC = $(RESULT).c

CCC = gcc


all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT) $(SRC)
	
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT)

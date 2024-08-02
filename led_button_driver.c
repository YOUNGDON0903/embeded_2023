#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple gpio driver for setting a LED and reading a button");

/* Variables for device and device class */
static dev_t my_device_nr_2;
static struct class *my_class_2;
static struct cdev my_device_2;

#define DRIVER_NAME "led_button_driver"
#define DRIVER_CLASS "LED_BUTTON_Class"

/**
 * @brief Read data out of the buffer
 */
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	short tmp;
	tmp = 0;
	/* Get amount of data to copy */
	to_copy = min(count, sizeof(tmp));

	/* Read value of button */
	tmp += gpio_get_value(5);
	tmp += 2*gpio_get_value(6);

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, &tmp, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	char value;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));

	/* Copy data to user */
	not_copied = copy_from_user(&value, user_buffer, to_copy);

	/* Setting the LED */
	switch(value) {
		case '0':
			gpio_set_value(26, 0);
			break;
		case '1':
			gpio_set_value(26, 1);
			break;
		case '2':
			gpio_set_value(19, 1);
			break;
		case '3':
			gpio_set_value(19, 0);
			break;
		default:
			printk("Invalid Input!\n");
			break;
	}

	/* Calculate data */
	delta = to_copy - not_copied;
	return delta;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("led_button - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("led_button - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("Hello, Kernel!\n");

	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr_2, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr_2 >> 20, my_device_nr_2 && 0xfffff);

	/* Create device class */
	if((my_class_2 = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not e created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class_2, NULL, my_device_nr_2, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device_2, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device_2, my_device_nr_2, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	/* GPIO 26 init */
	if(gpio_request(26, "rpi-gpio-26")) {
		printk("Can not allocate GPIO 26\n");
		goto AddError;
	}

	/* Set GPIO 26 direction */
	if(gpio_direction_output(26, 0)) {
		printk("Can not set GPIO 26 to output!\n");
		goto Gpio26Error;
	}

	/* GPIO 19 init */
	if(gpio_request(19, "rpi-gpio-19")) {
		printk("Can not allocate GPIO 19\n");
		goto AddError;
	}

	/* Set GPIO 19 direction */
	if(gpio_direction_output(19, 0)) {
		printk("Can not set GPIO 19 to output!\n");
		goto Gpio19Error;
	}

	/* GPIO 5 init */
	if(gpio_request(5, "rpi-gpio-5")) {
		printk("Can not allocate GPIO 5\n");
		goto AddError;
	}

	/* Set GPIO 5 direction */
	if(gpio_direction_input(5)) {
		printk("Can not set GPIO 5 to input!\n");
		goto Gpio5Error;
	}
	/* GPIO 6 init */
	if(gpio_request(6, "rpi-gpio-6")) {
		printk("Can not allocate GPIO 6\n");
		goto AddError;
	}

	/* Set GPIO 6 direction */
	if(gpio_direction_input(6)) {
		printk("Can not set GPIO 6 to input!\n");
		goto Gpio6Error;
	}


	return 0;

Gpio26Error:
	gpio_free(26);
Gpio19Error:
	gpio_free(19);
Gpio5Error:
	gpio_free(5);
Gpio6Error:
	gpio_free(6);
AddError:
	device_destroy(my_class_2, my_device_nr_2);
FileError:
	class_destroy(my_class_2);
ClassError:
	unregister_chrdev_region(my_device_nr_2, 1);
	return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	gpio_set_value(26, 0);
	gpio_set_value(19, 0);
	gpio_free(5);
	gpio_free(6);
	gpio_free(19);
	gpio_free(26);
	cdev_del(&my_device_2);
	device_destroy(my_class_2, my_device_nr_2);
	class_destroy(my_class_2);
	unregister_chrdev_region(my_device_nr_2, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);



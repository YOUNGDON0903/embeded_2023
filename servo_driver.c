#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple gpio driver for segments");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

static int set_servo_angle;
static struct hrtimer servo_timer;
static ktime_t ktime_period;

#define DRIVER_NAME "my_segment"
#define DRIVER_CLASS "MyModuleClass_seg"

#define Servo_GPIO 2

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	unsigned short value = 0;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));	//copy user input to com

	/* Copy data to user */
	not_copied = copy_from_user(&value, user_buffer, to_copy);	//if, it's not 0, some of input not copied

	/* Setting the segments LED */
	/*
	if(value & (1 << 0)){
		gpio_set_value(2, 1);	//gpio pin ON
	}
	else{
		gpio_set_value(2, 0);	//gpio pin OFF 
	}
	*/

	sscanf(buffer, "%d", &angle);

    // Set servo angle
	//
	int pulse_width_us = (angle * 1000 / 180) + 1000; // 1ms to 2ms
    ktime_period = ktime_set(0, pulse_width_us * 1000); // convert us to ns
    hrtimer_start(&servo_timer, ktime_period, HRTIMER_MODE_REL);

	
	
	/* Calculate data */
	delta = to_copy - not_copied;
	return delta;
}




/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("servo motor - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("servo motor - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	//.read = driver_read,
	.write = driver_write
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("Hello, Kernel!\n");

	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

	/* Create device class */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not e created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	/* Set D1~4 segments GPIO */
	/* GPIO 2 init */
	if(gpio_request(2, "rpi-gpio-2")) {
		printk("Can not allocate GPIO 2\n");
		goto AddError;
	}

	/* Set GPIO 2 direction */
	if(gpio_direction_output(2, 0)) {
		printk("Can not set GPIO 2 to output!\n");
		goto Gpio2Error;
	}
	/* GPIO 3 init */
	if(gpio_request(3, "rpi-gpio-3")) {
		printk("Can not allocate GPIO 3\n");
		goto AddError;
	}

	return 0;
	//just for using gpio
Gpio2Error:
	gpio_free(Servo_GPIO);
	gpio_free(2);
	/*
Gpio3Error:
	gpio_free(3);
Gpio4Error:
	gpio_free(4);
Gpio17Error:
	gpio_free(17);
Gpio21Error:
	gpio_free(21);
Gpio20Error:
	gpio_free(20);
Gpio16Error:
	gpio_free(16);
Gpio12Error:
	gpio_free(12);
Gpio7Error:
	gpio_free(7);
Gpio8Error:
	gpio_free(8);
Gpio25Error:
	gpio_free(25);
Gpio24Error:
	gpio_free(24);

AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	//just for using gpio
	gpio_set_value(Servo_GPIO, 0);
	/*
	gpio_set_value(3, 0);
	gpio_set_value(4, 0);
	gpio_set_value(17, 0);
	gpio_set_value(21, 0);
	gpio_set_value(20, 0);
	gpio_set_value(16, 0);
	gpio_set_value(12, 0);
	gpio_set_value(7, 0);
	gpio_set_value(8, 0);
	gpio_set_value(25, 0);
	gpio_set_value(24, 0);
	*/
	gpio_free(Servo_GPIO);
	/*
	gpio_free(3);
	gpio_free(4);
	gpio_free(17);
	gpio_free(21);
	gpio_free(20);
	gpio_free(16);
	gpio_free(12);
	gpio_free(7);
	gpio_free(8);
	gpio_free(25);
	gpio_free(24);
	*/
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

static int set_servo_angle(int angle) {
    // Convert angle to appropriate PWM signal here
    // Example conversion (may need adjustment for your servo):
    // 0 degrees = 1ms pulse, 180 degrees = 2ms pulse
    // Pulse period for servo is typically 20ms
    
}
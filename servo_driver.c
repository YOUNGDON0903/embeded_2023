#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/pwm.h>
#include <linux/timer.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple driver to access the Hardware PWM IP");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;
static struct timer_list my_pwm_timer;

#define DRIVER_NAME "my_servo_driver"
#define DRIVER_CLASS "MyModuleClass"
#define PWM_ACTIVE_TIME 1

/*timer callback - run when timeout*/
static void my_pwm_timer_callback(struct timer_list *t) {
    pwm_disable(pwm0);
    printk("PWM is now disabled\n");
}
/* Variables for pwm  */
struct pwm_device *pwm0 = NULL;
u32 pwm_on_time = 50000000;

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	short value;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));

	/* Copy data to user */
	not_copied = copy_from_user(&value, user_buffer, to_copy);
	
	/* Set PWM on time */
	if(value < 0 || value > 10){
		printk("Invalid Value\n");
	}
	else{
		pwm_config(pwm0,  5000000 * value + 50000000, 1000000000); 
		//period - 10e9  0 degree - 5*10e7  180 degree - 10e8

		/*set timer*/
		timer_setup(&my_pwm_timer, my_pwm_timer_callback, 0);
		mod_timer(&my_pwm_timer, jiffies + msecs_to_jiffies(PWM_ACTIVE_TIME));

		pwm_enable(pwm0);
	}

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
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
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", MAJOR(my_device_nr), MINOR(my_device_nr));

	/* Create device class */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
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
	printk("flag1\n");
	pwm0 = pwm_request(0, "my-pwm");
	if(pwm0 == NULL) {
		printk("Could not get PWM0!\n");
		goto AddError;
	}
	pwm_disable(pwm0);

	pwm_config(pwm0, pwm_on_time, 1000000000);

	timer_setup(&my_pwm_timer, my_pwm_timer_callback, 0);
	mod_timer(&my_pwm_timer, jiffies + msecs_to_jiffies(PWM_ACTIVE_TIME));
	printk("flag2_just before enable\n");
	pwm_enable(pwm0);

	return 0;

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
	del_timer(&my_pwm_timer); // delete timer
	pwm_disable(pwm0);
	pwm_free(pwm0);
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);


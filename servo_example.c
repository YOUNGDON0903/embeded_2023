#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

//#include "tflite.h"   -> get func detect -> return X and class (with structure?)

int main(int argc, char **argv) {	//input int -> argc , char -> argv

	short buff;

	int servo = open("/dev/my_servo", O_WRONLY); // if you want read='O_RDONLY' write='O_WRONLY', read&write='O_RDWR'
	int led_button = open("/dev/my_led_button", O_RDWR);

	float returned[2] = detect()
	


	if(argv[1][0] == '0' && (argv[1][1] == 'x' || argv[1][1] == 'X'))
		buff = (unsigned short)strtol(&argv[1][2], NULL, 16);
	else
		buff = (unsigned short)strtol(&argv[1][0], NULL, 10);
	

	write(servo, &buff, 2); //byte size 2, driver's count arg

	close(servo);
	close(led_button);
	return 0;
}

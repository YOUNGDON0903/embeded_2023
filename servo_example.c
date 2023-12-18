#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

//#include "tflite.h"   -> get func detect -> return X and class (with structure?)

int main(int argc, char **argv) {	// argc:count of input ,  argv:input

	short buff;
	
	int servo = open("/dev/my_servo", O_WRONLY); // if you want read='O_RDONLY' write='O_WRONLY', read&write='O_RDWR'


	if(argc < 2){
		printf("put arg 0x0000 or int\n");
		return -1;
	}

	if(servo == -1) {
		printf("Opening was not possible!\n");
		return -1;
	}
	printf("Opening was successfull!\n");
	
	buff = (unsigned short)strtol(&argv[1][0], NULL, 10);


	write(servo, &buff, 2); //byte size 2, driver's count arg=-0']h=0
	close(servo);
	//close(led_button);
	return 0;
}

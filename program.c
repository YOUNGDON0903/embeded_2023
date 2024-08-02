#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#define PIPE "./my_pipe"
#define BUFFER_SIZE 128


int main(int argc, char **argv) {

    short buff; 
	short buff_1;
    short buff_2;
	short prev = 0;

    short step_input; //right or left of step motor
    char mode = '1'; //mode of camera and it's same with led state
    char direction;
    char buffer[BUFFER_SIZE];

    int step = open("/dev/step_driver", O_WRONLY); // if you want read='O_RDONLY' write='O_WRONLY', read&write='O_RDWR'
    int ledbutton = open("/dev/led_button_driver",O_RDWR);
    int pipe_fd;

    char cmd[256];
	time_t now;
    struct tm *tm_info;
    char time_str[100];
    
    char on = '2';
    char off = '3';
	
    
    //capture count initialize
    int cnt = 0;
    

    if (ledbutton == -1) {
        printf("Opening was not possible!\n");
    }

	if(step == -1) {
		printf("Opening was not possible!\n");
		return -1;
	}
	printf("Opening was successfull!\n");
	

    pipe_fd = open(PIPE, O_RDONLY);
    if (pipe_fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

	write(ledbutton, &mode, 1);
	while (1) {
		read(ledbutton, &buff, 2);
		buff_1 = buff % 2;
		buff_2 = buff - buff_1;
		if (buff_1 == 1 && prev == 0) {
			if (mode == '1') {
				mode = '0';
				write(ledbutton, &mode, 1);
			}
			else {
				mode = '1';
				write(ledbutton, &mode, 1);
			}
		}
		prev = buff_1;
		if (buff_2 == 1) {   //button(6) function


		}


			
		ssize_t bytes_read = read(pipe_fd,buffer, BUFFER_SIZE-1);

		if(bytes_read > 0){
			buffer[bytes_read] = '\0';
			printf("Received: %s\n", buffer);

			if(mode == '1'){
				//read pipe
				if(*buffer == '1'){
					//left
					write(ledbutton,&off,1);
					direction = 'L';
					write(step, &direction, 1);
					
				}else if(*buffer == '2'){
					//right
					write(ledbutton,&off,1);
					direction = 'R';
					write(step, &direction, 1);
				}else if(*buffer == '3'){
					//stop
					direction = 'S';
					write(step,&direction,1);
					
				}else if(*buffer == '4'){
					//capture start
					    write(ledbutton,&on,1);
				
					
				}else if(*buffer == '5'){
					//capture complete
					    write(ledbutton,&off,1);
					
				    }
					
					
					
					/*cnt++;
					 * if(cnt>3){
					    //photo ready
					    write(ledbutton,&on,1);
					    cnt = 0;
					    }
					*/
				    
					
					
				}
			
			}
			
		}
    

    close(pipe_fd);
    close(ledbutton);
	close(step);
	return 0;
}


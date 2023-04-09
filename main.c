/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

#define PS2_BASE 0XFF200100	

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>


// Begin part3.c code for Lab 7


volatile int pixel_buffer_start; // global variable

void wait_for_vsync();
void clear_screen();
void swap (int *a, int *b);

void setRandKeys(int num, int *array);
int map(int val);

void wait_for_vsync() {
	volatile int* pixel_ctrl_ptr = (int *)0xFF203020; //pixel controller
	register int status;
	
	*pixel_ctrl_ptr = 1;
	
	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0) {
		status = *(pixel_ctrl_ptr + 3);
	}
}	

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer


    //start of program
    int level = 1
    bool failed = false;
	bool pressed = false;
	int* keys[20];
	unsigned char byte2 = 0;
	unsigned char byte3 = 0;

	volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address
	int PS2_data, RVALID;

	//starting screen
	//promt user to press any key to start
	while(pressed = false){
		PS2_data = *(PS2_ptr);
		RVALID = (PS2_data & 0x8000);	// extract the RVALID field
		if (RVALID != 0)
		{
			/* always save the last 2 bytes received */
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
		}
		if(byte3 != 0){
			//key pressed
			pressed = true;
		}

	}

    while(true){
		//level screen
        setRandKeys(level, keys);
        //print keys to display

		bool levelDone = false;
		bool failed = false;
		int counter = 1;

		while(!levelDone && !failed){
			//read key input//
			PS2_data = *(PS2_ptr);
			RVALID = (PS2_data & 0x8000);	// extract the RVALID field//
			if (RVALID != 0)
			{
				/* always save the last 2 bytes received *///
				byte2 = byte3;
				byte3 = PS2_data & 0xFF;
			}

			if(byte3 == 240){
				continue;
			}


			//check key input to match array//
			if(map(byte3) == keys[counter]){
				//display check mark
			}
			else{
				failed = true;
			}
			
			if(counter = level){
				levelDone = true;
			}
			else{
				counter++;
			}
		}

		if(failed){
			//fail/game over screen
			level = 1;
			continue;
		}
		else{
			//pass screen
			level++;
			continue;
		}

		if(level > 20){
			//win screen
		}

    }
	
}

// code for subroutines (not shown)
void setRandKeys(int num, int *array){
	srand(time(NULL));
	for(int i = 0 ; i < num; i++){
		array[i] = rand() % 26 + 1;
	}
	return;
}

int map(int val){
	//array to map all PS2 inputs to letter codes, A is 1 and Z is 26
	int mapArray[] = {28, 50, 33, 35, 36, 43, 52, 51, 67, 59, 66, 75, 
	58, 49, 68, 77, 21, 45, 27, 44, 60, 42, 29, 34, 53, 26};

	int size = sizeof(mapArray) / sizeof(int);

	for(int i = 0; i < size; i++){
		if(val == mapArray[i]){
			//if value is in map array, return number associated with PS2 input code
			return i + 1;
		}
	}
	//no value found in map, unexpected input
	return -1;
}

void clear_screen() {
	for (int i=0; i<320; i++) {
		for (int j=0; j<240; j++) {
			plot_pixel(i, j, 0x0000);
		}
	}
}

void swap (int *a, int *b)
	{
		int temp = 0;
		temp = *a;
		*a = *b;
		*b = temp;	
	}
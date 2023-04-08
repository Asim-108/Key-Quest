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


// Begin part3.c code for Lab 7


volatile int pixel_buffer_start; // global variable

void wait_for_vsync();
void erase(int posx[], int posy[]);
void drawBox(int xpos, int ypos, int colour);
void drawPattern(int posx[], int posy[], int colour[]);
void update(int posx[], int posy[], int dx[], int dy[]);
void plot_pixel(int x, int y, short int line_color);
void drawLine(int x0, int y0, int x1, int y1, int colour);
void clear_screen();
void swap (int *a, int *b);
int* setRandKeys(level);
bool levelPassed(keys, level);

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
    // // declare other variables(not shown)
	// int dx_box[NUM_BOXES], dy_box[NUM_BOXES], c_box[NUM_BOXES], posx_box[NUM_BOXES], posy_box[NUM_BOXES];
	// int prevx[NUM_BOXES], prevy[NUM_BOXES];
	// int pprevx[NUM_BOXES], pprevy[NUM_BOXES];
	
	// int colours[] = {WHITE, YELLOW, RED, GREEN, BLUE, CYAN, MAGENTA, GREY, PINK, ORANGE};
    // // initialize location and direction of rectangles(not shown)
	
	// for(int i = 0; i < NUM_BOXES; i++){
	// 	dx_box[i] = rand() % 2 * 2 - 1;
	// 	dy_box[i] = rand() % 2 * 2 - 1;
	// 	c_box[i] = colours[rand() % 10];
	// 	posx_box[i] = rand() % 317;
	// 	posy_box[i] = rand() % 237;
	// }
	
	// for(int i = 0; i < NUM_BOXES; i++){
	// 	prevx[i] = posx_box[i];
	// 	prevy[i] = posy_box[i];
	// }
	
	// for(int i = 0; i < NUM_BOXES; i++){
	// 	pprevx[i] = posx_box[i];
	// 	pprevy[i] = posy_box[i];
	// }



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
    while(true){
        //print key sequence
        int* keys = setRandKeys(level);
        //print keys to display


        if(levelPassed(keys, level) == false){
            //failed screen
            level = 1;
            continue;
        }
        else{
            //level passed screen
            level += 1;
        }
    }

    

    while (1)
    {
        
		
		/* Erase any boxes and lines that were drawn in the last iteration */
		//erase(posx_box, posy_box);
		//clear_screen();
		
		erase(pprevx, pprevy);

        // code for drawing the boxes and lines (not shown)
		drawPattern(posx_box, posy_box, c_box);
        // code for updating the locations of boxes (not shown)
		
		for(int i = 0; i < NUM_BOXES; i++){
			pprevx[i] = prevx[i];
			pprevy[i] = prevy[i];
		}
		
		for (int i = 0; i < NUM_BOXES; i++){
			prevx[i] = posx_box[i];
			prevy[i] = posy_box[i];
		}
		
		update(posx_box, posy_box, dx_box, dy_box);	
		
		//*(pixel_ctrl_ptr) = pixel_buffer_start;
		
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

// code for subroutines (not shown)
setRandKeys(int num){
    //return pointer to array of random numbers, length of num
}

bool levelPassed(keys, level){
    //read key inputs from user
}

void erase(int posx[], int posy[]){
	for(int i = 0; i < NUM_BOXES; i++){
		drawBox(posx[i], posy[i], 0x0000);
		drawLine(posx[i], posy[i], posx[(i+1)%NUM_BOXES], posy[(i+1)%NUM_BOXES], 0x0000);
	}
}

void drawBox(int xpos, int ypos, int colour){
	
	for (int i = 0; i < BOX_LEN; i++){
		for(int j = 0; j < BOX_LEN; j++){
			plot_pixel(xpos + i, ypos + j, colour);
		}
	}
}

void drawPattern(int posx[], int posy[], int colour[]){
	for(int i = 0; i < NUM_BOXES; i++){
		drawBox(posx[i], posy[i], colour[i]);
		drawLine(posx[i], posy[i], posx[(i+1)%NUM_BOXES], posy[(i+1)%NUM_BOXES], colour[i]);
	}
}

void update(int posx[], int posy[], int dx[], int dy[]){
	for(int i = 0; i < NUM_BOXES; i++){
		posx[i] += dx[i];
		posy[i] += dy[i];
		
		if(posx[i] == 0 && dx[i] == -1){
			dx[i] = 1;
		}
		else if(posx[i] == 318 && dx[i] == 1){
			dx[i] = -1;
		}
		
		if(posy[i] == 0 && dy[i] == -1){
			dy[i] = 1;
		}
		else if(posy[i] == 238 && dy[i] == 1){
			dy[i] = -1;
		}
	}
}



void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void drawLine(int x0, int y0, int x1, int y1, int colour)
{
	bool is_steep = ABS(y1 - y0) > ABS(x1 - x0);
		if (is_steep) {
			swap(&x0, &y0);
			swap(&x1, &y1);
		}
		if (x0 > x1) {
			swap(&x0, &x1);
			swap(&y0, &y1);
		}		

	int deltax = x1 - x0;
	int deltay = y1 - y0;
	if (deltay <0) 
        deltay = -deltay;
	
	int error = -(deltax / 2);
	int y = y0;
		for (int x=x0; x < x1; x++){
			if (is_steep == true){
				plot_pixel(y, x, colour);
			}
			else {
				plot_pixel(x, y, colour);
			}
			error = error + deltay;
			if (error >= 0) {
				int y_step=0;
				if (y0 < y1) {
					y_step =1;
				}
				else {
					y_step = -1;
				}
				y = y + y_step;
				error = error - deltax;
			}
		
		}
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
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

// Begin part1.s for Lab 7

volatile int pixel_buffer_start; // global variable

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
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
// code not shown for clear_screen() and draw_line() subroutines
void draw_line(int x0, int y0, int x1, int y1, int colour)
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
	
void draw_check_mark(int x0, int y0, int size, short int color) {
    draw_line(x0, y0 + size / 2, x0 + size / 3, y0 + size, color);
    draw_line(x0 + size / 3, y0 + size, x0 + size, y0, color);
}

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen();
    draw_check_mark(120, 80, 70, 0x07E0);
	
	while(1){
    }
	
	return 0;
}


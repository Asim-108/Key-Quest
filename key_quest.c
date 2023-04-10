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
#define HEX_BASE 0XFF200020

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

#define DELAY_CONST 2000000000

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

//include statements for array codes
#include draw_check_mark.h
#include letter_array_codes.h
#include level_array_codes.h
#include more_displays.h

// Begin code

volatile int pixel_buffer_start; // global variable

void wait_for_vsync();
void clear_screen();
void plot_pixel(int x, int y, short int line_color);


void setRandKeys(int num, int *array);
int map(int val);
int hexNum(int num);

void wait_for_vsync() {
	volatile int* pixel_ctrl_ptr = (int *)0xFF203020; //pixel controller
	register int status;
	
	*pixel_ctrl_ptr = 1;
	
	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0) {
		status = *(pixel_ctrl_ptr + 3);
	}
}	

void clear_screen() {
	for (int i=0; i<320; i++) {
		for (int j=0; j<240; j++) {
			plot_pixel(i, j, 0x0000);
		}
	}
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
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

  volatile int * hex_ptr = (int* )0XFF200020;

  //start of program
  int level = 1;
  bool failed = false;
	bool pressed = false;
	int keys[10];
	unsigned char byte2 = 0;
	unsigned char byte3 = 0;

	volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address
	int PS2_data, RVALID;

	//display starting screen
	int index = 0;
	for(int y = 0; y < RESOLUTION_Y; y++){
		for(int x = 0; x < RESOLUTION_X; x++){
			plot_pixel(x, y, press_start[index]);
			index++;
		}
	}
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer


	//promt user to press any key to start
	while(pressed == false){
		PS2_data = *(PS2_ptr);
		RVALID = (PS2_data & 0x8000);	// extract the RVALID field
		if (RVALID != 0)
		{
			/* always save the last 2 bytes received */
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
		}
		if(byte2 != 0){
			//key pressed
			pressed = true;
		}

	}

  //main gameplay loop
  while(true){

    //drawing level background
		index = 0;
		for(int y = 0; y < RESOLUTION_Y; y++){
			for(int x = 0; x < RESOLUTION_X; x++){
				plot_pixel(x, y, level_background[index]);
				index++;
			}
		}

    *(hex_ptr) = hexNum(level);

    //drawing level number
    index = 0;
    for(int y = 0; y < 40; y++){
      for(int x = 0; x < 60; x++){
        if(levelsArray[level - 1][index] == (short int)0xffff) {
          index ++;
          continue; 
        }
        plot_pixel(x + 220, y + 80, levelsArray[level - 1][index]);
        index++;
      }
    }
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

    //hardware delay loop
    for(int i = 0; i < DELAY_CONST; i++){
      continue;
    }

    //setting random keys
    setRandKeys(level, keys);

    //clearing backbuffer
    clear_screen();
    //printing random letters to screen
    for(int i = 0; i < level; i++){
      index = 0;
      for(int y = 0; y < 60; y++){
        for(int x = 0; x < 40; x++){
          if(lettersArray[keys[i]][index] == (short int)0xffff) {
            index ++;
            continue; 
          }
          plot_pixel(x + 140, y + 90, lettersArray[keys[i]][index]);
          index++;
        }
      }
      wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
      clear_screen();
      //hardware delay loop
      for(int i = 0; i < DELAY_CONST/2; i++){
        continue;
      }
    }

    clear_screen();
    wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

    

		bool levelDone = false;
		bool failed = false;
		int counter = 1;

    //reading key inputs
		while(!levelDone && !failed){
			PS2_data = *(PS2_ptr);
			RVALID = (PS2_data & 0x8000);	// extract the RVALID field//
			if (RVALID != 0)
			{
				/* always save the last 2 bytes received *///
				byte2 = byte3;
				byte3 = PS2_data & 0xFF;
			}
      //if no key pressed, continue until input recieved
			if(byte2 == 240){
				continue;
			}

			//check key input to match array//
      printf("%d, %d\n", map(byte3), keys[counter - 1]);

			if(map(byte3) == keys[counter - 1]){
        //plot checkmark
        index = 0;
        for(int y = 0; y < 79; y++){
          for(int x = 0; x < 79; x++){
            if(check_mark[index] == (short int)0xffff) {
              index ++;
              continue; 
            }
            plot_pixel(x + 120, y + 80, check_mark[index]);
            index++;
          }
        }
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

        //display checkmark for a short time
        for(int i = 0; i < DELAY_CONST/4; i++){
          continue;
        }
        //clear checkmark from screen
        clear_screen();
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

			}
      //if not matched letter, level failed
			else{
				failed = true;
			}
			
      //if couter reaches level, you're done the level
			if(counter == level){
				levelDone = true;
			}
      //if not, increase the counter
			else{
				counter++;
			}

      //keep polling while a key is held down to avoid multiple inputs
      while(byte2 != 240){
        PS2_data = *(PS2_ptr);
        RVALID = (PS2_data & 0x8000);	// extract the RVALID field//
        if (RVALID != 0)
        {
          /* always save the last 2 bytes received *///
          byte2 = byte3;
          byte3 = PS2_data & 0xFF;
        }
      }
      
		}

		if(failed){
      clear_screen();
      index = 0;
      for(int y = 0; y < RESOLUTION_Y; y++){
        for(int x = 0; x < RESOLUTION_X; x++){
          plot_pixel(x, y, game_over[index]);
          index++;
        }
		  }
      wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
      for(int i = 0; i < DELAY_CONST; i++){
          continue;
      }
			level = 1;

		}
		else{
      if(level == 10){
        level++;
      }
      else{
        clear_screen();
        index = 0;
        for(int y = 0; y < RESOLUTION_Y; y++){
          for(int x = 0; x < RESOLUTION_X; x++){
            plot_pixel(x, y, level_up[index]);
            index++;
          }
        }
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
        for(int i = 0; i < DELAY_CONST; i++){
            continue;
        }
        level++;
      }
		}

		if(level > 10){
			clear_screen();
      index = 0;
      for(int y = 0; y < RESOLUTION_Y; y++){
        for(int x = 0; x < RESOLUTION_X; x++){
          plot_pixel(x, y, win_game[index]);
          index++;
        }
		  }
      wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
      //infinite loop, idle program here
      while(1){
        //loop
      }
		}

  }
}

// code for subroutines (not shown)
void setRandKeys(int num, int *array){
	srand(time(NULL));
	for(int i = 0 ; i < num; i++){
    array[i] = rand() % 26;
    if(i > 0){
      while(array[i] == array[i - 1]){
        array[i] = rand() % 26;
      }
    }
	}
	return;
}

int map(int val){
	//array to map all PS2 inputs to letter codes, A is 0 and Z is 25
	int mapArray[] = {28, 50, 33, 35, 36, 43, 52, 51, 67, 59, 66, 75, 
	58, 49, 68, 77, 21, 45, 27, 44, 60, 42, 29, 34, 53, 26};

	int size = sizeof(mapArray) / sizeof(int);
	
	for(int i = 0; i < size; i++){
		if(val == mapArray[i]){
			//if value is in map array, return number associated with PS2 input code
			return i;
		}
	}
	//no value found in map, unexpected input
	return -1;
}

int hexNum(int num){
  int displayVal;
  switch(num){
    case 1:
      displayVal = 6;
      break;
    case 2:
      displayVal = 91;
      break;
    case 3:
      displayVal = 79;
      break;
    case 4:
      displayVal = 102;
      break;
    case 5:
      displayVal = 109;
      break;
    case 6:
      displayVal = 125;
      break;
    case 7:
      displayVal = 7;
      break;
    case 8:
      displayVal = 127;
      break;
    case 9:
      displayVal = 103;
      break;
    case 10:
      displayVal = 1599;
      break;
    default:
      displayVal = 0;
      break;
  }

  return displayVal;
}
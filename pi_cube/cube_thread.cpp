#include <iostream> 
#include <chrono>
#include <thread>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>

#include "gpio/gpio.h"

using namespace std;

#define RGB

extern bool CUBE_ON, FINISHED;
extern const int PANEL_WIDTH,CHAIN_LENGTH,PIXELS;
extern uint32_t binary_color[];

void delayMicrosecondsHard (unsigned long int howLong) {
	unsigned long int offset = 4;
	if (howLong >= offset) {
    		struct timespec sleep_time = { 0, (howLong-offset)*1000 };
        	nanosleep(&sleep_time, NULL);
	} else { 
		struct timeval tNow, tLong, tEnd ;
  		gettimeofday (&tNow, NULL) ;
  		tLong.tv_sec  = howLong / 1000000 ;
  		tLong.tv_usec = howLong % 1000000 ;
  		timeradd (&tNow, &tLong, &tEnd) ;
  		while (timercmp (&tNow, &tEnd, <))
    			gettimeofday (&tNow, NULL) ;
    		//struct timespec sleep_time = { 0, 1 };
        	//nanosleep(&sleep_time, NULL);
	}
}

 
void cube_thread() { 
  const unsigned ROW_A = 14, ROW_B = 15, ROW_C = 16, ROW_D = 17, ROW_E = 18;
  const unsigned TOP_R = 22;//TOP_G = 1<<23, TOP_B = 1<<24;
  //const unsigned BOT_R = 25;//BOT_G = 1<<26, BOT_B = 1<<27;
  const unsigned LAT =  19, CLK = 20, OE = 21; 
  vector<int> pins = {ROW_A,ROW_B,ROW_C,ROW_D,ROW_E,22,23,24,25,26,27,LAT,CLK,OE};
  set_output_pins(pins);
  uint32_t frame = 0, cube_off_count = 0;
  chrono::duration<double,std::milli> elapsed;
  auto start = chrono::high_resolution_clock::now();
  uint32_t *top_ptr = binary_color, *bot_ptr = (binary_color+32*PANEL_WIDTH*CHAIN_LENGTH);
  uint32_t delays[] = {0,1,2,4,8,16,32,64}; 
  while (!FINISHED) {
	if (!CUBE_ON && cube_off_count > 5) { 
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		continue; 
	} 
	else if (!CUBE_ON) { ++cube_off_count; }
	else if (CUBE_ON) { cube_off_count = 0;}

	for (uint32_t row = 0; row < 32; ++row) {
		uint32_t offset = row*PANEL_WIDTH*CHAIN_LENGTH;
		for (int modulation = 0; modulation < 8; ++modulation) {
			//GPIO_SET = 1<<OE | 1<<LAT;
			//GPIO_SET = 1<<OE; GPIO_CLR = 1<<LAT;
			GPIO_SET = 1<<LAT;
			for (int ii = 0; ii < PANEL_WIDTH*CHAIN_LENGTH; ++ii) {
				if (!CUBE_ON) {
					GPIO_CLR=0x3F<<TOP_R;
				} else {
					uint32_t a = *(top_ptr+offset+ii), b = *(bot_ptr+offset+ii);
					#ifdef RBG
					uint32_t flag_a = ((a >> modulation) & 0x1) | ((a >> (6+modulation) & 0x4)) | ((a >> (15+modulation) & 0x2)); 
					uint32_t flag_b = ((b >> modulation) & 0x1) | ((b >> (6+modulation) & 0x4)) | ((b >> (15+modulation) & 0x2)); 	
					#else
					uint32_t flag_a = ((a >> modulation) & 0x1) | ((a >> (7+modulation) & 0x2)) | ((a >> (14+modulation) & 0x4)); 
					uint32_t flag_b = ((b >> modulation) & 0x1) | ((b >> (7+modulation) & 0x2)) | ((b >> (14+modulation) & 0x4)); 
					#endif
					uint32_t flag = (flag_a | (flag_b<<3));
					GPIO_SET = flag << TOP_R;
					GPIO_CLR = ((~flag) & 0x3f)<<TOP_R;
				}
				GPIO_CLR = 1<<CLK;
				GPIO_SET = 1<<CLK;
			}
			GPIO_CLR = 1<<CLK;
			GPIO_SET = row<<ROW_A;
			GPIO_CLR = ((~row) & 0x1F)<<ROW_A;
			
			GPIO_CLR = 1<<LAT;
			
			GPIO_CLR = 1<<OE;
			delayMicrosecondsHard( delays[modulation] );
			GPIO_SET = 1<<OE;
    			//struct timespec sleep_time = { 0, 1 };
        		//nanosleep(&sleep_time, NULL);
		}
  	}
	++frame;
	//std::this_thread::sleep_for(std::chrono::milliseconds(0));
  }
  elapsed = chrono::duration<double,std::milli>(chrono::high_resolution_clock::now()-start);
  cout << elapsed.count()/frame << endl;
} 

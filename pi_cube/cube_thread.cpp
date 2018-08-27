////////////////////////////////////////////////////////////
////
//// LED Cube for Teaching Introductory Programming Concepts
//// Copyright (C) 2018 Ira Hill (ijh3@ufl.edu)
////
//// This program is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////
//// Namespace/Defines
//////////////////////////////////////////////////////////////
using namespace std;
#define RBG

////////////////////////////////////////////////////////////
//// Globals
//////////////////////////////////////////////////////////////
extern bool CUBE_ON, FINISHED;
extern const int PANEL_WIDTH,CHAIN_LENGTH,PIXELS;
extern uint32_t binary_color[];

////////////////////////////////////////////////////////////
//// Pi specific delay routine
//////////////////////////////////////////////////////////////
void delayMicrosecondsHard (unsigned long int howLong) {
	unsigned long int offset = 4;
	if (howLong >= offset) {
    		struct timespec sleep_time = { 0, static_cast<long int>((howLong-offset)*450) };
        	nanosleep(&sleep_time, NULL);
	} else { 
		struct timeval tNow, tLong, tEnd ;
  		gettimeofday (&tNow, NULL) ;
  		tLong.tv_sec  = howLong / 1000000 ;
  		tLong.tv_usec = howLong % 1000000 ;
  		timeradd (&tNow, &tLong, &tEnd) ;
  		while (timercmp (&tNow, &tEnd, <))
    			gettimeofday (&tNow, NULL) ;
	}
}

////////////////////////////////////////////////////////////
//// Dedicated Thread to update LED Cube
//////////////////////////////////////////////////////////////
void cube_thread() { 
  const unsigned ROW_A = 13, LAT = 18, CLK = 19, OE = 20; 
  vector<int> pins = {ROW_A,14,15,16,17,22,23,24,25,26,27,LAT,CLK,OE,5,6,7,8,9,10};
  set_output_pins(pins);
  uint32_t frame = 0, cube_off_count = 0, half_width = PANEL_WIDTH*CHAIN_LENGTH/2;
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
			for (uint32_t ii = 0; ii < half_width; ++ii) {
				if (!CUBE_ON) {
					GPIO_CLR=0x7E003F<<5;
				} else {
					//uint32_t top_j1 = 128<<8;
					//uint32_t bot_j1 = 128<<16;
					uint32_t top_j1 = *(top_ptr+ii+offset+half_width);
				        uint32_t bot_j1 = *(bot_ptr+ii+offset+half_width);
					uint32_t top_j2 = *(top_ptr+ii+offset), bot_j2 = *(bot_ptr+ii+offset);
					uint32_t flag = ((top_j2 >> modulation) & 0x1u) |
							((top_j2 >> (7+modulation)) & 0x2u) |
							((top_j2 >> (14+modulation)) & 0x4u) |
							((bot_j2 >> (4+modulation)) & 0x10u) |
							((bot_j2 >> (11+modulation)) & 0x20u) |
							((top_j1 << (17-modulation)) & (uint32_t)0x20000) |
							((top_j1 << (10-modulation)) & (uint32_t)0x40000) |
							((bot_j1 << (20-modulation)) & (uint32_t)0x100000) |
							((bot_j1 << (13-modulation)) & (uint32_t)0x200000);
					if (modulation <= 3) {
						flag |= ((top_j1 << (3-modulation)) & (uint32_t)0x80000) |
							((bot_j2 << (3-modulation)) & 0x8u);
					} else {
						flag |= ((top_j1 >> (modulation-3)) & (uint32_t)0x80000) |
							((bot_j2 >> (modulation-3)) & 0x8u);
					}
					if (modulation <= 6) {
						flag |= ((bot_j1 << (6-modulation)) & (uint32_t)0x400000);
					} else {
						flag |= ((bot_j1 >> (modulation-6)) & (uint32_t)0x400000);
					}
					GPIO_SET = flag << 5;
			  		GPIO_CLR = ((~flag) & (uint32_t)0x7E003F) << 5;
				}
				GPIO_CLR = 1<<CLK;
				GPIO_SET = 1<<CLK;
			}
			GPIO_CLR = 1<<CLK;
			GPIO_SET = row<<ROW_A;
			GPIO_CLR = ((~row) & (uint32_t)0x1F)<<ROW_A;
			
			GPIO_CLR = 1<<LAT;
			
			GPIO_CLR = 1<<OE;
			delayMicrosecondsHard( delays[modulation] );
			GPIO_SET = 1<<OE;
		}
	}
	++frame;
  }
  elapsed = chrono::duration<double,std::milli>(chrono::high_resolution_clock::now()-start);
  cout << elapsed.count()/frame << endl;
}

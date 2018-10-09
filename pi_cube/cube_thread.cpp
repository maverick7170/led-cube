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
#include <mutex>

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
extern mutex mtx;
volatile uint32_t *timer1Mhz;

////////////////////////////////////////////////////////////
//// Dedicated Thread to update LED Cube
//////////////////////////////////////////////////////////////
void cube_thread() {
  	int mem_fd;
  	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
	    	perror("can't open /dev/mem: ");
	      	return;
	}
	uint32_t *timereg =
		(uint32_t*) mmap(NULL,// Any adddress in our space will do
		(4*1024),   // Map length
		PROT_READ|PROT_WRITE,  // Enable r/w on GPIO registers.
		MAP_SHARED,
	        mem_fd,                // File to map
                0x3F000000  + 0x3000 // Offset to bcm register
               	);	
  	close(mem_fd);  
    	if (timereg == NULL) { return;}
      	timer1Mhz = timereg + 1;

      	const unsigned ROW_A = 13, LAT = 18, CLK = 19, OE = 20; 
	vector<int> pins = {ROW_A,14,15,16,17,22,23,24,25,26,27,LAT,CLK,OE,5,6,7,8,9,10};
	set_output_pins(pins);
	
	uint32_t frame = 0, half_width = PANEL_WIDTH*CHAIN_LENGTH/2;
	chrono::duration<double,std::milli> elapsed;
	auto start = chrono::high_resolution_clock::now();
	uint32_t *top_ptr = binary_color, *bot_ptr = (binary_color+32*PANEL_WIDTH*CHAIN_LENGTH);
	uint32_t delays[] = {1,2,4,8,16,32,64,128}; 
	
	while (!FINISHED) {
		for (uint32_t row = 0; row < 32; ++row) {
			uint32_t offset = row*PANEL_WIDTH*CHAIN_LENGTH;
			for (int modulation = 0; modulation < 8; ++modulation) {
				GPIO_SET = 1<<LAT;
				for (uint32_t ii = 0; ii < half_width; ++ii) {
					//GPIO_CLR=0x7E003F<<5;
					//uint32_t top_j1 = 128u<<8, bot_j1 = 128u<<16, top_j2 = 128u<<16, uint32_t bot_j2 = 128u;
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
					GPIO_CLR = 1<<CLK;
					GPIO_SET = 1<<CLK;
				}
				GPIO_CLR = 1<<CLK;
				GPIO_SET = row<<ROW_A;
				GPIO_CLR = ((~row) & (uint32_t)0x1F)<<ROW_A;	
				GPIO_CLR = 1<<LAT;
				GPIO_CLR = 1<<OE;
				uint32_t start = *timer1Mhz;
				while (*timer1Mhz - start <= delays[modulation]) {}
				GPIO_SET = 1<<OE;
			}
		}
		++frame;
		std::this_thread::sleep_for(std::chrono::nanoseconds(100));
	}
	elapsed = chrono::duration<double,std::milli>(chrono::high_resolution_clock::now()-start);
	cout << "CUBE THREAD: Elasped time (ms): " << elapsed.count()/frame << endl;
}

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

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include "gpio.h"
#include <cstdio>

void *gpio_map;
volatile unsigned *gpio; 

void set_output_pins(std::vector<int> pins) {
  for ( auto pin_name : pins) {
    INP_GPIO(pin_name); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(pin_name);
  }
}

void set_input_pins(std::vector<int> pins) {
  for ( auto pin_name : pins) {
    INP_GPIO(pin_name); // must use INP_GPIO before we can use OUT_GPIO
  }
}

// Set up a memory regions to access GPIO
bool setup_io()
{
   int  mem_fd;
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      //exit(-1);
      return false;
   }
 
   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );
 
   close(mem_fd); //No need to keep mem_fd open after mmap
 
   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      //exit(-1);
      return false;
   }
 
   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map; 
   return true;
} // setup_io

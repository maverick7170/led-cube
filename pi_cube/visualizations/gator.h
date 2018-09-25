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
#ifndef GATOR_
#define GATOR_

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include "base_visualizer.h"

class GATOR : public VISUALIZER {
public:
	GATOR(BNO080 &imu_) : VISUALIZER(imu_) {
  		for (int ii = 1; ii <= 6; ++ii) {
			std::string filename = "/usr/local/led_samples/gator_" + std::to_string(ii) + ".txt";
			read_text_file(filename,pixels[ii-1].data(),PANEL_WIDTH,CHAIN_LENGTH);
		}
	} 
	void start(uint32_t *led_data) {
		std::fill(led_data,led_data+24576,0);
	}
	void run(uint32_t *led_data) {
		++run_index;
		if (run_index % 5 != 0) { return; }
		double_buffer.fill(background_color);
		for (int row = 0; row < 64; ++row) {
			for (int col = 0; col < 64; ++col) {
				int a = row*PANEL_WIDTH*CHAIN_LENGTH + col + 64;
				int offset = 0;
				if (col+ticker%256 > 255) {offset = -256;}
				double_buffer[a+ticker%256+offset] = pixels[ticker%6][a];
				offset = 0;
				if (col+ticker%256+128 > 255) {offset = -256;}
				double_buffer[a+ticker%256+offset+128] = pixels[ticker%6][a];
			}
		}
		memcpy(led_data,double_buffer.data(),24576*4);
		++ticker;
	}
private:
	std::array<std::array<uint32_t,24576>,10> pixels;
	uint32_t background_color = 250u | 250u << 8 | 250u << 16;
	const int PANEL_WIDTH = 64, CHAIN_LENGTH = 6;
	size_t ticker = 0;
};



#endif

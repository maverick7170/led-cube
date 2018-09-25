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
#ifndef LOGO_
#define LOGO_

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include "base_visualizer.h"

class LOGO : public VISUALIZER {
public:
	LOGO(BNO080 &imu_, std::string filename) : VISUALIZER(imu_) {
		std::fill(pixels.begin(),pixels.end(),background_color);
  		read_text_file(filename,pixels.data(),64,6);
	} 
	void start(uint32_t *led_data) {
		memcpy(led_data,pixels.data(),24576*4);
	}
private:
	std::array<uint32_t,24576> pixels;
	uint32_t background_color = 128 | 128ul << 8 | 128ul << 16;
};
#endif

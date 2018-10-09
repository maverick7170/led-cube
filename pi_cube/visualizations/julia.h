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
#ifndef JULIA_
#define JULIA_

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include "base_visualizer.h"
#include "../fractals/juliaset.h"
#include <complex>

class JULIA : public VISUALIZER {
public:
	JULIA(BNO080 &imu_) : VISUALIZER(imu_), juliaset(128,64) {
		//juliaset.set_params(0,0,3.75,3.75/2,0.7885*std::exp(a*1i));
		//juliaset.set_params(0,0,3.3,3.3/2,-0.7269+0.1889i);
		juliaset.set_params(0,0,3.3,3.3/2,-0.8+0.156i);
		//juliaset.set_params(0,0,3.3,3.3,-0.4+0.6i);
	} 
	void start(uint32_t *led_data) {
		std::fill(led_data,led_data+24576,0);
	}
	void run(uint32_t *led_data) {
		if (run_index % 50 == 0) {
			double_buffer.fill(background_color);
	       		//a += 0.01;
			//juliaset.c = 0.7885*exp(a*1i);
			
			juliaset.m_width /= 1.025;
			juliaset.m_height /= 1.025;

			juliaset.update(double_buffer.data());
			memcpy(led_data,double_buffer.data(),24576*4);
		}
		++run_index;
		++ticker;
	}
private:
	JuliaSet juliaset;
	double a = 0;
	std::array<std::array<uint32_t,24576>,10> pixels;
	uint32_t background_color = 0;
	const int PANEL_WIDTH = 64, CHAIN_LENGTH = 6;
	size_t ticker = 0;
};



#endif

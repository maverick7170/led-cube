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
	JULIA(BNO080 &imu_, int mode_) : VISUALIZER(imu_), juliaset(128,64), mode(mode_) {
	} 
	void start(uint32_t *led_data) {
		std::fill(led_data,led_data+24576,0);
		switch (mode) {
			case 0: 
				juliaset.set_window(128,64);
				juliaset.set_params(0,0,3.75,3.75/2,0.7885*std::exp(a_param*1i));
				break;
			case 1:
				juliaset.set_window(128,64);
				juliaset.set_params(0,0,3.3,3.3/2,-0.7269+0.1889i);
				break;
			case 2:
				juliaset.set_window(128,64);
				juliaset.set_params(0,0,3.3,3.3/2,-0.8+0.156i);
				break;
			case 3:
				juliaset.set_window(64,64);
				juliaset.set_params(0,0,3.3,3.3,-0.4+0.6i);
				break;
		}
	}
	void run(uint32_t *led_data) {
		if (run_index % 20 == 0) {
			double_buffer.fill(background_color);
			switch (mode) {
	       			case 0:
					a_param += 0.015;
					juliaset.c = 0.7885*exp(a_param*1i);
					break;
				case 1: case 2: case 3:
					juliaset.m_width /= zoom_factor;
					juliaset.m_height /= zoom_factor;
					if (juliaset.m_width <= 0.8) {
						zoom_factor = 1./zoom_factor;
					} else if (juliaset.m_width >= 30) {
						zoom_factor = 1./zoom_factor;
					}	
					break;
			}
			juliaset.update(double_buffer.data());
			memcpy(led_data,double_buffer.data(),24576*4);
		}
		++run_index;
		++ticker;
	}
private:
	JuliaSet juliaset;
	int mode;
	double a_param = 0, zoom_factor = 1.025;
	std::array<std::array<uint32_t,24576>,10> pixels;
	uint32_t background_color = 137u<<16;
	const int PANEL_WIDTH = 64, CHAIN_LENGTH = 6;
	size_t ticker = 0;
};



#endif

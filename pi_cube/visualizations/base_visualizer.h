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
#ifndef BASE_VISUALIZER_
#define BASE_VISUALIZER_

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include "../imu/bno080.h"
#include <algorithm>

class VISUALIZER {
public:
	VISUALIZER(BNO080 &imu_) : imu(imu_) {
		std::fill(double_buffer.begin(),double_buffer.end(),0);
	}
	virtual void start(uint32_t *led_data) {
		memcpy(led_data,double_buffer.data(),64*64*6*4);	
	}
	virtual void stop(uint32_t *led_data) {
		std::fill(double_buffer.begin(),double_buffer.end(),0);
		memcpy(led_data,double_buffer.data(),64*64*6*4);		
	}
	virtual void run(uint32_t *led_data) {
	}
	BNO080 &imu;
	static std::array<uint32_t,64*64*6> double_buffer;
};

std::array<uint32_t,64*64*6> VISUALIZER::double_buffer;

#endif

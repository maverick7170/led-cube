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
#ifndef DICE_
#define DICE_

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include "base_visualizer.h"


class DICE : public VISUALIZER {
public:
	DICE(BNO080 &imu_, uint32_t *pixel_data_) : VISUALIZER(imu_) {
		pixel_data = pixel_data_;
		memcpy(double_buffer.data(),pixel_data,64*64*6*4);
	} 
	void start(uint32_t *led_data) {
		imu.reset();
 		imu.request_report(BNO080::LINEAR_ACCELERATION,75);
		memcpy(led_data,pixel_data,64*64*6*4);
	}
	void stop(uint32_t *led_data) {
		imu.reset();
	}
	void run(uint32_t *led_data) {
		
	}
private:
	uint32_t *pixel_data;
};



#endif

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
#ifndef SPARKY_
#define SPARKY_

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include <mutex>
#include "base_visualizer.h"
#include "../liquid_fun/lf_sparky.h"

extern std::mutex mtx;

class SPARKY : public VISUALIZER {
public:
	SPARKY(BNO080 &imu_) : VISUALIZER(imu_) {}
	~SPARKY() {}	
	void start(uint32_t *led_data) {
		if (animate == nullptr) {
			animate = new LFSparky(384,64,4,double_buffer.data());
			animate->CreateWalls();
		}
	}
	void stop(uint32_t *led_data) {
		if (animate) { delete animate; }
		animate = nullptr;
	}
	void run(uint32_t *led_data) {
		//++run_index;
		//if (run_index % 6 != 0) { return; }
		if (animate == nullptr) {return;}
		double_buffer.fill(0u);
		animate->Step();
    		animate->ProcessContacts();
    		animate->Draw();
		mtx.lock();
		memcpy(led_data,double_buffer.data(),24576*4);
		mtx.unlock();	
	}
private:
	LiquidFunAnimation *animate = nullptr; 
};
#endif

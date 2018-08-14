////////////////////////////////////////////////////////////
//
// LED Cube for Teaching Introductory Programming Concepts
// Copyright (C) 2018 Ira Hill (ijh3@ufl.edu)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <iostream>
#include <random>
#include <chrono>
#include <ctime>
#include <cmath>
#include "led_cube.h"

int main(int argc, char *argv[])
{
	//Check if we should update simulation or a real cube
	int cube_type = 0;
	if (argc > 1) {
		cube_type = atoi(argv[1]);
	}

	//Create cube and vector to hold all 12 images
	LED_Cube cube;
	std::vector<uint8_t> red(64,255), green(64,255), blue(64,0);
	for (auto ii = 0; ii < 32; ++ii) {
		red[ii] = round((ii+1)*7.968);
		green[ii+32] = round((32-ii)*7.968);
	}

	//Equalize bar properties
	double mag = 40, step = 0.2244;
	int bar_width = 5;

	//Random numbers for bar baseline and shift up/down
	std::mt19937 engine(time(0));
	std::uniform_int_distribution<int> dist_baseline(0,30);
	std::uniform_real_distribution<double> dist_shift(-10,10);

	//Loop for 60 seconds, showing mario walking
	auto start = std::chrono::system_clock::now();

	while (true) {
		//Check elapsed time
		std::chrono::duration<double> elapsed = std::chrono::system_clock::now()-start;
		if (elapsed.count() >= 30) { break; }

		//Clear all colors
		cube.reset();

		//Using sin waves, create random equalizer bars on each face
		for (auto face = 0; face < 6; ++face) {
			auto baseline = dist_baseline(engine);
			std::vector<double> bars;
			for (int ii = 1; ii < 64/bar_width+2; ++ii) {
				bars.push_back(mag*sin(ii*step)+baseline+dist_shift(engine));
			}
			for (int row = 63; row >= 0; row--) {
				for (int col = 0; col < 64; ++col) {
					auto bar_index = col/bar_width, index = row*64*6 + face*64 + col;
					//For each bar, draw a black bar between them for clarity
					if ( col % bar_width == bar_width - 1) {
						cube.set_led(index,cube.black);
					} else {
						auto height = 63-row;
						if (height <= bars[bar_index]) {
							cube.set_led(index,Pixel(red[height],green[height],blue[height]));
						}
					}
				}
			}
		}

		//Update cube simulation with image and wait before showing next one
		cube.update(cube_type);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));		
	}

	return 0;
}
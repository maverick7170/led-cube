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
#include "led_cube.h"
#include <png.hpp>

int main(int argc, char *argv[])
{
	//Check if we should update simulation or a real cube
	int cube_type = 0;
	if (argc > 1) {
		cube_type = atoi(argv[1]);
	}

	//Create cube and vector to hold all 12 images
	LED_Cube cube;
	std::vector<png::image<png::rgb_pixel>> all_pixels;
	const int image_count = 12;

	//Load all 12 images into vector
	for (auto ii = 1; ii <= image_count; ++ii) {
		std::string filename = "../../resources/mario/mario_" + std::to_string(ii) + ".png";
		png::image<png::rgb_pixel> pix(filename);
		all_pixels.push_back( pix );
	}

	//Loop for 60 seconds, showing mario walking
	size_t ticker = 0;
	auto start = std::chrono::system_clock::now();

	while (true) {
		//Check elapsed time
		std::chrono::duration<double> elapsed = std::chrono::system_clock::now()-start;
		if (elapsed.count() >= 30) { break; }

		//Set background color
		cube.set_all_leds(cube.white);

		//Draw image
		for (auto row = 0; row < 64; ++row) {
			for (auto col = 0; col < 64; ++col) {
				int index = row*64*6 + col + 64, offset = 0;
				if (col + ticker%256 > 255) {
					offset -= 256;
				}
				//Need to convert between png library rgb_pixel and our simple Pixel
				png::rgb_pixel pix = all_pixels[ticker%image_count][row][col];
				cube.set_led(index+ticker%256+offset,Pixel(pix.red,pix.green,pix.blue));
			}
		}

		//Update cube simulation with image and wait before showing next one
		cube.update(cube_type);
		++ticker;
		std::this_thread::sleep_for(std::chrono::milliseconds(25));		
	}

	return 0;
}
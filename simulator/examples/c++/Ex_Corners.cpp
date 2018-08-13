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

int main()
{
	//Create cube and vector of default colors
	LED_Cube cube;
	std::vector<Pixel> colors = {cube.white, cube.red, cube.green, cube.blue, 
								 cube.yellow, cube.magenta, cube.aqua, cube.salmon,
								 cube.silver, cube.lawn_green, cube.purple, cube.navy };

	//Create a vector of the 4 corners on every face
	std::vector<int> indices = {0,383,24192,24575};
	for (auto ii = 64; ii < 384; ii+=64) {
		indices.push_back(ii-1);
		indices.push_back(ii);
		indices.push_back(ii+24191);
		indices.push_back(ii+24192);
	}

	//Set each corner to a random color
	std::mt19937 engine(time(0));
	std::uniform_int_distribution<int> dist(0,colors.size()-1);
	for (auto index : indices) {
		cube.set_led(index,colors[dist(engine)]);
	}

	//Update cube simulation with new colors
	cube.update();

	return 0;
}
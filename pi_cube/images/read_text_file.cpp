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

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include <iostream> 
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

void read_text_file(std::string filename, uint32_t *data, int PANEL_WIDTH, int CHAIN_LENGTH) {
	uint32_t count=0, width = 0;
	std::vector<uint32_t> panels;
	std::ifstream fid(filename);
	std::string line;
  	getline(fid,line);
  	if (line.substr(0,6) == "Width:") {
		width = stoul(line.substr(7));	
  	}	  
  	getline(fid,line);
  	//if (line.substr(0,7) == "Height:") {
		//height = stoul(line.substr(8));
  	//}
  	getline(fid,line);
  	if (line.substr(0,7) == "Panels:") {
		std::stringstream ss(line.substr(8));
		uint32_t p;
		while (ss >> p) { panels.push_back(p); }
  	}
  	//std::cout << "Processing image with width " << width << " and height " << height << " and panels " << panels[0] << std::endl;
	while (width && !fid.eof()) {
		std::string line;
		fid >> line;
		if (line.size() < 1) { break; }
		uint32_t val = std::stoul(line);
		for (auto p : panels) {
			uint32_t loc = (count/width)*PANEL_WIDTH*CHAIN_LENGTH+(count%width)+p*64;
			data[loc] = val;
		}
		++count;
  	}
}

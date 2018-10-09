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
#ifndef JULIASET_
#define JULIASET_

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>
#include <complex>
#include <algorithm>

using namespace std::complex_literals;

uint32_t jet_r[50] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 39, 59, 78, 98, 118, 137, 157, 177, 196, 216, 235, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 235, 216, 196, 177};
uint32_t jet_g[50] = {0, 0, 0, 0, 0, 0, 0, 20, 39, 59, 78, 98, 118, 137, 157, 177, 196, 216, 235, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 235, 216, 196, 177, 157, 137, 118, 98, 78, 59, 39, 20, 0, 0, 0, 0, 0};
uint32_t jet_b[50] = {137, 157, 177, 196, 216, 235, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 235, 216, 196, 177, 157, 137, 118, 98, 78, 59, 39, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

class JuliaSet {
public:
	JuliaSet(int WINDOW_WIDTH_, int WINDOW_HEIGHT_) : WINDOW_WIDTH(WINDOW_WIDTH_), WINDOW_HEIGHT(WINDOW_HEIGHT_), sequence(WINDOW_HEIGHT,std::vector<std::complex<double>>(WINDOW_WIDTH,0)) {
	
	}
	void set_params(double cx, double cy, double width, double height, std::complex<double> c_) {
		m_cx = cx; 
		m_cy = cy; 
		m_width = width; 
		m_height = height; 
		c = c_;
	}
	void set_iterations(int iterations_) {
		iterations = iterations_;
		color_mod = iterations/50;
	}
	void update(uint32_t pixels[]) {
		for (auto &s : sequence) { std::fill(s.begin(),s.end(),0); }        
		double step_size = m_width/WINDOW_WIDTH;   
		auto im = m_cy-m_height/2;
		for (int ii = 0; ii < WINDOW_HEIGHT; ++ii) {
			auto index = ii*384+64;
			auto re = m_cx-m_width/2;
            		for (int jj = 0; jj < WINDOW_WIDTH; ++jj) {
                		for (int kk = 0; kk < iterations; ++kk) {
                    			if (kk == 0) { sequence[ii][jj] = re + im*1i; }
                    			if (!pixels[index]) {
                        			sequence[ii][jj] = sequence[ii][jj]*sequence[ii][jj] + c;
                        			//if ( abs(sequence[ii][jj].real()) >= 2 || abs(sequence[ii][jj].imag()) >= 2) {
                        			if (abs(sequence[ii][jj]) >= 2) {
                            				int offset = kk/color_mod;
                            				pixels[index] = jet_r[offset] | (jet_g[offset] << 8) | (jet_b[offset] << 16);
                            				pixels[index+128] = jet_r[offset] | (jet_g[offset] << 8) | (jet_b[offset] << 16);
                            				break;
                        			}
                    			}
                		}
                		re += step_size;
				++index;
            		}
            		im += step_size;
        	}
    	}    
public:
	const int WINDOW_WIDTH, WINDOW_HEIGHT;
	double m_cx, m_cy, m_width, m_height;
	std::complex<double> c;
	int iterations = 50, color_mod = iterations/50;
private:
	std::vector< std::vector<std::complex<double>> > sequence;
};
#endif

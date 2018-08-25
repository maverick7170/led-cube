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
#include <chrono>
#include <thread>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>

#include "liquid_fun/particle_sim.h"
#include "imu/bno080.h"
#include "gpio/gpio.h"
#include "udp/UDPServer.h"

using namespace std;

extern bool CUBE_ON, FINISHED;
bool CUBE_ON = true, FINISHED = false;
extern const int PANEL_WIDTH, CHAIN_LENGTH, PIXELS;
const int PANEL_WIDTH = 64, CHAIN_LENGTH = 6, PIXELS = PANEL_WIDTH*PANEL_WIDTH*CHAIN_LENGTH;
extern uint32_t binary_color[];
uint32_t binary_color[PIXELS], double_buffer[PIXELS];
void cube_thread(); 
uint32_t background_color = 128 | 128ul << 8 | 128ul << 16;

void read_txt_file(string filename) {
  uint32_t count=0, width = 0;
  vector<uint32_t> panels;

  for (int ii = 0; ii < PIXELS; ++ii) { binary_color[ii] = background_color; }
 
  ifstream fid(filename);
  string line;
  getline(fid,line);
  if (line.substr(0,6) == "Width:") {
	width = stoul(line.substr(7));	
  }	  
  getline(fid,line);
  if (line.substr(0,7) == "Height:") {
	//height = stoul(line.substr(8));
  }
  getline(fid,line);
  if (line.substr(0,7) == "Panels:") {
	stringstream ss(line.substr(8));
	uint32_t p;
	while (ss >> p) { panels.push_back(p); }
  }
  //std::cout << "Processing image with width " << width << " and height " << height << " and panels " << panels[0] << std::endl;
  while (width && !fid.eof()) {
	string line;
	//getline(fid,line);
	fid >> line;
	if (line.size() < 1) { break; }
	uint32_t val = stoul(line);
	for (auto p : panels) {
		uint32_t loc = (count/width)*PANEL_WIDTH*CHAIN_LENGTH+(count%width)+p*64;
		binary_color[loc] = val;
	}
	++count;
  }
}


int main(int argc, char **argv)
{
  
  ///////////////////////////////////////////
  std::cout << "MAIN THREAD: Loading images..." << std::endl;
  array<uint32_t,PIXELS> logos, numbers, obstacles, blank, deathstar;
  read_txt_file("/home/pi/Code/led_panel/images/Panel_Numbers.txt");
  memcpy(numbers.data(),binary_color,PIXELS*4);
  read_txt_file("/home/pi/Code/led_panel/images/ASEE_Combo_Bravo.txt");
  memcpy(logos.data(),binary_color,PIXELS*4);
  read_txt_file("/home/pi/Code/led_panel/images/Obstacles.txt");
  memcpy(obstacles.data(),binary_color,PIXELS*4);
  read_txt_file("/home/pi/Code/led_panel/images/DeathStar.txt");
  memcpy(deathstar.data(),binary_color,PIXELS*4);
 
  vector<array<uint32_t,PIXELS>> animation(10,array<uint32_t,PIXELS>());	
  for (int ii = 1; ii <= 6; ++ii) {
	string filename = "/home/pi/Code/led_panel/images/gator_" + to_string(ii) + ".txt";
	read_txt_file(filename);
	memcpy(animation[ii-1].data(),binary_color,PIXELS*4);
  }

  for (int ii = 0; ii < PIXELS; ++ii) { binary_color[ii] = background_color; blank[ii] = 0; double_buffer[ii] = 0; }
  std::cout << "MAIN THREAD: Finished loading images..." << std::endl;
  ///////////////////////////////////////////
 
  uint32_t color_test[10] = { 255, 255<<8, 255<<16, 255 | 255<<8, 255 | 160<<8, 128<<16, 255<<8 | 255<<16, 128 | 128<<16, 255 | 192<<8 | 192<<16, 255 | 255<<8 | 255<<16 };  


  ///////////////////////////////////////////
  std::cout << "MAIN THREAD: Setting output pins" << std::endl;
  setup_io();
  set_output_pins(std::vector<int>{5});
  GPIO_SET = 1<<5;
  std::cout << "MAIN THREAD: Setting output pins finished..." << std::endl;
  ///////////////////////////////////////////

  ///////////////////////////////////////////
  std::cout << "MAIN THREAD: Setting imu" << std::endl;
  BNO080 imu(0x4B);
  Eigen::Vector3d rotx,roty,rotz;
  imu.reset();
  std::cout << "MAIN THREAD: Setting imu finished..." << std::endl;
  ///////////////////////////////////////////

  ///////////////////////////////////////////
  std::cout << "MAIN THREAD: Starting udp server" << std::endl;
  UDPServer server(8080);
  ///////////////////////////////////////////
  
  ///////////////////////////////////////////
  ParticleSim *psim = nullptr;
  ///////////////////////////////////////////

  uint32_t mode = 0, ticker = 0;
 
  off_t control_shm_length = 4; //pixels_shm_length = PIXELS*4;
  int control_shm_fd = shm_open("/control_shm",O_RDWR | O_CREAT,0666);
  //int pixels_shm_fd = shm_open("/pixels_shm",O_RDWR | O_CREAT,0666);
  ftruncate(control_shm_fd, control_shm_length);
  //ftruncate(pixels_shm_fd, pixels_shm_length);

  assert(control_shm_fd > 0);
  //assert(pixels_shm_fd > 0);

  uint32_t *control_shm = (uint32_t *)mmap(NULL, control_shm_length, PROT_READ|PROT_WRITE, MAP_SHARED, control_shm_fd, 0);
  *control_shm = 1;
  //uint32_t *pixels_shm = (uint32_t *)mmap(NULL, pixels_shm_length, PROT_READ|PROT_WRITE, MAP_SHARED, pixels_shm_fd, 0);
 
  memcpy(binary_color,blank.data(),PIXELS*4);
  
  thread t1(cube_thread);
  
  //int policy = SCHED_FIFO;
  //sched_param sch_params;
  //sch_params.sched_priority = 90;
  //if (pthread_setschedparam(t1.native_handle(), policy, &sch_params) ) {
  //	std::cout << "MAIN LOOP: Thread priority failed..." << std::endl;
  //}


  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(3, &cpuset);
  if ( pthread_setaffinity_np(t1.native_handle(),sizeof(cpu_set_t), &cpuset) != 0 ) {
	std::cout << "MAIN LOOP: Thread affinity failed..." << std::endl;
  }

  uint32_t main_loop_iter = 0, last_iter = 0;
  uint32_t next_mode;
  while (1) {
	if (main_loop_iter++ % 100 == 0) {
		next_mode = *control_shm;
	} else { next_mode = mode; }

	if (next_mode >= 2608) { next_mode -= 2608; }
	if (next_mode >= 660949) { 
		std::cout << "MAIN THREAD: Exiting with mode: " << next_mode << std::endl;
		CUBE_ON = false; break; 
	}
	if (next_mode != mode) {
		std::cout << "MAIN THREAD: Mode switch from " << mode << " to " << next_mode << std::endl;
		switch (mode) {
			case 0: case 2608:
  				GPIO_CLR = 1<<5;
				this_thread::sleep_for(chrono::milliseconds(100)); 
				CUBE_ON = true;
				break;
			case 1: case 2609: case 4: case 2612:
 				imu.request_report(BNO080::LINEAR_ACCELERATION,0);
				//this_thread::sleep_for(chrono::milliseconds(100)); 
				break;
			case 6: case 2614:
 				imu.request_report(BNO080::ACCELEROMETER,0);
				//this_thread::sleep_for(chrono::milliseconds(100)); 
				break;
			case 7: case 2615:
 				imu.request_report(BNO080::ACCELEROMETER,0);
				//this_thread::sleep_for(chrono::milliseconds(100)); 
				break;
			case 8: case 2616:
				imu.request_report(BNO080::MAGNETIC_FIELD,0);
				imu.request_report(BNO080::GAME_ROTATION_VECTOR,0);
				//this_thread::sleep_for(chrono::milliseconds(100)); 
				break;
		}
		mode = next_mode;
		switch (mode) {
			case 0:
				if (CUBE_ON) {
					CUBE_ON = false;
					this_thread::sleep_for(chrono::milliseconds(25)); 
  					GPIO_SET = 1<<5;
				} else {
  					GPIO_CLR = 1<<5;
					this_thread::sleep_for(chrono::milliseconds(25)); 
					CUBE_ON = true;
				}
				break;
			case 1: 
				//Numbers
 				imu.request_report(BNO080::LINEAR_ACCELERATION,75);
				CUBE_ON = true;
				memcpy(binary_color,numbers.data(),PIXELS*4);
				break;
			case 2:
				//Logos
				CUBE_ON = true;
				memcpy(binary_color,logos.data(),PIXELS*4);
				break;
			case 3:
				//Gator
				CUBE_ON = true;
				ticker = 0;
				memcpy(binary_color,animation[0].data(),PIXELS*4);
				break;
			case 4:
				//Sparky
 				imu.request_report(BNO080::LINEAR_ACCELERATION,75);
				CUBE_ON = true;
				ticker = 0;
				if (psim != nullptr) {delete psim;}
				psim = new ParticleSim(b2Vec2(0,0));
				psim->CreateWalls();
				psim->Sparky();
				memcpy(binary_color,blank.data(),PIXELS*4);
				break;
			case 5:
				//Explore Colors
				CUBE_ON = true;
				ticker = 0;
				memcpy(binary_color,blank.data(),PIXELS*4);
				break;
			case 6:
				//IMU Test
				//imu.reset();
 				imu.request_report(BNO080::ACCELEROMETER,75);
				CUBE_ON = true;
				memcpy(binary_color,blank.data(),PIXELS*4);
				break;
			case 7:
				//Water
				/*CUBE_ON = true;
				//imu.reset();
 				imu.request_report(BNO080::ACCELEROMETER,75);
				if (psim != nullptr) {delete psim;}
				psim = new ParticleSim(b2Vec2(0,-9.81));
				psim->CreateWalls();
				psim->Water();
				*/
				memcpy(binary_color,blank.data(),PIXELS*4);
				break;
			case 8:
				//Calibrate
				CUBE_ON = true;
				//imu.reset();
 				imu.request_report(BNO080::GAME_ROTATION_VECTOR,75);
 				imu.request_report(BNO080::MAGNETIC_FIELD,75);
				break;
			case 9:
				CUBE_ON = true;
				memcpy(binary_color,blank.data(),PIXELS*4);
				break;
		}
	}
	if (mode == 1) {
		if ( imu.get_reports() && imu.good_reads >= 3 ) {
			if (main_loop_iter-last_iter > 40 && (fabs(imu.linear.x) > 1.5 || fabs(imu.linear.y) > 1.5 || fabs(imu.linear.z) > 1.5)) {
				vector<int> order = {0,1,2,3,4,5};
				random_shuffle(order.begin(),order.end());
				//memcpy(binary_color,blank.data(),PIXELS*4);
				std::cout << "MAIN LOOP: Shuffling cube: " << std::endl;
				for (int row = 0; row < 64; ++row) {
					for (int col = 0; col < 64; ++col) {
						for (int page = 0; page < 6; ++page) {
							int a = row*PANEL_WIDTH*CHAIN_LENGTH + page*64+col;
							int b = row*PANEL_WIDTH*CHAIN_LENGTH + order[page]*64+col;
							double_buffer[a] = numbers[b];
						}
					}
				}
				memcpy(binary_color,double_buffer,PIXELS*4);
				last_iter = main_loop_iter;
			}
		}
	} else if (mode == 3 && main_loop_iter % 5 == 0) {
		//Gator
  		for (int ii = 0; ii < PIXELS; ++ii) { double_buffer[ii] = background_color; }
		for (int row = 0; row < 64; ++row) {
			for (int col = 0; col < 64; ++col) {
				int a = row*PANEL_WIDTH*CHAIN_LENGTH + col + 64;
				int offset = 0;
				if (col+ticker%256 > 255) {offset = -256;}
				double_buffer[a+ticker%256+offset] = animation[ticker%6][a];
				offset = 0;
				if (col+ticker%256+128 > 255) {offset = -256;}
				double_buffer[a+ticker%256+offset+128] = animation[ticker%6][a];
			}
		}
		memcpy(binary_color,double_buffer,PIXELS*4);
		//this_thread::sleep_for(chrono::milliseconds(50)); 
		++ticker;
	} else if (mode == 4 && main_loop_iter % 3 == 0) {	
		//sparky
		std::array<double,3> impulse = {0,0,0};
		if ( imu.get_reports() && imu.good_reads >= 5 && main_loop_iter-last_iter > 20 ) {
			if (fabs(imu.linear.x) > 1.5 || fabs(imu.linear.y) > 1.5 || fabs(imu.linear.z) > 1.5) {
				//std::cout << std::fixed << std::setprecision(4) << "MAIN THREAD Impulse: " << imu.linear.x << "," << imu.linear.y << "," << imu.linear.z << " and reads " << imu.good_reads << " with " << main_loop_iter-last_iter << std::endl;
				impulse[0] = imu.linear.x/2.0;
				impulse[1] = imu.linear.y/2.0;
				impulse[2] = imu.linear.z/2.0;
				last_iter = main_loop_iter;
			}
		}
		psim->impulse = impulse;
		psim->step(double_buffer,blank.data());
		memcpy(binary_color,double_buffer,PIXELS*4);
		//this_thread::sleep_for(chrono::milliseconds(25)); 
	} else if (mode == 5) {
		//Color choices
		memcpy(double_buffer,blank.data(),PIXELS*4);
		for (int ii = 0; ii < PIXELS; ++ii) {
			double_buffer[ii] = color_test[ticker/100 % 10];
		}
		memcpy(binary_color,double_buffer,PIXELS*4);
		++ticker;
	} else if (mode == 6) {
		//Orientation
		if ( imu.get_reports() && imu.good_reads >= 3 ) {
			//std::cout << "MAIN THREAD Orientation: " << imu.accel.x << "," << imu.accel.y << "," << imu.accel.z << std::endl;
			double grav[3] = {imu.accel.x,imu.accel.y,imu.accel.z};
			int up_panel = -1;
			if ( sqrt(grav[0]*grav[0]+grav[1]*grav[1]+grav[2]*grav[2]) >= 11) {

			} else if ( abs(grav[0]) >= 7.  && abs(grav[0]) >= abs(grav[1]) && abs(grav[0]) >= abs(grav[2])) {
				if (grav[0] > 0) {
					up_panel = 4;
				} else {
					up_panel = 2;
				}
			} else if ( abs(grav[1]) >= 7. && abs(grav[1]) >= abs(grav[2]) ) {
				if (grav[1] > 0) {
					up_panel = 5;
				} else {
					up_panel = 0;
				}
			} else if ( abs(grav[2]) >= 7.) {
				if (grav[2] > 0) {
					up_panel = 3;
				} else {
					up_panel = 1;
				}
			}
			if (up_panel >= 0) {
				memcpy(double_buffer,blank.data(),PIXELS*4);
				for (int row = 0; row < 64; ++row) {
					for (int col = 0; col < 64; ++col) {
						if ( sqrt((row-31)*(row-31) + (col-31)*(col-31)) <= 25) {
							int a = row*PANEL_WIDTH*CHAIN_LENGTH + col + up_panel*64;
							double_buffer[a] = 255;
						}
					}
				}
				memcpy(binary_color,double_buffer,PIXELS*4);
			}
		}  
	} else if ( mode == 7 && main_loop_iter % 3 == 0 ) {
		//Water
		/*std::array<double,3> gravity = {0,0,0};
		if ( imu.get_reports() && imu.good_reads >= 5 ) {
			//std::cout << "MAIN THREAD: Water: " << imu.accel.x << "," << imu.accel.y << "," << imu.accel.z << std::endl;
			gravity[0] = imu.accel.x;
			gravity[1] = imu.accel.y;
			gravity[2] = imu.accel.z;
		}
		psim->gravity = gravity;
		psim->step(binary_color,obstacles.data());
		*/
	} else if ( mode == 8) {
		//Calibrate
		if ( imu.get_reports() ) {
			imu.game.get_xyz(rotx,roty,rotz);
			std::cout << "MAIN THREAD: GAME ROT STATUS: " << rotx[0] << "," << rotx[1] << "," << rotx[2] << " with " << imu.game.status << std::endl;
			std::cout << "MAIN THREAD: MAG STATUS: " << imu.mag.x << "," << imu.mag.y << "," << imu.mag.z << " with " << imu.mag.status << std::endl;
			uint32_t status_color[2] = {255,255}, radius[2] = {15,15};
			switch (imu.game.status) {
				case 1: status_color[0] = 255 | 255<<16; break;
				case 2: status_color[0] = 255<<8; break;
				case 3: status_color[0] = 255<<8; radius[0] = 30; break;
			}
			switch (imu.mag.status) {
				case 1: status_color[1] = 255 | 255<<16; break;
				case 2: status_color[1] = 255<<8; break;
				case 3: status_color[1] = 255<<8; radius[1] = 30; break;
			}
			memcpy(binary_color,blank.data(),PIXELS*4);
			for (int row = 0; row < 64; ++row) {
				for (int col = 0; col < 64; ++col) {
					if ( sqrt((row-31)*(row-31) + (col-31)*(col-31)) <= radius[0]) {
						int a = row*PANEL_WIDTH*CHAIN_LENGTH + col + 5*64;
						binary_color[a] = status_color[0];
					}
					if ( sqrt((row-31)*(row-31) + (col-31)*(col-31)) <= radius[1]) {
						int a = row*PANEL_WIDTH*CHAIN_LENGTH + col + 1*64;
						binary_color[a] = status_color[1];
					}
				}
			}
		}
	} else if ( mode == 9) {
		int total_bytes = 0;
		while (true) {
			std::vector<uint8_t> msg;
			std::string sender;
			int port, flag = 0;
			int bytes_read = server.receive_(msg,sender,port);
			if (bytes_read > 6 && msg[0] == 128) {
				int row = msg[3];
				uint16_t length = msg[1] | (uint16_t)msg[2] << 8;
				uint16_t offset = msg[4] | (uint16_t)msg[5] << 8;
				flag = msg[6];
				if (length == bytes_read) {
					total_bytes += bytes_read;
					for (int ii = 7, cnt = 0; ii < length; ii+=3,++cnt) {
						double_buffer[ row*64*6 + cnt + offset] = (uint32_t)msg[ii] | (uint32_t)msg[ii+1] << 8 | (uint32_t)msg[ii+2] << 16;
					}
				}
			} else { break; }
			if (!flag) { break; }
			memcpy(binary_color,double_buffer,PIXELS*4);
		}
		if (total_bytes) {
			//std::cout << "MAIN THREAD: Received udp message..." << std::endl;
			//std::cout << "MAIN THREAD: Updating " << total_bytes << " bytes from udp..." << std::endl;
		}
	}
	this_thread::sleep_for(chrono::milliseconds(10)); 
  }
  std::cout << "MAIN THREAD: FINISHED" << std::endl;
  imu.reset();
  this_thread::sleep_for(chrono::milliseconds(200)); 
  FINISHED = true;
  this_thread::sleep_for(chrono::milliseconds(200)); 
  t1.join();
  GPIO_SET = 1<<5;
} 

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
#include <mutex>

#include "visualizations/base_visualizer.h"
#include "visualizations/dice.h"
#include "visualizations/gator.h"
#include "visualizations/sparky.h"
#include "visualizations/logo.h"
#include "visualizations/julia.h"
#include "visualizations/udp.h"

#include "imu/bno080.h"
#include "gpio/gpio.h"
#include "udp/UDPServer.h"

using namespace std;

//////////////////////////////////////////////////////////////
//// Globals and Prototypes
//////////////////////////////////////////////////////////////
extern bool CUBE_ON, FINISHED;
bool CUBE_ON = true, FINISHED = false;
extern const int PANEL_WIDTH, CHAIN_LENGTH, PIXELS;
const int PANEL_WIDTH = 64, CHAIN_LENGTH = 6, PIXELS = PANEL_WIDTH*PANEL_WIDTH*CHAIN_LENGTH;
extern uint32_t binary_color[];
uint32_t binary_color[PIXELS];
extern mutex mtx;
mutex mtx;

void cube_thread(); 

//////////////////////////////////////////////////////////////
//// LED Cube Main Thread
//////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
  	std::cout << "MAIN THREAD: Loading images..." << std::endl;
	std::array<uint32_t,PIXELS> blank;
	std::fill(blank.begin(),blank.end(),0);
	std::fill(binary_color,binary_color+PIXELS,0);
	std::cout << "MAIN THREAD: Finished loading images..." << std::endl;

  	std::cout << "MAIN THREAD: Setting output pins" << std::endl;
  	setup_io();
  	std::cout << "MAIN THREAD: Setting output pins finished..." << std::endl;

  	std::cout << "MAIN THREAD: Setting imu" << std::endl;
  	BNO080 imu(0x4B);
  	Eigen::Vector3d rotx,roty,rotz;
  	imu.reset();
  	std::cout << "MAIN THREAD: Setting imu finished..." << std::endl;

  	std::cout << "MAIN THREAD: Starting udp server" << std::endl;
  	UDPServer server(8080);
  

  	off_t control_shm_length = 4;
  	int control_shm_fd = shm_open("/control_shm",O_RDWR | O_CREAT,0666);
  	ftruncate(control_shm_fd, control_shm_length);
	assert(control_shm_fd > 0);

  	uint32_t *control_shm = (uint32_t *)mmap(NULL,control_shm_length,PROT_READ|PROT_WRITE,MAP_SHARED,control_shm_fd,0);
  	*control_shm = 5;


  	std::cout << "MAIN THREAD: Starting cube thread" << std::endl;
  	thread t1(cube_thread);

  	std::cout << "MAIN THREAD: Setting thread priorities" << std::endl;
	std::ofstream fid("/proc/sys/kernel/sched_rt_runtime_us",std::ofstream::out);
  	fid << "-1";
	fid.close();

	int policy = SCHED_FIFO;
  	sched_param sch_params;
  	sch_params.sched_priority = 99;
  	if (pthread_setschedparam(t1.native_handle(), policy, &sch_params) ) {
		std::cout << "MAIN LOOP: Thread priority failed..." << std::endl;
	}
	cpu_set_t cpuset;
  	CPU_ZERO(&cpuset);
  	CPU_SET(3, &cpuset);
  	if ( pthread_setaffinity_np(t1.native_handle(),sizeof(cpu_set_t), &cpuset) != 0 ) {
		std::cout << "MAIN LOOP: Thread affinity failed..." << std::endl;
  	}
	
  	uint32_t main_loop_iter = 0, mode = 0, next_mode = 0;
	std::array<VISUALIZER*,7> visualizer = {new VISUALIZER(imu),
						new DICE(imu),
						new LOGO(imu,"/usr/local/led_samples/ASEE_Combo_Bravo.txt"),
						new GATOR(imu),
						new SPARKY(imu),
						new JULIA(imu),
						new UDP(imu,server)
						};
	while (1) {
		if (main_loop_iter++ % 100 == 0) {
			next_mode = *control_shm;
		}
		next_mode -= (next_mode >= 2608) ? 2608 : 0;
		if (next_mode == 665341) { 
			std::cout << "MAIN THREAD: Exiting with mode: " << next_mode << std::endl;
			CUBE_ON = false; break; 
		}
		if (next_mode != mode) {
			std::cout << "MAIN THREAD: Mode switch from " << mode << " to " << next_mode << std::endl;
			visualizer[mode]->stop(binary_color);
			if (next_mode < visualizer.size()) {
				visualizer[mode]->stop(binary_color);
				visualizer[next_mode]->start(binary_color);
				mode = next_mode;
			} else {
				mode = 0;
			}
		}
		//std::cout << "MAIN THREAD: Running mode " << mode << std::endl;
		visualizer[mode]->run(binary_color);
		this_thread::sleep_for(chrono::milliseconds(8)); 
	}
	std::cout << "MAIN THREAD: FINISHED" << std::endl;
	imu.reset();
	this_thread::sleep_for(chrono::milliseconds(200));
	FINISHED = true;
	this_thread::sleep_for(chrono::milliseconds(200)); 
	t1.join();
	return 0;
}

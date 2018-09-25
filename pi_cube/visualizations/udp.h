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
#ifndef UDP_
#define UDP_

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include "base_visualizer.h"
#include "../udp/UDPServer.h"

class UDP : public VISUALIZER {
public:
	UDP(BNO080 &imu_, UDPServer &server_) : VISUALIZER(imu_), server(server_) {
	} 
	void start(uint32_t *led_data) {
	}
	void run(uint32_t *led_data) {
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
			memcpy(led_data,double_buffer.data(),24576*4);
		}
	}
private:
	UDPServer &server;
	std::array<uint32_t,24576> pixels;
};
#endif

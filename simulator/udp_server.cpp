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
#include <SFML/OpenGL.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////
extern std::atomic<bool> window_is_running;
extern std::atomic<uint8_t> RENDER_STATE_CHANGES;
extern std::vector<GLfloat> *led_color;
extern std::mutex mtx_data;

////////////////////////////////////////////////////////////
// Handle any udp packets seperately from rendering and user input
////////////////////////////////////////////////////////////
void udpThread() {
    //Simulation runs a UDP server on local port 8080 to receive commands
    sf::UdpSocket server;
    if (server.bind(8080) != sf::Socket::Done) { return; }
    server.setBlocking(false);
    std::vector<uint8_t> server_msg(5000,0);

    //Wait for valid led pointer
    while ( led_color == nullptr && window_is_running ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    //Process any UDP packets until there are non waiting
    while (window_is_running) {
        server_msg[0] = 0;
        sf::IpAddress sender; 
        unsigned short port;
        uint8_t flag = 0;
        size_t bytes_read;

        //Allow thread to sleep if a message isn't available
        if (server.receive(server_msg.data(), server_msg.size(), bytes_read, sender, port) != sf::Socket::Done) { 
        	std::this_thread::sleep_for(std::chrono::milliseconds(25));
        	continue; 
        }

        //Ensure messsage header is 128 and there are at least 7 bytes at port
        if (bytes_read > 6 && server_msg[0] == 128) {
            int row = server_msg[3];
            uint16_t length = server_msg[1] | (uint16_t)server_msg[2]<<8;
            uint16_t offset = server_msg[4] | (uint16_t)server_msg[5]<<8;
            flag = server_msg[6];
            //The total length of the message must match how many bytes were pulled, what happens if two messages come in?
            if (length == bytes_read) {
                for (int ii = 7,cnt=0; ii < length; ii+=3,++cnt) {
                    for(int jj = 0; jj < 4; ++jj) {
                        std::lock_guard<std::mutex> lock(mtx_data);
                        (*led_color)[ row*64*6*16 + cnt*16 + offset*16 + jj*4] = server_msg[ii]/255.f; 
                        (*led_color)[ row*64*6*16 + cnt*16 + offset*16 + jj*4+1] = server_msg[ii+1]/255.f;
                        (*led_color)[ row*64*6*16 + cnt*16 + offset*16 + jj*4+2] = server_msg[ii+2]/255.f;
                        (*led_color)[ row*64*6*16 + cnt*16 + offset*16 + jj*4+3] = 1.0f;
                    }
                }
            } 
            //Trigger update when flag comes through as 0
            if (!flag) {
                RENDER_STATE_CHANGES |= 0x8;
            }
        }
    }
}
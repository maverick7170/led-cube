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
#include <SFML/Network.hpp>
#include <thread>
#include <chrono>

////////////////////////////////////////////////////////////
// Library Functions
////////////////////////////////////////////////////////////
extern "C" {
    sf::UdpSocket *client = nullptr;

    int sf_bind(int port) {
        if (client != nullptr) {
            delete(client);
            client = nullptr;
        }
        client = new(sf::UdpSocket);
        if (client->bind(sf::Socket::AnyPort) != sf::Socket::Done) { 
            return -1; 
        }
        client->setBlocking(false);  
        return 0; 
    }

    int sf_update_raw(std::string address, int port, size_t delay, uint8_t bytes[]) {
        if (client == nullptr) { return -1; }
        uint16_t length = 1159, offset = 0, mask = 0xff;
        int sent = 0;
        sf::IpAddress recipient = address;
        for (int row = 0; row < 64; ++row) {
            uint8_t flag = 1;
            if (row == 63) { flag = 0; }
            std::vector<uint8_t> msg = {128, 
                                        static_cast<uint8_t>(length & mask), 
                                        static_cast<uint8_t>((length>>8) & mask), 
                                        static_cast<uint8_t>(row), 
                                        static_cast<uint8_t>(offset & mask), 
                                        static_cast<uint8_t>((offset>>8) & mask), 
                                        flag};
            msg.insert(msg.end(),bytes+row*64*6*3,bytes+((row+1)*64*6*3));
            if (client->send(msg.data(),msg.size(),recipient,port) == sf::Socket::Done) {++sent;}
            if (row % 3 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }
        }
        return sent;
    }

    int sf_update(uint8_t bytes[], int flag) {
        if (client == nullptr) { return -1; }
        if (!flag) {
            return sf_update_raw("127.0.0.1",8080,1,bytes);
        } else {
            return sf_update_raw("10.0.0.1",8080,1,bytes);
        }
    }
}
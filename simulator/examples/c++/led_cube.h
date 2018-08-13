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
#ifndef LED_CUBE_
#define LED_CUBE_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Network.hpp>
#include <thread>
#include <array>
#include <chrono>
#include <vector>

////////////////////////////////////////////////////////////
/// \brief Helper class for working with RGB data
////////////////////////////////////////////////////////////
class Pixel {
public:
    Pixel() = default;
    Pixel(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) 
    {}
    uint8_t r = 0, g = 0, b = 0;
};

////////////////////////////////////////////////////////////
/// \brief Helper class for communicating over UDP to cube
////////////////////////////////////////////////////////////
class LED_Cube {
public:
    ////////////////////////////////////////////////////////////
    /// \brief Default destructor
    ///
    //////////////////////////////////////////////////////////// 
    LED_Cube(int port = sf::Socket::AnyPort) :  white(255,255,255), red(255,0,0), green(0,255,0),
                                                blue(0,0,255), yellow(255,255,0), magenta(255,0,255), 
                                                aqua(0,255,255), salmon(250,128,114), silver(192,192,192), 
                                                lawn_green(124,252,0), purple(128,0,128), navy(0,0,128) {
        reset();
        if (client.bind(port) != sf::Socket::Done) { 
            return; 
        }
        client.setBlocking(false);  
    }

    ////////////////////////////////////////////////////////////
    /// \brief Reset all leds to black (value 0 for RGB)
    ///
    //////////////////////////////////////////////////////////// 
    void reset() {
        std::fill(leds.begin(),leds.end(),0);
    }

    ////////////////////////////////////////////////////////////
    /// \brief Set all leds to 1 color
    ///
    //////////////////////////////////////////////////////////// 
    void set_all_leds(Pixel pixel) {
        for (auto ii = 0; ii < leds.size(); ii+=3) {
            leds[ii] = pixel.r;
            leds[ii+1] = pixel.g;
            leds[ii+2] = pixel.b;
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Change the color of all leds on 1 face of the cube
    ///
    //////////////////////////////////////////////////////////// 
    void set_face_color(int face, Pixel pixel) {
        if (face < 0 || face > 5) { return; }
        for (auto row = 0; row < 64; ++row) {
            for (auto col = 0; col < 64; ++col) {
                auto index = face*64*3 + row*64*6*3 + col*3;
                leds[index] = pixel.r;
                leds[index+1] = pixel.g;
                leds[index+2] = pixel.b;
            }
        }
    }    

    ////////////////////////////////////////////////////////////
    /// \brief Change the color of 1 row of leds on 1 face
    ///
    //////////////////////////////////////////////////////////// 
    void set_row_face_color(int row, int face, Pixel pixel) {
        if (row < 0 || row > 63) { return; }
        if (face < 0 || face > 5) { return; }
        for (auto col = 0; col < 64; ++col) {
            auto index = row*64*6*3 + face*64*3 + col*3;
            leds[index] = pixel.r;
            leds[index+1] = pixel.g;
            leds[index+2] = pixel.b;
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Change the color of 1 col of leds on 1 face
    ///
    //////////////////////////////////////////////////////////// 
    void set_col_face_color(int col, int face, Pixel pixel) {
        if (col < 0 || col > 63) { return; }
        if (face < 0 || face > 5) { return; }
        for (auto row = 0; row < 64; ++row) {
            auto index = row*64*6*3 + face*64*3 + col*3;
            leds[index] = pixel.r;
            leds[index+1] = pixel.g;
            leds[index+2] = pixel.b;
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Change the color of 1 col of leds on 1 face
    ///
    //////////////////////////////////////////////////////////// 
    void set_led(int index, Pixel pixel) {
        if (index < 0 || index > 24575) { return; }
        leds[index*3] = pixel.r;
        leds[index*3+1] = pixel.g;
        leds[index*3+2] = pixel.b;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Send array of led colors to cube
    ///
    /// The RGB leds are simply a flat area of uint8 for every
    /// reg,green,blue color values
    ///
    //////////////////////////////////////////////////////////// 
    int update(int flag = 0) {
        if (!flag) {
            return update_raw("127.0.0.1",8080,1);
        } else {
            return update_raw("10.0.0.1",8080,1);
        }
    }

    ////////////////////////////////////////////////////////////
    // Public Member data
    //////////////////////////////////////////////////////////// 
    Pixel black,white,red,green,blue,yellow,magenta,aqua,salmon,silver,lawn_green, purple, navy;

private:
    ////////////////////////////////////////////////////////////
    /// \brief Internal UDP call for sending bytes over UDP
    ///
    /// Forms the correct UDP message type expected by the cube.
    ///
    /// \return type Number of bytes sent
    ///
    ////////////////////////////////////////////////////////////     
    int update_raw(std::string address, int port, size_t delay) {
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
            msg.insert(msg.end(),leds.begin()+row*64*6*3,leds.begin()+((row+1)*64*6*3));
            if (client.send(msg.data(),msg.size(),recipient,port) == sf::Socket::Done) {++sent;}
            if (row % 3 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }
        }
        return sent;
    }

    ////////////////////////////////////////////////////////////
    // Private Member data
    //////////////////////////////////////////////////////////// 
    sf::UdpSocket client;
    std::array<uint8_t,73728> leds;
};

#endif
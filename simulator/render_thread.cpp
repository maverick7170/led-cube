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
#ifdef USE_GLEW
#include <GL/glew.h>
#endif
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

#include "opengl/view_cube.h"
#include "opengl/led_cube.h"
#include "virtual_terminal/terminal.h"

////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////
extern std::atomic<bool> window_is_running;
extern std::atomic<uint8_t> RENDER_STATE_CHANGES;
extern std::mutex mtx_window, mtx_data, mtx_terminal;
extern double RENDER_STATE_DATA[6];
extern std::vector<GLfloat> *led_color;

////////////////////////////////////////////////////////////
// Handle rendering and user input seperately
////////////////////////////////////////////////////////////
void renderThread(sf::RenderWindow &window, Terminal &terminal) {
    //Texture is needed to show cube orientation in top-right of screen
    window.setActive(true);
    sf::Texture texture;
    #ifdef XCODE
    if (!texture.loadFromFile(resourcePath() + "/texture.jpg")) { std::cout << "Error" << std::endl; }
    #else
    if (!texture.loadFromFile("resources/texture.jpg")) { std::cout << "Error" << std::endl; }  
    #endif
    
	// Opengl settings
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClearDepth(1.f);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);   
    glClearColor(0.2f,0.2f,0.2f, 1.0f);   
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

    //Opengl led cube and view cube 
    ViewCube view_cube;
    view_cube.setup();
    LEDCube led_cube;
    led_cube.setup();
    led_color = led_cube.get_colors_ptr();

    //Bookkeeping
    double window_size_x = window.getSize().x, window_size_y = window.getSize().y;
    double window_ratio = static_cast<double>(window_size_x) / window_size_y;
    size_t iterations = 0;
    sf::Clock clock;

    //Render loop
    while (window_is_running) {

        //Reset
        if ( RENDER_STATE_CHANGES & 0x1) {
            std::lock_guard<std::mutex> lock(mtx_data);
            led_cube.resetView();
            view_cube.resetView();
            RENDER_STATE_CHANGES &= ~(0x1);
        }

        //Rotate
        if ( (RENDER_STATE_CHANGES >> 1) & 0x1) {
            std::lock_guard<std::mutex> lock(mtx_data);
            led_cube.rotate(RENDER_STATE_DATA[1],RENDER_STATE_DATA[2],RENDER_STATE_DATA[3]);
            view_cube.rotate(RENDER_STATE_DATA[1],RENDER_STATE_DATA[2],RENDER_STATE_DATA[3]);
            RENDER_STATE_CHANGES &= ~(0x2); 
        }

        //Scale
        if ( (RENDER_STATE_CHANGES >> 2) & 0x1) {
            std::lock_guard<std::mutex> lock(mtx_data);
            led_cube.scale(RENDER_STATE_DATA[0]);
            RENDER_STATE_CHANGES &= ~(0x4);
        }		

        //Update trigger from udp
        if ( (RENDER_STATE_CHANGES >> 3) & 0x1) {
            std::lock_guard<std::mutex> lock(mtx_data);
            led_cube.update();
            RENDER_STATE_CHANGES &= ~(0x8);
        }   

        //Window size change
        if ( (RENDER_STATE_CHANGES >> 4) & 0x1 ) {
            std::lock_guard<std::mutex> lock(mtx_data);
            window_size_x = RENDER_STATE_DATA[4]; 
            window_size_y = RENDER_STATE_DATA[5];
            window_ratio = static_cast<double>(window_size_x) / window_size_y;
            RENDER_STATE_CHANGES &= ~(0x10);      
        }

        //Begin drawing on active context: ~3.3 ms with glClear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Draw in main viewport for displaying cube: ~5.8 ms to draw cube
        glViewport(0, 0, window_size_x, window_size_y);
        led_cube.draw();

        //OpenGL drawing of orientation compass in top right corner: ~5.9 ms with drawing cube and compass
        sf::Texture::bind(&texture); 
        int adjusted_size = 175*window_size_y/720.f;
        glViewport(window_size_x-adjusted_size, window_size_y-adjusted_size, adjusted_size, adjusted_size);  
        view_cube.draw(); 

        //Draw terminal last as text overlay at the bottom of the screen
        glViewport(0, 0, window_size_x, window_size_y);
        {
            std::lock_guard<std::mutex> lock(mtx_terminal);
            terminal.draw(window_size_y*0.3);  
        }      

        //Update overall display: ~2.1 ms on OSX with just display (disable display scaling)
        ++iterations;
        std::lock_guard<std::mutex> lock(mtx_window);
        window.display();

	   //Help with vertical sync
	   //std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    //Time stastictics
    auto avg_milliseconds = clock.getElapsedTime().asMilliseconds();
    std::cout << "Average window refresh rate: " << avg_milliseconds/static_cast<double>(iterations) << " ms" << std::endl;
}

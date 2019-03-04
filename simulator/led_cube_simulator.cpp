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
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

#include "virtual_terminal/py.h"
#include "virtual_terminal/terminal.h"
#include "virtual_terminal/text_display.h"

#ifdef UNIX
#include <X11/Xlib.h>
#endif

////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////
extern std::atomic<bool> window_is_running;
extern std::atomic<uint8_t> RENDER_STATE_CHANGES;
extern std::mutex mtx_window, mtx_data, mtx_terminal;
extern double RENDER_STATE_DATA[6];
extern std::vector<GLfloat> *led_color;
std::atomic<bool> window_is_running(true);
std::atomic<uint8_t> RENDER_STATE_CHANGES(0);
std::mutex mtx_window, mtx_data, mtx_terminal;
double RENDER_STATE_DATA[6] = {0,0,0,0,0,0};
std::vector<GLfloat> *led_color = nullptr;

////////////////////////////////////////////////////////////
// Forward Declerations
////////////////////////////////////////////////////////////
void renderThread(sf::RenderWindow &window, Terminal &terminal);
void udpThread();
std::string resourcePath(void);

////////////////////////////////////////////////////////////
// Entry point of application
////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {	
    //Linux X11 multithreading
    #ifdef UNIX
    XInitThreads();
    #endif

    //Handle font issues for each os
    // Setup render window at 24 bits with verticle sync enabled
    const size_t SCREEN_WIDTH = sf::VideoMode::getDesktopMode().width; 
    const size_t WINDOW_WIDTH = SCREEN_WIDTH/1.5, WINDOW_HEIGHT = WINDOW_WIDTH/(1280./720.); 
    const int font_size = 32*WINDOW_WIDTH/1280.;
    #ifdef XCODE
    const std::string font_filename = resourcePath()+"UbuntuMono-R.ttf";
    #else
    const std::string font_filename = "resources/UbuntuMono-R.ttf";
    #endif


    //Force SFML to create Font sheet since we are using native OpenGL calls    
    sf::Font font; if (!font.loadFromFile(font_filename)) { }
    sf::RenderTexture w; w.create(800,800);
    w.setActive(true);
    sf::Text user_input;
    user_input.setFont(font);
    user_input.setCharacterSize(font_size);
    user_input.setFillColor(sf::Color::White);
    std::string ascii;
    for (char ii = 33; ii <= 126; ++ii) { 
        ascii.push_back(ii); 
    } 
    for (const auto ii : {1,2,3,4} ) {
        user_input.setString(ascii);
        w.clear();
        w.draw(user_input);
        w.display();    
    }
    w.setActive(false);
    font.getTexture(font_size).copyToImage().saveToFile("blah22.png");


    //std::cout << "Setting window to: " << WINDOW_WIDTH << "," << WINDOW_HEIGHT << std::endl;
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 24;
    contextSettings.stencilBits = 8;
    contextSettings.antialiasingLevel = 4;
    contextSettings.majorVersion = 4;
    contextSettings.minorVersion = 6;
    contextSettings.attributeFlags = sf::ContextSettings::Core;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "LED Cube Simulator", sf::Style::Default, contextSettings);
    //window.setVerticalSyncEnabled(true);
    
    //Initialize glew if needed
    #ifdef USE_GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout <<  "Glew failed to initialize: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    #endif

    
    //Virtual terminal to store user's commands and track history
    Terminal terminal(window,font,font_size);
    terminal.new_line();

    //Initialize python embedded system for running commands
    Py::setup();

    //Mouse
    sf::Vector2i last_mouse_position(-99999,-99999);

    //Do rendering in seperate thread
    window.setActive(false);
    std::thread t1(renderThread, std::ref(window), std::ref(terminal));
    std::thread t2(udpThread);

    //Track double clicks for resetting view
    sf::Clock ticker;
    sf::Time last_click_time = ticker.getElapsedTime(), last_key_time = ticker.getElapsedTime();
    sf::Event next_event;

    //SFML loop
    while ( window_is_running ) {
        //Process SFML events including mouse and keyboard
        std::vector<sf::Event> events;
        {
            std::lock_guard<std::mutex> lock(mtx_window);
            while (window.pollEvent(next_event)) {
                events.push_back(next_event);
            }
        }
        for (auto &event : events) {
            if (event.type == sf::Event::Closed) { 
                window_is_running = false;
                break;
            } else if (event.type == sf::Event::Resized) {
                std::lock_guard<std::mutex> lock(mtx_data);
                RENDER_STATE_DATA[4] = window.getSize().x;
                RENDER_STATE_DATA[5] = window.getSize().y;
                RENDER_STATE_CHANGES |= 0x10;
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    if ( (ticker.getElapsedTime()-last_click_time).asMilliseconds() < 250 ) {
                        std::lock_guard<std::mutex> lock(mtx_data);
                        RENDER_STATE_CHANGES |= 0x1;
                    } else {
                        last_click_time = ticker.getElapsedTime();
                    }
                }
            } else if ( event.type == sf::Event::MouseWheelScrolled ) {
                std::lock_guard<std::mutex> lock(mtx_data);           
                RENDER_STATE_DATA[0] = event.mouseWheelScroll.delta;
                RENDER_STATE_CHANGES |= 0x4;
            } else if ( event.type == sf::Event::MouseMoved ) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
                    if (last_mouse_position.x != -99999) {
                        double dx = mouse_position.x - last_mouse_position.x;
                        double dy = mouse_position.y - last_mouse_position.y;
                        double mag = sqrt(dx*dx + dy*dy);
                        if (fabs(mag) > 0 && fabs(mag) <= 100) { 
                            std::lock_guard<std::mutex> lock(mtx_data);
                            RENDER_STATE_DATA[1] = dx;
                            RENDER_STATE_DATA[2] = dy;
                            RENDER_STATE_DATA[3] = mag;
                            RENDER_STATE_CHANGES |= 0x2;
                        }
                    }
                    last_mouse_position = mouse_position;
                }
            } else if (event.type == sf::Event::KeyPressed) {
                std::lock_guard<std::mutex> lock(mtx_terminal);
                if (event.key.code == sf::Keyboard::Left) {
                    terminal.cursor_left();
                } else if (event.key.code == sf::Keyboard::Right) {
                    terminal.cursor_right();
                } else if (event.key.code == sf::Keyboard::Up) {
                    terminal.history_back();
                } else if (event.key.code == sf::Keyboard::Down) {
                    terminal.history_forward();
                }
            } else if (event.type == sf::Event::TextEntered) {
                auto key = event.text.unicode;
                last_key_time = ticker.getElapsedTime();
                std::lock_guard<std::mutex> lock(mtx_terminal);
                if (key == 8) {
                    terminal.erase();
                } else if (key == 10 || key == 13) {
                    terminal.run();
                } else if (key == 9) {
                    terminal.tab_complete();
                } else if (key < 128) {
                    terminal.insert(key);
                }
            }
            //Allow thread to sleep
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
    }

    //Wait for threads to finish and close out
    t1.join();
    t2.join();
    window.close();

    return 0;
}	

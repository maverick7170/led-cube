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
#ifndef TERMINAL_
#define TERMINAL_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mutex>
#include <vector>
#include <string>
#include "text_display.h"
#include "py.h"

////////////////////////////////////////////////////////////
/// \brief Class description
///
////////////////////////////////////////////////////////////
class Terminal 
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// Detials
    ///
    ////////////////////////////////////////////////////////////	
	Terminal(sf::RenderWindow &window, sf::Font &font_, int font_size_) : display(window,font_,font_size_) {
        max_line_length = window.getSize().x/font_size_*2;
		vertical_line_gap = font_size_*0.8;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////	
	void new_line() {
        std::lock_guard<std::mutex> lock(mtx_terminal);
		lines.push_back(">>> ");
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////		
	void cursor_left() {
        std::lock_guard<std::mutex> lock(mtx_terminal);
		if ( cursor_index < lines[last_row].size()-4) { ++cursor_index; }
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////		
	void cursor_right() {
        std::lock_guard<std::mutex> lock(mtx_terminal);
		if (cursor_index) { --cursor_index; }	
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////		
	void history_back() {
        std::lock_guard<std::mutex> lock(mtx_terminal);
		for (int ii = history_index-1; ii >= 0; --ii) {
			if (lines[ii].substr(0,3) == ">>>" || lines[ii].substr(0,3) == "...") {
				history_index = ii;
				break;
			}
		}
		lines[last_row] = lines[history_index];		
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////		
	void history_forward() {
        std::lock_guard<std::mutex> lock(mtx_terminal);
		if (history_index >= last_row) { return; }
		for (auto ii = history_index+1; ii <= last_row; ++ii) {
			if (lines[ii].substr(0,3) == ">>>" || lines[ii].substr(0,3) == "...") {
				history_index = ii;
				break;
			}
		}
		lines[last_row] = lines[history_index];		
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////		
	void erase() {
        std::lock_guard<std::mutex> lock(mtx_terminal);
		if (lines[last_row].size() > 4) {
			lines[last_row].erase(lines[last_row].size()-1-cursor_index,1);
        }		
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////		
	void insert(char key) {
        std::lock_guard<std::mutex> lock(mtx_terminal);
        if (lines[last_row].size() <= max_line_length) {
		  lines[last_row].insert( lines[last_row].size()-cursor_index,std::string(1,key) );
        }
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////
    void tab_complete() {
        std::lock_guard<std::mutex> lock(mtx_terminal);
        if (lines[last_row].size() <= 4) { return; }
        std::string y = lines[last_row].substr(4);
        std::string x = "x,y,z=TabCompleteNames('" + y + "')\n"
            "if y is None:\n"
            " k=[ii for ii in dir()+BUILT_IN_COMMANDS[x[0]] if ii[0] is not '_' if x == ii[0:len(x)] ]\n"
            " for m in k:\n"
            "  print(f'{m}')\n"
            "elif y in dir():\n"
            " if len(z) == 0:\n"
            "  k=[ii for ii in dir(eval(y)) if ii[0] is not '_']\n"
            " else:\n"
            "  k=[ii for ii in dir(eval(y)) if z == ii[0:len(z)] ]\n"
            " for m in k:\n"
            "  print(f'{y}.{m}(')\n";
        std::vector<std::string> output = Py::run(x);
        if (output.size() > 1) {
            lines.push_back(""); ++last_row;
            for  (auto o : output) { 
                if (lines[last_row].size() + o.size() > max_line_length) {
                    lines.push_back(""); ++last_row;
                }
                lines[last_row] += o + "  ";
            }
            lines.push_back(">>> "+y);  
            last_row = lines.size()-1;
            history_index = last_row;
            cursor_index = 0;
            if (last_row >= 7) { scroll_index += (last_row-6)-scroll_index; }
        }  else if (output.size() == 1) {
            lines[last_row] = ">>> " + output[0];
        }                
    }

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////		
	void run() {
        std::lock_guard<std::mutex> lock(mtx_terminal);
		if (lines[last_row].size() <= 4) { return; }
		std::string x = lines[last_row].substr(4);
		if ( x[x.size()-1] == ':') {
			compound_command += x+"\n";
			lines.push_back("... ");
		} else if (compound_command.size() && x.size() > 0) {
			compound_command += x+"\n";
			lines.push_back("... ");
		} else {
			if (x.size() > 5 && x.substr(0,4) == "run ") {
				x = "exec(open(\"./examples/python/" + x.substr(4) + "\").read())";
			}
			if (compound_command.size()) {
				x = compound_command+x;
				compound_command = "";
			}
			std::vector<std::string> output = Py::run(x);
			for  (auto o : output) { 
                for (auto kk = 0u; kk < o.size(); kk += 70) {
				    lines.push_back(o.substr(kk,70));
                }
			}
			lines.push_back(">>> ");
		}
		last_row = lines.size()-1;
		history_index = last_row;
        cursor_index = 0;
		if (last_row >= 8) { scroll_index += (last_row-7)-scroll_index; }		
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////    
    void draw(float y) {
        std::lock_guard<std::mutex> lock(mtx_terminal);
        for (auto ii = 0; ii < 8; ++ii,y-=vertical_line_gap) {
            if (ii + scroll_index >= lines.size()) {
                break;
            } else if (ii + scroll_index == lines.size()-1){
                display.draw(lines[ii+scroll_index],25,y,cursor_index);
            } else {
                display.draw(lines[ii+scroll_index],25,y,-1);
            }
        }
    }
	
    ////////////////////////////////////////////////////////////
    // Public Member data
    ////////////////////////////////////////////////////////////  	
	std::vector<std::string> lines;
	size_t scroll_index = 0, last_row = 0, cursor_index = 0, history_index = 0;
	std::string compound_command = "";
    TextDisplay display;
    size_t max_line_length, vertical_line_gap;
    std::mutex mtx_terminal;
};

#endif
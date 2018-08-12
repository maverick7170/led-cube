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
#include <Python.h>
#include <vector>
#include <string>

////////////////////////////////////////////////////////////
/// Forward Declerations
////////////////////////////////////////////////////////////
std::string resourcePath(void);

namespace Py {
    //Globals for namespace
	PyObject *sys, *io, *io_reader;

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////	
	void setup() {
		Py_SetProgramName((wchar_t*)"python");
		Py_Initialize();
   		sys = PyImport_ImportModule("sys");
    	io = PyImport_ImportModule("io");
    	PyRun_SimpleString("import random\n");
    	PyRun_SimpleString("import os\n");
    	PyRun_SimpleString("import sys\n");
		#ifdef XCODE
        PyRun_SimpleString(std::string("sys.path.append('" + resourcePath() + "/python')\n").c_str());
		#endif
    	PyRun_SimpleString("sys.path.append(os.getcwd()+'/examples/python')\n");
        PyRun_SimpleString("print(sys.path)\n");
    	//PyRun_SimpleString("sys.path.append(os.getcwd()+'/python/examples')\n");
    	PyRun_SimpleString("from cube_simulator import Pixel,rgb,RGB,Cube,TabCompleteNames,BUILT_IN_COMMANDS\n");
    	PyRun_SimpleString("cube = Cube()\n");
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////	
	std::vector<std::string> run(std::string x) {
		std::vector<std::string> output;
        io_reader = PyObject_CallMethod(io, "StringIO", nullptr);
		PyObject_SetAttrString(sys, "stdout", io_reader);
		PyObject_SetAttrString(sys, "stderr", io_reader);
		PyRun_SimpleString(x.c_str());
		PyObject *blah = PyObject_CallMethod(io_reader,"getvalue",nullptr);
		if (blah) {
			std::string s(PyUnicode_AsUTF8(blah));
			if (s.size() > 0) {
				size_t last_split = 0;
				for (auto cnt = 0u; cnt < s.size(); ++cnt) {
					if (s[cnt] == '\n') {
						output.push_back(s.substr(last_split,cnt-last_split));
						last_split = cnt+1;
					}
				}
				Py_XDECREF( blah );
			}
		}
		Py_XDECREF(io_reader);
		return output;
	}

}
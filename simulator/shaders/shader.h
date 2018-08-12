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
#ifndef SHADER_
#define SHADER_


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <string>

class Shader {

public:
    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    //////////////////////////////////////////////////////////// 
	Shader() = default;


    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    //////////////////////////////////////////////////////////// 
    void createShader(std::string vertex, std::string frag) {
        GLchar infoLog[INFOLOG_LEN];
        GLint vertexShader = loadShaderFromFile(GL_VERTEX_SHADER, vertex);
        GLint fragmentShader = loadShaderFromFile(GL_FRAGMENT_SHADER, frag);
        shader = glCreateProgram();
        glAttachShader(shader, vertexShader);
        glAttachShader(shader, fragmentShader);
        glLinkProgram(shader);
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, INFOLOG_LEN, NULL, infoLog);
            printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }


    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    //////////////////////////////////////////////////////////// 
    void useShader() {
    	glUseProgram(shader);
    }

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    //////////////////////////////////////////////////////////// 
    GLint getUniform(std::string name) {
    	return glGetUniformLocation(shader, name.c_str());
    }

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    //////////////////////////////////////////////////////////// 
    static GLint loadShaderFromFile(GLint type, std::string filename) {
        GLchar infoLog[INFOLOG_LEN];
        GLint success, shader = glCreateShader(type);
        std::string all_lines, next_line;
        std::ifstream fid(filename);
        while (std::getline(fid,next_line)) { all_lines += next_line + "\n"; }
        const GLchar* source = static_cast<const GLchar*>(all_lines.c_str());
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, INFOLOG_LEN, NULL, infoLog);
            printf("ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
        }
        return shader;
    }

private:
    ////////////////////////////////////////////////////////////
    // Public Member data
    //////////////////////////////////////////////////////////// 
	GLint shader, success;
	static const int INFOLOG_LEN = 512;
};
#endif
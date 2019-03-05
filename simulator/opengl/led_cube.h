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
#ifdef USE_GLEW
    #include <GL/glew.h>
#endif
#ifdef APPLE
    #include <OpenGL/gl3.h>
#endif
#include <vector>
#include <mutex>
#include <SFML/OpenGL.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include "drawable.h"


 
////////////////////////////////////////////////////////////
/// \brief Class description
////////////////////////////////////////////////////////////
class LEDCube : public Drawable
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Default destructor
    ///
    ////////////////////////////////////////////////////////////        
	LEDCube() = default;

    ////////////////////////////////////////////////////////////
    /// \brief Create the OpenGL indices for cube and compass
    ///
    /// Two VAOs are created along with the corresponding
    /// VBOs and shaders for drawing with modern OpenGL.
    ///
    ////////////////////////////////////////////////////////////        
	void setup() {
        std::vector<GLfloat> black = {0.3f,0.3f,0.3f,1.f};
        float led_dim = 6.5f, wall_dim = led_dim*1.2*31.5+led_dim; 
        glm::mat4 modelview = glm::mat4(1.0f);  
    	for (int row = 63; row >= 0; --row) {
            modelview = glm::mat4(1.0f);;
        	for (int p = 0; p < 6; ++p) {
            	modelview = glm::mat4(1.0f);;
            	switch (p) {
                	case 0:  
                            modelview = glm::rotate(modelview, glm::radians(90.f), glm::vec3(1,0,0)); break;
                	case 2:  
                            modelview = glm::rotate(modelview, glm::radians(90.f), glm::vec3(0,1,0)); break;
                	case 3:  
                            modelview = glm::rotate(modelview, glm::radians(180.f), glm::vec3(0,1,0)); break;
                	case 4: 
                            modelview = glm::rotate(modelview, glm::radians(270.f), glm::vec3(0,1,0)); break;
                	case 5: 
                            modelview = glm::rotate(modelview, glm::radians(-90.f), glm::vec3(1,0,0)); break;
            	}
                modelview = glm::translate(modelview, glm::vec3(0,0,wall_dim));
            	if (row == 63) {
            		GetQuadCorners(modelview,wall_dim,wall_vertex,wall_index);
            		for (auto ii = 0; ii < 4; ++ii) {
            			wall_color.insert(wall_color.end(),black.begin(),black.end());
                    }
            	}
                modelview = glm::translate(modelview, glm::vec3(0,0,led_dim/8.));
            	for (int col = 0; col < 64; ++col) {
                    glm::mat4 modelview_temp = modelview;
                    modelview_temp = glm::translate(modelview_temp, glm::vec3( (col-31.5)*led_dim*1.2,(row-31.5)*led_dim*1.2,0  ));
                	uint32_t color = 0;
                	std::vector<GLfloat> this_color = {(color&0xFF)/255.f,((color >> 8)&0xFF)/255.f,((color >> 16)&0xFF)/255.f,1.f};
                	GetQuadCorners(modelview_temp,led_dim/2.0,vertices,indices);  
                	for (auto ii = 0; ii < 4; ++ii) {
                		colors.insert(colors.end(),this_color.begin(),this_color.end());
                	}
            	}
        	}
    	}

        //Combine data for vbos
        for (auto &index : wall_index) {
            index += vertices.size()/3;
        }
        indices.insert(indices.end(), wall_index.begin(), wall_index.end());
    	vertices.insert(vertices.end(),wall_vertex.begin(),wall_vertex.end());
    	colors.insert(colors.end(),wall_color.begin(),wall_color.end());

        //Build and compile shader programs
        #ifdef XCODE
            shader.createShader(resourcePath()+"/shaders/led.vert",resourcePath()+"shaders/led.frag");
        #else
            shader.createShader("resources/shaders/led.vert","resources/shaders/led.frag");
        #endif
        
        //Create vaos for cube and compass along with needed vbo
        glGenBuffers(3, vbo);
        glGenVertexArrays(1, &vao);

        //VAO data for cube
        glBindVertexArray(vao);
        //Led vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), indices.data(), GL_STATIC_DRAW);
        //Led vertices
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //Led colors
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*colors.size(), colors.data(), GL_DYNAMIC_DRAW);	
        glEnableVertexAttribArray(1); 
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);        

        //Need uniforms for model-view-projection matrix in shaders
        mvp = shader.getUniform("mvp");

        //The mvps are different for each vao
        proj_mat = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 1.0f, 2500.f);
        view_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 100.0f, -1050.0f));
        model_mat = scale_mat = glm::mat4(1.0f);   
	}

    ////////////////////////////////////////////////////////////
    /// \brief Update the cube's current led colors
    ///
    /// Send the new led colors to the GPU on the corresponding VBO
    ///
    ////////////////////////////////////////////////////////////        
	void update() {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(GLfloat)*colors.size(),colors.data());
	}

    ////////////////////////////////////////////////////////////
    /// \brief Return reference to color vector
    ///
    ////////////////////////////////////////////////////////////        
    std::vector<GLfloat>* get_colors_ptr() {
        return &colors;
    }
 
private:
    ////////////////////////////////////////////////////////////
    // Private Member data
    //////////////////////////////////////////////////////////// 
	GLuint vbo[3];
    std::vector<GLfloat> wall_vertex, wall_color, vertices, colors;
    std::vector<GLuint> wall_index;    
};

#endif



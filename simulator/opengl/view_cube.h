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
#ifndef VIEW_CUBE_
#define VIEW_CUBE_

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
class ViewCube : public Drawable
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Default destructor
    ///
    ////////////////////////////////////////////////////////////        
	ViewCube() = default;

    ////////////////////////////////////////////////////////////
    /// \brief Create the OpenGL indices for view cube
    ///
    /// Two VAOs are created along with the corresponding
    /// VBOs and shaders for drawing with modern OpenGL.
    ///
    ////////////////////////////////////////////////////////////        
	void setup() {
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
                if (row == 63) {
                    glm::mat4 modelview_temp = modelview;
                    modelview_temp = glm::translate(modelview_temp, glm::vec3(0,0,wall_dim/5.f));
                    GetQuadCorners(modelview_temp,wall_dim/5.f,vertices,indices);
                    if (p && p < 5) {
                        for (auto ii = 0; ii <= 6; ii+=2) {
                            texture_coords[ii] += 107/540.f; 
                        }
                    } else if (p == 5) {
                        texture_coords = {10/540.f,211/220.f, 10/540.f,117/220.f,  104/540.f,117/220.f,   104/540.f,211/220.f};
                    }
                    texture.insert(texture.end(),texture_coords.begin(),texture_coords.end());
                }
        	}
    	}

        //Build and compile shader programs
        #ifdef XCODE
        shader.createShader(resourcePath()+"/shaders/comp.vert",resourcePath()+"/shaders/comp.frag");
        #else
        shader.createShader("resources/shaders/comp.vert","resources/shaders/comp.frag");
        #endif
        
        //Create vaos for cube and compass along with needed vbo
        glGenBuffers(3, vbo);
        glGenVertexArrays(1, &vao);

        //VAO data for view cube
        glBindVertexArray(vao);
        //Vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), indices.data(), GL_STATIC_DRAW);
        //Vertices
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW); 
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //Texture
        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*texture.size(), texture.data(), GL_STATIC_DRAW);  
        glEnableVertexAttribArray(0); 
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); 
        glBindVertexArray(0);  

        //Need uniforms for model-view-projection matrix in shaders
        mvp = shader.getUniform("mvp");

        //The mvps are different for each vao
        proj_mat = glm::perspective(glm::radians(90.0f), 1.0f , 1.0f, 2500.f);
        view_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -175.0f));
        model_mat = scale_mat = glm::mat4(1.0f);   

	}

private:
    ////////////////////////////////////////////////////////////
    // Private Member data
    //////////////////////////////////////////////////////////// 
    GLuint vbo[3];
    std::vector<GLfloat> vertices,texture,texture_coords = {10/540.f,104/220.f, 10/540.f,10/220.f,  104/540.f,10/220.f,   104/540.f,104/220.f};
};

#endif



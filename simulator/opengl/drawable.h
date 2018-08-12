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
#ifndef DRAWABLE_
#define DRAWABLE_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#ifdef USE_GLEW
    #include <GL/glew.h>
#endif
#ifdef APPLE
    #include <OpenGL/gl3.h>
#endif
#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <SFML/OpenGL.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include "../shaders/shader.h"

////////////////////////////////////////////////////////////
/// Forward Declerations
////////////////////////////////////////////////////////////
std::string resourcePath(void);

////////////////////////////////////////////////////////////
/// \brief Base class for managing VAO
////////////////////////////////////////////////////////////
class Drawable
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Default destructor
    ///
    ////////////////////////////////////////////////////////////        
	Drawable() = default;

    ////////////////////////////////////////////////////////////
    /// \brief Create the OpenGL indices for cube and compass
    ///
    /// Two VAOs are created along with the corresponding
    /// VBOs and shaders for drawing with modern OpenGL.
    ///
    ////////////////////////////////////////////////////////////        
	virtual void setup() = 0; 

    ////////////////////////////////////////////////////////////
    /// \brief Reset the cube's orientation back to default
    ///
    /// Set the model view matrix back to identity and the scale to 1
    ///
    ////////////////////////////////////////////////////////////
    virtual void resetView() {
        model_mat = scale_mat = glm::mat4(1.0f); 
        scale_factor= 1.0f;  
        mvp_dirty_flag = true;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Rotate the virtual cube based on mouse position
    ///
    /// Use the mouse's change in x and y to create a vector for
    /// rotating the virtual cube. The magnitude of that vector
    /// determines the amount of rotation.
    ///
    ////////////////////////////////////////////////////////////  
    virtual void rotate(double dx, double dy, double mag) {
        dx /= mag; 
        dy /= mag;
        model_mat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glm::radians(mag/3.f)), glm::vec3(dy,dx,0) )*model_mat;
        mvp_dirty_flag = true;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Scale the virtual cube base on the mouse scrollwheel.
    ///
    /// Details
    ///
    ////////////////////////////////////////////////////////////  
    virtual void scale(double delta) {
        if (delta > 0) {
            scale_factor += 0.025;
        } else if (delta < 0) {
            scale_factor -= 0.025;
        }
        scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor));
        mvp_dirty_flag = true;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////  
    virtual void windowChange(size_t x, size_t y) {
        proj_mat = glm::perspective(glm::radians(60.0f), static_cast<float>(x)/y, 1.0f, 2500.f);  
    }

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    ////////////////////////////////////////////////////////////        
	virtual void draw() {
        shader.useShader();
        if (mvp_dirty_flag) {         
            glm::mat4 camera = proj_mat*view_mat*model_mat*scale_mat;
            glUniformMatrix4fv(mvp,1,GL_FALSE,&camera[0][0]);
            mvp_dirty_flag = false;
        }
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 
        glBindVertexArray(0);  
    }

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    //////////////////////////////////////////////////////////// 
    static void GetQuadCorners(const glm::mat4 &modelview, float dim, std::vector<GLfloat> &v, std::vector<GLuint> &indx) 
    {
        glm::vec4 corners[4];
        corners[0] = modelview*glm::vec4(-dim,-dim,0.f,1.f); //bot_left 1,31
        corners[1] = modelview*glm::vec4(-dim,dim,0.f,1.f); //top_left  1,4
        corners[2] = modelview*glm::vec4(dim,dim,0.f,1.f); //top_right  18,4 
        corners[3] = modelview*glm::vec4(dim,-dim,0.f,1.f); //bot_right  18,31
        GLuint count = v.size()/3;
        for (auto ii = 0; ii < 4; ++ii) {
            v.push_back(corners[ii][0]);
            v.push_back(corners[ii][1]);
            v.push_back(corners[ii][2]);
        }
        indx.push_back(count); indx.push_back(count+1); indx.push_back(count+2);
        indx.push_back(count); indx.push_back(count+2); indx.push_back(count+3);
    }

protected:
    ////////////////////////////////////////////////////////////
    // Protected Member data
    ////////////////////////////////////////////////////////////    
    Shader shader;  
    GLuint vao;
    std::vector<GLuint> indices;
    GLint mvp; 
    glm::mat4 proj_mat, view_mat, model_mat, scale_mat;

private:
    ////////////////////////////////////////////////////////////
    // Private Member data
    //////////////////////////////////////////////////////////// 
    bool mvp_dirty_flag = true;
    float scale_factor= 1;
  
};

#endif



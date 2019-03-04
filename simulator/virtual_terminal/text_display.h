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
#ifndef TEXT_DISPLAY_H
#define TEXT_DISPLAY_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#ifdef APPLE
    #include <OpenGL/gl3.h>
#endif
#include <string>
#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include "../shaders/shader.h"

////////////////////////////////////////////////////////////
// Forward declerations
////////////////////////////////////////////////////////////
std::string resourcePath(void);

////////////////////////////////////////////////////////////
/// \brief Class description
///
////////////////////////////////////////////////////////////
class TextHandler {
public:
	TextHandler() = default;
	sf::Vector2<float> coords[4];
	float width,height,x_shift,y_shift;
};

////////////////////////////////////////////////////////////
/// \brief Class description
///
////////////////////////////////////////////////////////////
class TextDisplay {
public:
    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    //////////////////////////////////////////////////////////// 	
	TextDisplay(sf::RenderWindow &window, sf::Font &font_, int font_size_) :  
																			 font(font_), 
																			 font_size(font_size_),  
																			 cursor_height(font_size/2) {
		Projection = glm::ortho(0.0f, static_cast<float>(window.getSize().x), 0.0f, static_cast<float>(window.getSize().y), -1.f, 100.f);
		max_line_length = window.getSize().x/font_size*2;
		font.getTexture(font_size).copyToImage().saveToFile("blah33.png");


		/*
		sf::Text user_input;
		user_input.setFont(font);
		user_input.setCharacterSize(font_size);
		user_input.setFillColor(sf::Color::White);
		std::string ascii = "><";
		for (char ii = 33; ii <= 126; ++ii) { 
			//ascii.push_back(ii); 
		} 
		user_input.setString(ascii);
		for (auto ii = 0; ii < 10; ++ii) {
			window.clear();
			window.draw(user_input);
			window.display();
		}
		window.clear();
		*/
		
		auto g = font.getGlyph('a', font_size, false);
		character_width = g.advance;
		character_height = character_width*1.2f;
		baseline = (character_height-g.bounds.height)/2.f;
		a_bounds_top = g.bounds.top;

		// Build and compile shader program. 
        #ifdef XCODE
        shader.createShader(resourcePath()+"/shaders/text.vert",resourcePath()+"/shaders/text.frag");
        #else
        shader.createShader("resources/shaders/text.vert","resources/shaders/text.frag");
        #endif
        
        mvp = shader.getUniform("mvp");
        shader.useShader();
		glUniformMatrix4fv(mvp,1,GL_FALSE,&Projection[0][0]);

		//Handle texture coordinates outside of 0 to 1
		sf::Texture::bind(&font.getTexture(font_size));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  		//float color[] = { 1.0f, 0.0f, 0.0f, 0.5f };
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

		this->load_glyphs();
        this->setup_vao();
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    //////////////////////////////////////////////////////////// 
	void load_glyphs() {
		for (size_t ii = 32; ii <= 126; ++ii) { 
			auto g = font.getGlyph(static_cast<char>(ii), font_size, false);
			auto ss = font.getTexture(font_size).getSize();
			float text_height = ss.y, text_width = ss.x;
			auto left = g.textureRect.left/text_width, top = g.textureRect.top/text_height, height = g.textureRect.height/text_height, width = g.textureRect.width/text_width;
			all_text[ii].coords[0] = sf::Vector2<float>(left,top);
			all_text[ii].coords[1] = sf::Vector2<float>(left,top+height);
			all_text[ii].coords[2] = sf::Vector2<float>(left+width,top+height);
			all_text[ii].coords[3] = sf::Vector2<float>(left+width,top);
			all_text[ii].width = g.textureRect.width;
			all_text[ii].height = g.textureRect.height/2.f;
			all_text[ii].x_shift = g.bounds.left;
			all_text[ii].y_shift =  a_bounds_top-g.bounds.top - (baseline - (character_height-g.bounds.height)/2.f);
		}
		all_text[static_cast<int>(' ')] = all_text[static_cast<int>('a')];
		for (size_t ii = 0; ii < 4; ++ii) {
			all_text[static_cast<int>(' ')].coords[ii] = sf::Vector2<float>(1,1);
		}
		font.getTexture(font_size).copyToImage().saveToFile("blah55.png");
	}

    ////////////////////////////////////////////////////////////
    /// \brief Description
    ///
    /// Details
    ///
    /// \return type
    ///
    //////////////////////////////////////////////////////////// 
	void setup_vao() {
		indices.resize(6*max_line_length); 
		for (size_t ii = 0, index = 0; ii < indices.size(); ii += 6, index += 4) {
			indices[ii] = index;    indices[ii+1] = index+1; indices[ii+2] = index+2;
			indices[ii+3] = index;  indices[ii+4] = index+2; indices[ii+5] = index+3; 
		}
		texture.resize(8*max_line_length,0.f);
		vertices.resize(12*max_line_length,0.f); 

		//Initialize vao with vbo
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(3, vbo);
    	// Index Attribute 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GL_FLOAT), indices.data(), GL_STATIC_DRAW);
    	// Position attribute 
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GL_FLOAT), vertices.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    	// Texture attribute 
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, texture.size()*sizeof(GL_FLOAT), texture.data(), GL_DYNAMIC_DRAW);   
		glEnableVertexAttribArray(0); 
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
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
	void draw(std::string next_line, float cx, float cy, float cursor_x) {
		shader.useShader();
		sf::Texture::bind(&font.getTexture(font_size));
		glBindVertexArray(vao);
		size_t num_of_vertices = std::min(next_line.size(),max_line_length-3);
		size_t ii = 0, offset;
		for (; ii < num_of_vertices; ++ii) {
			auto letter = all_text[static_cast<int>(next_line[ii])];
			offset = (ii+2)*8;
			assert(offset+8 < texture.size());
			for (size_t kk = 0, jj = 0; kk < 8; kk+=2, ++jj) {
				texture[offset+kk] = letter.coords[jj].x;
				texture[offset+kk+1] = letter.coords[jj].y;
			}
			offset = (ii+2)*12;
			assert(offset+11 < vertices.size());
			vertices[offset] = cx + letter.x_shift;
			vertices[offset+1] = cy + letter.height + letter.y_shift;
			vertices[offset+2] = 0;
			vertices[offset+3] = cx + letter.x_shift;
			vertices[offset+4] = cy + -letter.height + letter.y_shift;
			vertices[offset+5] = 0;
			vertices[offset+6] = cx + letter.x_shift + letter.width;
			vertices[offset+7] = cy + -letter.height + letter.y_shift;
			vertices[offset+8] = 0;
			vertices[offset+9] = cx + letter.x_shift + letter.width;
			vertices[offset+10] = cy + letter.height + letter.y_shift;
			vertices[offset+11] = 0;
			cx += character_width;
		}
		//Overlay
		std::copy(tex_border.begin(),tex_border.end(),texture.begin());
		std::copy(overlay.begin(),overlay.end(),vertices.begin());
		//Cursor
		std::copy(tex_white.begin(),tex_white.end(),texture.begin()+8);
		if (cursor_x >= 0) {
			cx -= cursor_x*character_width;
		} else {
			cx = -100;
		}
		vertices[12] = cx;
		vertices[13] = cy-cursor_height;
		vertices[14] = -0.1;
		vertices[15] = cx;
		vertices[16] = cy+cursor_height;
		vertices[17] = -0.1;
		vertices[18] = cx+character_width;
		vertices[19] = cy+cursor_height;
		vertices[20] = -0.1;
		vertices[21] = cx+character_width;
		vertices[22] = cy-cursor_height;
		vertices[23] = -0.1;

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferSubData(GL_ARRAY_BUFFER,0,texture.size()*sizeof(GL_FLOAT),texture.data());
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferSubData(GL_ARRAY_BUFFER,0,vertices.size()*sizeof(GL_FLOAT),vertices.data());           
		glDrawElements(GL_TRIANGLES, (num_of_vertices+2)*6, GL_UNSIGNED_SHORT, 0);		
		glBindVertexArray(0);   
	}
private:
    ////////////////////////////////////////////////////////////
    // Private Member data
    //////////////////////////////////////////////////////////// 
    sf::Font &font;	
	//const sf::Texture &font_texture;
	int font_size;
	float character_width, character_height, baseline, a_bounds_top;
	GLuint vao;
	GLuint vbo[3];
	std::vector<GLushort> indices;
	std::vector<GLfloat> texture, vertices, overlay = {0,0,-1, 0.,100,-1, 100,100,-1, 100,0,-1},
				 		 tex_border = {2,2, 2,4, 4,4, 4,2},
						 tex_white = {0,0, 0,0, 0,0, 0,0};
	TextHandler all_text[127];
	glm::mat4 Projection;
	Shader shader;
	GLint mvp;
	size_t max_line_length,cursor_height;
};

#endif
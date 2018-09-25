////////////////////////////////////////////////////////////
////
//// LED Cube for Teaching Introductory Programming Concepts
//// Copyright (C) 2018 Ira Hill (ijh3@ufl.edu)
////
//// This program is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////
#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include <iostream>
#include <Box2D/Box2D.h>

class Line {
	public:
		Line() = default;
		Line(const b2Vec2 &a, const b2Vec2 &b) {
			if ( fabs(b.x-a.x) > 1e-6 ) {
				m_line = (b.y-a.y)/(b.x-a.x);
				b_line = a.y - m_line*a.x;
			} else {
				vertical = true;
			}
			if (a.x < b.x) { xmin = a.x; xmax = b.x; }
			else { xmin = b.x; xmax = a.x; }
			if (a.y < b.y) { ymin = a.y; ymax = b.y; }
			else { ymin = b.y; ymax = a.y; }
		}
		double getY(double x) const {
			return m_line*x+b_line;
		}
		double getX(double y) const {
			return (y-b_line)/m_line;
		}
		double m_line,b_line,xmin,xmax,ymin,ymax;
		bool vertical = false;
};


class DebugDraw : public b2Draw {
public:
		DebugDraw(uint32_t *pixels_, size_t width_, size_t height_) : width(width_), height(height_)  {
			pixels = pixels_;
			scale_x = width/384.; 
			scale_y = height/64.;
		}
		void DrawParticles (const b2Vec2 *centers, float32 radius, const b2ParticleColor *colors, int32 count) {
			//std::cout << "Drawing particles" << std::endl;
			for (auto ii = 0; ii < count; ++ii) {
				DrawSolidCircle(centers[ii],radius,b2Vec2(1,0),b2Color(colors[ii].r/255.,colors[ii].g/255.,colors[ii].b/255.));
			}
		}
		void DrawSolidPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) {
			//std::cout << "Drawing solid polygons" << std::endl;
			std::vector<Line> lines;
			float xlow = vertices[0].x, xhigh = vertices[0].x, ylow = vertices[0].y, yhigh = vertices[0].y;
			for (auto ii = 0; ii < vertexCount; ++ii) {
				xlow = std::min(xlow,vertices[ii].x);
				xhigh = std::max(xhigh,vertices[ii].x);
				ylow = std::min(ylow,vertices[ii].y);
				yhigh = std::max(yhigh,vertices[ii].y);
				if (ii == vertexCount-1) {
					lines.push_back(Line(vertices[ii],vertices[0]));
				} else {
					lines.push_back(Line(vertices[ii],vertices[ii+1]));
				}
			}
			for (float r = floor(ylow); r <= ceil(yhigh); r+=0.2) {
				for (float c = floor(xlow); c <= ceil(xhigh); c+=0.2) {
					int cnt = 0;
					for (const auto &line : lines) {
						if (line.vertical) {
							if (fabs(c-line.xmin) <= 0.5 && r >= line.ymin && r <= line.ymax) {
								cnt = 1; break;
							} 
						} else if ( fabs(line.m_line) <= 1e-6 ) {
							if (c >= line.xmin && c <= line.xmax ) {
								cnt = 1; break;
							} 
						} else if (r >= floor(line.ymin) && r <= ceil(line.ymax))  {
							if (c <= line.getX(r)) {
								++cnt;
							}
						}
					}
					if (cnt % 2 == 1) {
						ChangePixels((64-r)*scale_y,c*scale_x,color);
					}
				}
			}
		}
		void DrawCircle (const b2Vec2 &center, float32 radius, const b2Color &color) {
			//std::cout << "Drawing circle" << std::endl;
		}
		void DrawSolidCircle (const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color) {
			auto x = center.x*scale_x, y = (64-center.y)*scale_y;
			auto new_radius = radius*scale_x, new_radius_2 = new_radius*new_radius;
			//std::cout << "Drawing solid circle at " << x << "," << y << " with radius " << new_radius << std::endl;
			for (float r = y-new_radius; r < y+new_radius; r+=0.1) {
				for (float c = x-new_radius; c < x+new_radius; c+=0.1) {
					if ( (y-r)*(y-r) + (x-c)*(x-c) <= new_radius_2) {
						ChangePixels(r,c,color);
					}
				}
			}
		}
		void DrawSegment (const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color) {
			//std::cout << "Drawing segment" << std::endl;
		}
		void DrawTransform (const b2Transform &xf) {
			//std::cout << "Drawing transform" << std::endl;
		}
		void DrawPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) {
			//std::cout << "Drawing polygon" << std::endl;
		}
		void ChangePixels(float r, float c, const b2Color &color) {
			int index = (int)(round(r)*width+round(c));
			if ( r >= 0 && c >= 0 && round(r) < height && round(c) < width ) {
				pixels[index] = (uint32_t)(color.r*255) | (uint32_t)(color.g*255) <<  8 | (uint32_t)(color.b*255) << 16;
			}
		}
private:
	uint32_t *pixels;
	size_t width = 384, height = 64;
	double scale_x = 1, scale_y = 1;
};

#endif

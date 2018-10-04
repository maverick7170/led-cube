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

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include "lf_animation.h"

void LiquidFunAnimation::Step() {
	contactL.ClearContacts();
	world.Step(timeStep, velocityIterations, positionIterations, particleIterations);
}

void LiquidFunAnimation::Draw() {
   	world.DrawDebugData();
}

void LiquidFunAnimation::CreateWalls() {
	//Walls: 1 body multiple fixtures
	b2BodyDef wallsDef;
	b2Body* walls; 
	walls = world.CreateBody(&wallsDef); 

	//Bottom Wall
	b2PolygonShape wallShape;
	std::array<b2Vec2,4> coords = {b2Vec2(0,1.),b2Vec2(0,0.),b2Vec2(383,0.),b2Vec2(383,1.)};
	wallShape.Set(coords.data(),coords.size());
	walls->CreateFixture(&wallShape,0.f);

	//Top Wall
	coords = {b2Vec2(0,64.),b2Vec2(0,63.),b2Vec2(383,63.),b2Vec2(383,64.)};
	wallShape.Set(coords.data(),coords.size());
	walls->CreateFixture(&wallShape,0.f);

	//Left Wall
	for (int ii = 0; ii < 6; ++ii) {
		int offset = (ii > 0) ? -2 : 0;
		coords = {b2Vec2(offset+ii*64,64.),b2Vec2(1+ii*64,64.),b2Vec2(1+ii*64,0.),b2Vec2(offset+ii*64,0.)};
		wallShape.Set(coords.data(),coords.size());
		walls->CreateFixture(&wallShape,0.f);
	}

	//Right Wall
	coords = {b2Vec2(382,64.),b2Vec2(382,0.),b2Vec2(383,0.),b2Vec2(383,64.)};
	wallShape.Set(coords.data(),coords.size());
	walls->CreateFixture(&wallShape,0.f);
	
	/*for (auto ii = 0; ii < wallShape.GetVertexCount(); ++ii) {
		auto point = wallShape.GetVertex(ii);
		std::cout << point.x << "," << point.y << std::endl;
	}*/
}

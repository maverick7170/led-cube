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
#ifndef LIQUIDFUNANIMATION_H
#define LIQUIDFUNANIMATION_H

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>
#include <array>
#include "debug_draw.h"
#include <Box2D/Box2D.h>

class ContactListener : public b2ContactListener {
public:
	void ClearContacts() {
		m_contactPoints.clear();
	}
	void BeginContact(b2Contact *contact) {
		if (contact->GetFixtureA()->GetUserData() != NULL || contact->GetFixtureB()->GetUserData() != NULL) {
			b2WorldManifold worldManifold;
			contact->GetWorldManifold(&worldManifold);
			m_contactPoints.push_back(worldManifold.points[0]);	
			//m_contact = true;
			//std::cout << "Contact at " << worldManifold.points[0].x << "," << worldManifold.points[0].y << std::endl;
		}
	}
	void EndContact(b2Contact* contact) {}
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {  }
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {}	
	std::vector<b2Vec2> m_contactPoints;
};


class LiquidFunAnimation {
public:
	LiquidFunAnimation(size_t width, size_t height, b2Vec2 gravity, uint32_t *pixels) : WINDOW_WIDTH(width), WINDOW_HEIGHT(height), world(gravity), drawer(pixels,width,height) 
	{
		world.SetContactListener(&contactL);
		drawer.SetFlags(DebugDraw::e_shapeBit | DebugDraw::e_particleBit );
		world.SetDebugDraw(&drawer);
		b2ParticleSystemDef particleSystemDef;
		m_particleSystem = world.CreateParticleSystem(&particleSystemDef);
		m_particleSystem->SetRadius(0.3f);
		m_particleSystem->SetMaxParticleCount(5000);
		m_particleSystem->SetDestructionByAge(true);
		m_particleSystem->SetDamping(0.5f);
	}
	virtual ~LiquidFunAnimation() {}
	void Step();
	void Draw();
	void CreateWalls();
	virtual void ProcessContacts() = 0;

	const size_t WINDOW_WIDTH = 384, WINDOW_HEIGHT = 64;
	b2World world;
	DebugDraw drawer;
	ContactListener contactL;
	b2ParticleSystem *m_particleSystem;
private:
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6, positionIterations = 2, particleIterations = 1;	
};

#endif

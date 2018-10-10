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
#ifndef SPARKY_H
#define SPARKY_H

//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include <random>
#include <ctime>
#include "lf_animation.h"

std::mt19937 engine(time(0));

class Explosion {
public:
	Explosion(b2ParticleSystem *m_particleSystem, const b2Vec2 center) : rand_color(0,255), rand_radius(1,4), rand_dir(-30,30), rand_life(1,5)  {
		b2CircleShape circle;
		circle.m_radius = rand_radius(engine);	
		b2ParticleGroupDef pd;
		//pd.groupFlags = b2_rigidParticleGroup | b2_solidParticleGroup;
		//b2_powderParticle | b2_springParticle
		pd.flags = b2_powderParticle |  b2_colorMixingParticle;
		pd.shape = &circle;
		pd.position = center;
		pd.color.Set(rand_color(engine),rand_color(engine),rand_color(engine),255);
		pd.userData = (void*)1;
		pd.linearVelocity = b2Vec2();
		m_group = m_particleSystem->CreateParticleGroup(pd);

		auto start_index = m_group->GetBufferIndex();
		b2Vec2 *pos = m_particleSystem->GetPositionBuffer();
		b2Vec2 *vel = m_particleSystem->GetVelocityBuffer();
		for (auto ii = start_index; ii < start_index + m_group->GetParticleCount(); ++ii) {
			m_particleSystem->SetParticleLifetime(ii,rand_life(engine));
			vel[ii] = (pos[ii] - center)*rand_dir(engine);
		}
	}
private:
	b2ParticleGroup *m_group;
	std::uniform_int_distribution<> rand_color, rand_radius,rand_dir;
        std::uniform_real_distribution<double> rand_life;
};

class LFSparky : public LiquidFunAnimation {
public:
	LFSparky(size_t width, size_t height, size_t bodies, uint32_t *pixels) : LiquidFunAnimation(width, height, b2Vec2(0,0), pixels), engine(time(0)), rand_x(2,382), rand_y(2,62), rand_percent(0,1) {
		/*b2PolygonShape pentagon;
		b2Vec2 pentagon_[5];
		float radius = 3.0, angle = 0;
		for (auto ii = 0; ii < 5; ++ii, angle += 72) {
			double x = -sin(angle*M_PI/180.)*radius, y = cos(angle*M_PI/180.)*radius;
			pentagon_[ii] = b2Vec2(x,y);
		}
		pentagon.Set(pentagon_,5);
		b2CircleShape circle;
		circle.m_radius = 3.;	
		for (size_t jj = 1; jj < 6; ++jj) {
			for (size_t ii = 0; ii < bodies; ++ii) {
				b2BodyDef body_def;
				body_def.type = b2_dynamicBody;
				body_def.position.Set(32+jj*64,10+ii*13);
				b2Body* dynamicBody = world.CreateBody(&body_def);
				dynamicBody->SetLinearVelocity(b2Vec2(uniform_int(-10,10),uniform_int(-10,10)));	
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circle;
				fixtureDef.density = 0.5f;
				fixtureDef.friction = 0.5f;
				fixtureDef.restitution = 0.5f;
				fixtureDef.userData = (void*)1;
				dynamicBody->CreateFixture(&fixtureDef);
			}
		}*/
	}

	void ProcessContacts() {
		if (rand_percent(engine) <= 0.1) {
			b2Vec2 point(rand_x(engine),rand_y(engine));
   			Explosion(m_particleSystem,point);
		}
		//std::cout << "Number of contacts: " << contactL.m_contactPoints.size() << std::endl;
	   	//return contactL.m_contactPoints;
   		//if ( contactL.m_contactPoints.size() ) {
   		//	explosions.push_back( Explosion(m_particleSystem,contactL.m_contactPoints[0]) );
   		//}
	}
private:
	std::vector<Explosion> explosions;
	std::mt19937 engine;
	std::uniform_real_distribution<double> rand_x,rand_y,rand_percent;
};

#endif

#ifndef PARTICLE_SIM
#define PARTICLE_SIM

#include <iostream>
#include <Box2D/Box2D.h>
#include <stdio.h>
#include <vector>
#include <random>
#include <ctime>
#include <memory>
#include "particle_vfx.h"


class ContactListener : public b2ContactListener {
	public:
	void BeginContact(b2Contact *contact) {
		//std::cout << "cc!" << std::endl;
		if (contact->GetFixtureA()->GetUserData() != NULL || contact->GetFixtureB()->GetUserData() != NULL) {
			b2WorldManifold worldManifold;
			contact->GetWorldManifold(&worldManifold);
		        m_contactPoint.push_back(worldManifold.points[0]);	
			m_contact = true;
			//std::cout << "Contact at " << m_contactPoint.x << "," << m_contactPoint.y << std::endl;
		}
	
	}
	std::vector<b2Vec2> m_contactPoint;
	bool m_contact = false;

};

class ParticleSim {
	public:
		ParticleSim(b2Vec2 gravity);
		int step(uint32_t pixels[], uint32_t clean_color[]);
		void reset();
		float randFloat(double a, double b);
		uint32_t randUint(uint32_t a, uint32_t b);
		void Sparky();
		void Water();
		void CreateWalls();
	public:
		b2World world;
		std::mt19937 engine;
		const float32 timeStep = 1.0f / 60.0f;
		const int32 velocityIterations = 8;
		const int32 positionIterations = 3;
		const int32 particleIterations = 3;
		b2Body *walls, *obstacles;
		b2ParticleSystem *m_particleSystem;
		double angle;
		std::vector<b2Vec2> saved_pos;
		std::vector<b2ParticleColor> saved_color;
		ContactListener contactL;
		std::vector<b2Body*> dynamicBodies;
		std::vector<std::shared_ptr<ParticleVFX>> vfx;
		std::vector<uint32_t> dynamicBodiesColors;
  		uint32_t color_test[10] = { 15, 15<<8, 15<<16, 15 | 15<<8, 15 | 10<<8, 8<<16, 15<<8 | 15<<16, 8 | 8<<16, 15 | 12<<8 | 12<<16, 15|15<<8|15<<16 };  
		bool is_water = false;
		std::array<double,3> impulse = {0,0,0};
		std::array<double,3> gravity = {0,0,0};
};
#endif

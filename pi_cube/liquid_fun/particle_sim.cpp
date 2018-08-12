#include "particle_sim.h"

ParticleSim::ParticleSim(b2Vec2 gravity) : world(gravity), engine(time(0))
{
	b2ParticleSystemDef particleSystemDef;
	m_particleSystem = world.CreateParticleSystem(&particleSystemDef);
	m_particleSystem->SetRadius(0.03f);
	m_particleSystem->SetDamping(0.4f);
}

void ParticleSim::Water() {
	is_water = true;
	double scale = 16.;
	b2BodyDef obstaclesDef;
	obstacles = world.CreateBody(&obstaclesDef);
	b2PolygonShape triangle, diamond, pentagon, hexagon;
	const b2Vec2 triangle_[3] = {b2Vec2(95/scale,(64-19)/scale),b2Vec2(107/scale,(64-39)/scale),b2Vec2(84/scale,(64-39)/scale)};
	triangle.Set(triangle_,3);
	obstacles->CreateFixture(&triangle,0.f);
	
	const b2Vec2 diamond_[4] = {b2Vec2(159/scale,(64-18)/scale),b2Vec2(173/scale,(64-33)/scale),b2Vec2(159/scale,(64-47)/scale),b2Vec2(145/scale,(64-33)/scale)};	
	diamond.Set(diamond_,4);
	obstacles->CreateFixture(&diamond,0.f);

	const b2Vec2 pentagon_[5] = {b2Vec2(223/scale,(64-18)/scale),b2Vec2(237/scale,(64-29)/scale),b2Vec2(232/scale,(64-44)/scale),b2Vec2(215/scale,(64-44)/scale),b2Vec2(210/scale,(64-29)/scale)};	
	pentagon.Set(pentagon_,5);
	obstacles->CreateFixture(&pentagon,0.f);

	const b2Vec2 hexagon_[6] = {b2Vec2(287/scale,(64-18)/scale),b2Vec2(300/scale,(64-25)/scale),b2Vec2(300/scale,(64-40)/scale),b2Vec2(287/scale,(64-47)/scale),b2Vec2(275/scale,(64-40)/scale),b2Vec2(275/scale,(64-25)/scale)};	
	hexagon.Set(hexagon_,6);
	obstacles->CreateFixture(&hexagon,0.f);

	
	int number_per_panel = 1;
	double width = (4.0-0.4)/number_per_panel;
	for (int panel = 1; panel < 5; ++panel) {
		for (int ii = 0; ii < number_per_panel; ++ii) {
			b2ParticleGroupDef pd;
			pd.flags = b2_tensileParticle | b2_colorMixingParticle;
			//pd.flags = b2_elasticParticle;
			b2Vec2 shape_[4] = { b2Vec2(width*ii+0.2+panel*4,3.8), b2Vec2(width*ii+0.2+width+panel*4,3.8), b2Vec2(width*ii+0.2+width+panel*4,3.5), b2Vec2(width*ii+0.2+panel*4,3.5) }; 
			b2PolygonShape shape; shape.Set(shape_,4);
			pd.color.Set(randUint(0,255),randUint(0,255),randUint(0,255),255);
			//pd.color.Set(255,0,0,255);
			pd.shape = &shape;
			m_particleSystem->CreateParticleGroup(pd);
		}
	}	
}

void ParticleSim::Sparky() {
	world.SetContactListener(&contactL);
	angle = 0;
	int number_per_panel = 3, bodyCount = 0;
	for (int panel = 1; panel < 5; ++panel) {
		for (int ii = 0; ii < number_per_panel; ++ii) {
			b2BodyDef db;
			db.type = b2_dynamicBody;
			db.position.Set(randFloat(0.5,3.5)+panel*4,randFloat(0.5,3.5));
			dynamicBodies.push_back( world.CreateBody(&db) );
			b2CircleShape circle;
			circle.m_radius = 0.275;
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.density = 0.15f;
			fixtureDef.friction = 0.0f;
			fixtureDef.restitution = 0.6f;
			fixtureDef.userData = (void*)1;
			dynamicBodiesColors.push_back( color_test[randUint(0,9)]  );
			dynamicBodies[bodyCount]->CreateFixture(&fixtureDef);
			dynamicBodies[bodyCount]->SetLinearVelocity(b2Vec2(0,0));
			//dynamicBodies[bodyCount]->SetLinearVelocity(b2Vec2(randFloat(-20,20),randFloat(-20,20)));
			++bodyCount;
		}
	}
}

void ParticleSim::CreateWalls() {
	// Define the ground box shape.
	b2BodyDef wallsDef;
	walls = world.CreateBody(&wallsDef);
	b2PolygonShape top_wall, bottom_wall;
	const b2Vec2 top_wall_[4] = {b2Vec2(0,4),b2Vec2(24,4),b2Vec2(24,3.9),b2Vec2(0,3.9)};
	const b2Vec2 bottom_wall_[4] = {b2Vec2(0,0.1),b2Vec2(24,0.1),b2Vec2(24,0),b2Vec2(0,0)};	
	top_wall.Set(top_wall_,4);
	bottom_wall.Set(bottom_wall_,4);
	walls->CreateFixture(&top_wall,0.f);
	walls->CreateFixture(&bottom_wall,0.f);
	
	for (int ii = 0; ii < 7; ++ii) {
		b2PolygonShape divider;
		double offset = 0.1;
		if (ii == 6) { offset = -0.1; }
		const b2Vec2 divider_[4] = {b2Vec2(4*ii,4),b2Vec2(4*ii+offset,4),b2Vec2(4*ii,0),b2Vec2(4*ii+offset,0)};
		divider.Set(divider_,4);
		walls->CreateFixture(&divider,0.f);
	}
}
	
void ParticleSim::reset()
{
}

float ParticleSim::randFloat(double a, double b) {
	std::uniform_real_distribution<double> dis(a,b);
	return dis(engine);
}

uint32_t ParticleSim::randUint(uint32_t a, uint32_t b) {
	std::uniform_int_distribution<uint32_t> dis(a,b);
	return dis(engine);
}

int ParticleSim::step(uint32_t pixels[], uint32_t clean_color[]) {
	//if (is_water && (gravity[0] > 0 || gravity[1] > 0 || gravity[2] > 0) {
	//	world.SetGravity(b2Vec2(-gravity[0],-gravity[1]));
	if ( fabs(impulse[0]) > 0 || fabs(impulse[1]) > 0 || fabs(impulse[2]) > 0) {
		std::cout << "PARTICLE: Applying impulse of " << impulse[0] << "," << impulse[1] << "," << impulse[2] << std::endl;
		for (auto ii = 0u; ii < dynamicBodies.size(); ++ii) {
			b2Vec2 pos = dynamicBodies[ii]->GetPosition();
			b2Vec2 imp(-impulse[0],impulse[1]);
			if ( (pos.x >= 8 && pos.x <= 12) || (pos.x >= 16 && pos.x <= 20) ) {
				imp.x = impulse[2];
			}
			dynamicBodies[ii]->ApplyLinearImpulse(imp,pos,true);
		}
		impulse[0] = impulse[1] = impulse[2] = 0;
	}

	world.Step(timeStep, velocityIterations, positionIterations, particleIterations);
		
	if (contactL.m_contact) {
		//Explode
		float size = randFloat(0.1,0.25);
		float speed = randFloat(10,20);
		float lifetime = randFloat(0.2,0.3);
		for (auto p : contactL.m_contactPoint) {
			if (m_particleSystem->GetParticleCount() <= 750) {
				vfx.push_back( std::make_shared<ParticleVFX>(m_particleSystem,p,size,speed,lifetime));
			}
		}
		contactL.m_contactPoint.clear();
		contactL.m_contact = false;
	}

	for (int ii = vfx.size()-1; ii >= 0; --ii) {
		vfx[ii]->Step(1./60.);
		if (vfx[ii]->IsDone()) {
			//std::cout << "DEAD! " << vfx[ii]->ParticlesInGroup() << std::endl;
			vfx.erase(vfx.begin()+ii);	
		}
	}

	memcpy(pixels,clean_color,64*64*6*4);

	for (auto ii = 0u; ii < dynamicBodies.size(); ++ii) {
		float r = dynamicBodies[ii]->GetFixtureList()->GetShape()->m_radius;
		float x = dynamicBodies[ii]->GetPosition().x, y = dynamicBodies[ii]->GetPosition().y;
		for (double row = y-r; row <= y+r; row += 0.05) {
			for (double col = x-r; col <= x+r; col += 0.05) {
				if ( (x-col)*(x-col) + (y-row)*(y-row) <= r*r) {
					int loc = (63-( round(row*16)))*64*6 + round(col*16);
					if (loc >= 0 &&  loc < 64*64*6) {
						pixels[loc] = dynamicBodiesColors[ii];
					}
				}
			}
		}	
	}
	
	b2Vec2 *particles = m_particleSystem->GetPositionBuffer();
	b2ParticleColor *colors = m_particleSystem->GetColorBuffer();
	int count = m_particleSystem->GetParticleCount();
	//std::cout << "NUM OF PARTICLES: " << count << std::endl;
	for (int ii = 0; ii < count; ++ii,++particles,++colors) {
		int loc = (63-( round(particles->y*16)))*64*6 + round(particles->x*16);
		if (loc >= 0 &&  loc < 64*64*6) {
			pixels[loc] = colors->r | colors->g << 8 | colors->b << 16;
		}
	}

	for (int ii = 0; ii < 64*64*6; ++ii) {
		if (ii < 64*6 || ii >= 64*63*6 || ii % 64 == 0 || ii % 64 == 63 || (ii%(64*6))/64 == 0 || (ii%(64*6))/64 == 5) {
			pixels[ii] = 255<<16;
		}
	}
	return 0;
}

#ifndef PARTICLEVFX_
#define PARTICLEVFX_

#include <iostream>
#include <Box2D/Box2D.h>

class ParticleVFX {
public:

	// Creates a particle tracker, centered on origin.
	// Size is how big of a particle splash to make (in meters)
	// Speed is the scale of how fast to make the particles shoot out
	// Lifetime in seconds.
	ParticleVFX(b2ParticleSystem *particleSystem, const b2Vec2 &origin,
				float32 size, float32 speed, float32 lifetime)
				//uint32 particleFlags)
	{

		// Create a circle to house the particles of size size
		b2CircleShape shape;
		shape.m_p = origin;
		shape.m_radius = size;

		// Create particle def of random color.
		b2ParticleGroupDef pd;
		pd.flags = b2_powderParticle | b2_colorMixingParticle;
		pd.shape = &shape;
		m_origColor.Set(rand() % 256, rand() % 256, rand() % 256, 255);
		pd.color = m_origColor;
		m_particleSystem = particleSystem;

		// Create a circle full of particles
		m_pg = m_particleSystem->CreateParticleGroup(pd);

		m_initialLifetime = m_remainingLifetime = lifetime;
		m_halfLifetime = m_initialLifetime * 0.5f;

		// Set particle initial velocity based on how far away it is from
		// origin, exploding outwards.
		int32 bufferIndex = m_pg->GetBufferIndex();
		b2Vec2 *pos = m_particleSystem->GetPositionBuffer();
		b2Vec2 *vel = m_particleSystem->GetVelocityBuffer();
		for (int i = bufferIndex; i < bufferIndex + m_pg->GetParticleCount();
			 i++)
		{
			vel[i] = pos[i] - origin;
			vel[i] *= speed;
		}
	}

	~ParticleVFX()
	{
		//std::cout << "Destroying " << m_pg->GetParticleCount() << std::endl;
      		m_pg->DestroyParticles(false);
	}

	// Calculates the brightness of the particles.
	// Piecewise linear function where particle is at 1.0 brightness until
	// t = lifetime/2, then linear falloff until t = 0, scaled by
	// m_halfLifetime.
	float32 ColorCoeff()
	{
		if (m_remainingLifetime >= m_halfLifetime)
		{
			return 1.0f;
		}
		return 1.0f - ((m_halfLifetime - m_remainingLifetime) /
					   m_halfLifetime);
	}

	void Step(float32 dt)
	{
		if (m_remainingLifetime > 0.0f)
		{
          m_remainingLifetime = std::max(m_remainingLifetime - dt, 0.0f);
			float32 coeff = ColorCoeff();

			b2ParticleColor *colors = m_particleSystem->GetColorBuffer();
			int bufferIndex = m_pg->GetBufferIndex();

			// Set particle colors all at once.
			for (int i = bufferIndex;
				 i < bufferIndex + m_pg->GetParticleCount(); i++)
			{
				b2ParticleColor &c = colors[i];
				c *= coeff;
				c.a = m_origColor.a;
			}
		}
	}

	int ParticlesInGroup() {
		return m_pg->GetParticleCount();
	}

	// Are the particles entirely black?
	bool IsDone() { return m_remainingLifetime <= 0.0f; }

private:
	float32 m_initialLifetime;
	float32 m_remainingLifetime;
	float32 m_halfLifetime;
	b2ParticleGroup *m_pg;
	b2ParticleSystem *m_particleSystem;
	b2ParticleColor m_origColor;
};
#endif

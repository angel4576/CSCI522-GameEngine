// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Inter-Engine includes
#include "../Lua/LuaEnvironment.h"

#include "PrimeEngine/Game/Client/ClientGame.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

#include "ParticleSystem.h"
#include "Particle.h"
#include "WindField.h"

#include "iostream"

using namespace PE::Components;
using namespace PE::Events;

namespace PE {
namespace Components {

	PE_IMPLEMENT_CLASS1(ParticleSystem, Component);

	// Handle ParticleSystem::s_hInstance;

	ParticleSystem::ParticleSystem(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, 
		float spawnRate)
		: Component(context, arena, hMyself)
		, m_originPosition(Vector3(0, 0, 0))
		, m_maxParticle(72)
		, m_numParticle(0)
		, m_spawnRate(spawnRate)
		, m_interval(1.0f / m_spawnRate)
		, m_isPullOut(false)
		, m_timeSinceLastEmit(0.0)
	{
		// m_interval = 1.0f / m_spawnRate;
		
		// initialize particle pool
		InitializeParticlePool();

		CreateWindField();

		
	}

	void ParticleSystem::InitializeParticlePool()
	{
		m_particlePool.resize(m_maxParticle);
		for (int i = 0; i < m_maxParticle; ++i)
		{
			// Create a particle
			Particle particle(m_originPosition, Vector3(0, 0, 0), 5.0f);
			m_particlePool.push_back(particle); 
		}
	}

	void ParticleSystem::addDefaultComponents()
	{
		Component::addDefaultComponents();
		
		PE_REGISTER_EVENT_HANDLER(Event_UPDATE, ParticleSystem::do_UPDATE);
		PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, ParticleSystem::do_PRE_RENDER_needsRC);

	}

	void ParticleSystem::addComponent(Handle hComponent, int* pAllowedEvents)
	{
		Component::addComponent(hComponent, pAllowedEvents);
		
		// Add particles to particle system
		/*if (hComponent.getObject<Component>()->isInstanceOf<Particle>())
		{
			m_particles.add(hComponent);
		}*/

	}

	void ParticleSystem::CreateWindField()
	{
		PE::Handle hWindField("WindField", sizeof(WindField));
		WindField* pWindField = new(hWindField) WindField(*m_pContext, m_arena, hWindField);
		pWindField->addDefaultComponents();

		m_windField = pWindField;

		// addComponent(hWindField);
	}

	void ParticleSystem::CreateParticle(PE::GameContext& context, PE::MemoryArena arena)
	{
		
	}

	Vector3 generateRandomDirection()
	{
		float min = -1.0f; float max = 1.0f;

		float randX = min + static_cast<float>(rand()) / RAND_MAX * (max-min);
		float randY = min + static_cast<float>(rand()) / RAND_MAX * (max-min);
		float randZ = min + static_cast<float>(rand()) / RAND_MAX * (max-min);
		
		Vector3 randDir = Vector3(randX, randY, randZ);
		randDir.normalize();
		return randDir;
	}

	// Emit towards Y direction
	Vector3 generateXZRandomDirection(float min, float max)
	{
		// float min = -0.5f; float max = 0.5f;

		float randX = min + static_cast<float>(rand()) / RAND_MAX * (max - min);
		float randZ = min + static_cast<float>(rand()) / RAND_MAX * (max - min);

		Vector3 randDir = Vector3(randX, 1.0f, randZ);
		randDir.normalize();
		return randDir;
	}


	Vector3 generateRandom2DPosition(Vector3 origin, float rangeX, float rangeZ)
	{
		float randPosX = origin.m_x + (static_cast<float>(rand()) / RAND_MAX * 2 - 1) * rangeX;
		float randPosZ = origin.m_z + (static_cast<float>(rand()) / RAND_MAX * 2 - 1) * rangeZ;

		return Vector3(randPosX, origin.m_y, randPosZ); 
	}

	// Generate position 
	Vector3 generateRandomPositionInCircle(Vector3 origin, float radius)
	{
		float randomTheta = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI; // [0, 2*PI]
		float randomR = static_cast<float>(rand()) / RAND_MAX * radius; // [0, radius]

		float x = randomR * cos(randomTheta);
		float z = randomR * sin(randomTheta);

		return Vector3(origin.m_x + x, origin.m_y, origin.m_z + z);
		// return Vector3(origin.m_x + x, origin.m_y + z, origin.m_z);
	}
	

	// Generate direction
	Vector3 generateRandomDirectionInCone(float coneAngle)
	{
		float coneRadian = coneAngle * (M_PI / 180.0f);
		// Offset angle from base direction [0, coneRadian]
		float randPhi = static_cast<float>(rand()) / RAND_MAX * coneRadian;
		// Random angle aroud base direction [0, 2*PI]
		float randTheta = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;

		// Unit sphere coord to xyz (about z)
		/*float x = sin(randPhi) * cos(randTheta);
		float y = sin(randPhi) * sin(randTheta);
		float z = cos(randPhi);*/

		// about y
		float x = sin(randPhi) * cos(randTheta);
		float z = sin(randPhi) * sin(randTheta);
		float y = cos(randPhi);

		Vector3 baseDir = Vector3(0, 1, 0);

		return Vector3(x, y, z); 
		
	}

	

	void ParticleSystem::emitParticles()
	{
		// Create a particle
		Vector3 dir = generateRandomDirectionInCone(20);
		
		float lifetime = 5.0f;

		//Particle particle(m_originPosition, dir, 5.0f);
		//m_qParticles.push_back(particle);
		//// m_particles.push_back(particle);

		// find first invalid particle in pool
		for (Particle& particle : m_particlePool)
		{
			if (!particle.m_isValid)
			{
				// reset parameters of particle in particle pool
				Vector3 spawnPos = generateRandomPositionInCircle(m_originPosition, 0.5f);

				// random particle size
				int minSize = 2; int maxSize = 10;
				int quadSize = minSize +  rand() % (maxSize - minSize + 1);
				// quadSize = 20;

				Vector3 particleColor = particle.m_color;
				Vector3 gravity = Vector3(0, -0.15f, 0);

				particle.reset(spawnPos, dir, gravity, particleColor, 1.0f, quadSize, 18.0f, lifetime);
				particle.activate();
				m_numParticle++;

				break;
			}
		}
		
	}

	void ParticleSystem::renderParticles()
	{
		
	}

	void ParticleSystem::do_UPDATE(PE::Events::Event* pEvt)
	{
		m_timeSinceLastEmit += 0.033f;
		if (m_timeSinceLastEmit >= m_interval && m_numParticle < m_maxParticle && !m_isPullOut)
		{
			emitParticles();

			m_timeSinceLastEmit = 0.0f;
		}

		// Loop particle pool
		for (Particle& particle : m_particlePool)
		{
			if (particle.m_isValid) 
			{
				particle.updateParticle(m_windField); 
				if (particle.m_lifetime <= 0.0f) // dead
				{
					particle.deactivate(); 
					m_numParticle--; 
				}
			}
		}

		std::cout << m_windField->m_windSpeed << std::endl;
		// float windSpeedSq = m_windField->m_windSpeed * m_windField->m_windSpeed;
		if (abs(m_windField->m_windSpeed) >= 160.0f)
		{
			m_isPullOut = true;
		}
		else
		{
			// m_isPullOut = false;
		}

		// old 
		//{
		//	// for all particles 
		//	for (int i = 0; i < m_qParticles.size(); ++i)
		//	{
		//		// if valid -> update position and draw
		//		if (m_qParticles[i].m_isValid)
		//		{
		//			m_qParticles[i].updateParticle();
		//		}

		//	}

		//	// remove dead particles
		//	while (!m_qParticles.empty() &x& !m_qParticles.front().m_isValid)
		//	{
		//		m_qParticles.pop_front();
		//		// m_qParticles.push_back();
		//	}
		//}
	}

	// called every frame 
	void ParticleSystem::do_PRE_RENDER_needsRC(PE::Events::Event* pEvt)
	{
		Event_PRE_RENDER_needsRC *pRealEvent = (Event_PRE_RENDER_needsRC*)(pEvt);
		
		for (Particle& particle : m_particlePool)
		{
			if (particle.m_isValid)
			{
				DebugRenderer::Instance()->createParticleMesh(
					particle.m_color,
					particle.m_size,
					false, false, true, false, 0,
					particle.m_position, 0.01f, pRealEvent->m_threadOwnershipMask);
			}
		}

		/*for (int i = 0; i < m_qParticles.size(); ++i)
		{
			
			DebugRenderer::Instance()->createParticleMesh(
				"", false, false, true, false, 0,
				m_qParticles[i].m_position, 0.01f, pRealEvent->m_threadOwnershipMask);

		}*/

	}


}
}


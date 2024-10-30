// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Inter-Engine includes
#include "../Lua/LuaEnvironment.h"

#include "PrimeEngine/Game/Client/ClientGame.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

#include "ParticleSystem.h"
#include "Particle.h"

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
		, m_maxParticle(64)
		, m_numParticle(0)
		, m_spawnRate(spawnRate)
		, m_interval(1.0f / m_spawnRate)
		, m_timeSinceLastEmit(0.0)
	{
		// m_interval = 1.0f / m_spawnRate;
		
		// initialize particle pool
		InitializeParticlePool();
		
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

	void ParticleSystem::emitParticles()
	{
		// Create a particle
		Vector3 dir = generateRandomDirection(); 
		float lifetime = 5.0f;
		//Particle particle(m_originPosition, dir, 5.0f);
		//m_qParticles.push_back(particle);
		//// m_particles.push_back(particle);

		// find first invalid particle in pool
		for (Particle& particle : m_particlePool)
		{
			if (!particle.m_isValid)
			{
				particle.reset(m_originPosition, dir, lifetime); 
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
		if (m_timeSinceLastEmit >= m_interval && m_numParticle < m_maxParticle)
		{
			emitParticles();

			m_timeSinceLastEmit = 0.0f;
		}

		// loop particle pool
		for (Particle& particle : m_particlePool)
		{
			if (particle.m_isValid) 
			{
				particle.updateParticle(); 
				if (particle.m_lifetime <= 0.0f) // dead
				{
					particle.deactivate(); 
					m_numParticle--; 
				}
			}
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
		//	while (!m_qParticles.empty() && !m_qParticles.front().m_isValid)
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
					"", false, false, true, false, 0,
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


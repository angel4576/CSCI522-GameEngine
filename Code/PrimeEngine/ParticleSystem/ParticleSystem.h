#ifndef __PYENGINE_2_0_PARTICLESYSTEM_H__
#define __PYENGINE_2_0_PARTICLESYSTEM_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"

// Standard
#include "vector"
#include "deque"

#include "Particle.h"

namespace PE {
namespace Components {

	struct ParticleSystem : public Component
	{
		PE_DECLARE_CLASS(ParticleSystem);

		// Singleton 
		//static ParticleSystem* Instance()
		//{
		//	return s_hInstance.getObject<ParticleSystem>();
		//}

		//static void SetInstance(Handle h)
		//{
		//	// Singleton
		//	s_hInstance = h;
		//}

		//static void Construct(PE::GameContext& context, PE::MemoryArena arena);

		// Constructor
		ParticleSystem(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, float spawnRate);

		virtual ~ParticleSystem() {}

		// Events
		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE)
		virtual void do_UPDATE(PE::Events::Event* pEvt);

		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
		void do_PRE_RENDER_needsRC(PE::Events::Event* pEvt);

		// Component
		virtual void addDefaultComponents();
		virtual void addComponent(Handle hComponent, int* pAllowedEvents = NULL);

		// Methods
		void InitializeParticlePool();
		void CreateParticle(PE::GameContext& context, PE::MemoryArena arena);
		void emitParticles();
		void renderParticles();

		// Member variables
		std::deque<Particle> m_qParticles;
		// std::vector<Particle> m_particles;

		Vector3 m_originPosition;

		int m_maxParticle;
		int m_numParticle;
		float m_spawnRate;
		float m_interval; // interval between each spawn

		// Particle pool to manage particles
		std::vector<Particle> m_particlePool;
		int m_numValidParticle = 0;


	private:
		float m_timeSinceLastEmit;


	};
}
}

#endif 
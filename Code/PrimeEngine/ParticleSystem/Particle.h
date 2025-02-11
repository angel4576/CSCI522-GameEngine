#ifndef __PYENGINE_2_0_PARTICLE_H__
#define __PYENGINE_2_0_PARTICLE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

#include "WindField.h"

namespace PE {
namespace Components 
{
	struct Particle
	{
		// PE_DECLARE_CLASS(Particle);

		// Constructors
		Particle() = default;
		Particle(Vector3 position, Vector3 velocity, float lifetime);

		virtual ~Particle() {}

		// Member variables
		Vector3 m_position; // particle world position
		Vector3 m_direction;
		Vector3 m_gravity;

		float m_size;
		float m_lifetime;
		float m_maxLifetime;
		float m_speed;
		bool m_isValid;

		float m_maxSize;
		float m_startSize;
		float m_endSize;

		Vector3 m_color;

		/*PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
		virtual void do_UPDATE(PE::Events::Event* pEvt);*/
		
		// Methods
		void updateParticle(WindField* windField);
		void updatePosition(WindField* windField);
		void updateLifetime(WindField* windField);
		void die();
		void adjustLifetime(WindField* windField);

		void updateSizeOverLifetime();
		void updateColorOverLifetime();
		
		void reset(Vector3 spawnPosition, Vector3 direction, Vector3 gravity, Vector3 color, float speed, float size, float maxSize, float lifetime);
		void activate() { m_isValid = true; }
		void deactivate() { m_isValid = false; }


		// Component
		/*virtual void addDefaultComponents();
		virtual void addComponent();*/

		// Draw
		// void DrawParticle();
	};



}
}




#endif

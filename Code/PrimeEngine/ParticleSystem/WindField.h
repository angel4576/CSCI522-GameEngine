#ifndef __PYENGINE_2_0_WINDFIELD_H__
#define __PYENGINE_2_0_WINDFIELD_H__

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

namespace PE {
namespace Components {

	// WindField is a circle plane; wind direction is same as normal
	struct WindField : public Component
	{
		PE_DECLARE_CLASS(WindField);

		// Constructor
		WindField(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);

		virtual ~WindField() {}

		// Events
		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE)
		virtual void do_UPDATE(PE::Events::Event* pEvt);

		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
		void do_PRE_RENDER_needsRC(PE::Events::Event* pEvt);

		// Component
		virtual void addDefaultComponents();
		virtual void addComponent(Handle hComponent, int* pAllowedEvents = NULL);

		// Method
		Vector3 computeWindNetForce(Vector3 position);
		static Vector3 computeAcceleration(float add);
		static void enhanceWindStrength();
		static void reduceWindStrength();


		public:
			Vector3 m_position;
			Vector3 m_windDirection; // wind direction

			static float m_windSpeed;
			static float m_netForceDirection;
			static float m_windStrength;
			static float m_attractionStrength;
			
			// coefficient
			float m_Clift; 
			float m_Cdown; // resistance
			
			// direction
			Vector3 m_gravity;
			// Vector3 m_windDirection;
			Vector3 m_resistDirection;
			
			// magnitude
			float m_windForce;
			float m_resistForce;

			Vector3 m_gravityAcceleration;
			static Vector3 m_acceleration;

			float m_area; // area facing wind
			float m_rhoAir;



	};


}
}


#endif

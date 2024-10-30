#ifndef __PYENGINE_2_0_PHYSICSMANAGER_H__
#define __PYENGINE_2_0_PHYSICSMANAGER_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"


// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "../Scene/RootSceneNode.h"
#include "../Scene/CameraManager.h"


// Sibling/Children includes

namespace PE {
namespace Components{

struct PhysicsManager : public Component
{
	PE_DECLARE_CLASS(PhysicsManager);

	// Singleton ------------------------------------------------------------------
	static PhysicsManager* Instance() 
	{ 
		return s_hInstance.getObject<PhysicsManager>(); 
	}

	static void SetInstance(Handle h) 
	{ 
		// Singleton
		s_hInstance = h; 
	}

	static void Construct(PE::GameContext &context, PE::MemoryArena arena);

	// Constructor -------------------------------------------------------------
	PhysicsManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);
	virtual ~PhysicsManager(){}
	
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PHYSICS_START);
	virtual void do_PHYSICS_START(PE::Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PHYSICS_END);
	virtual void do_PHYSICS_END(PE::Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(PE::Events::Event* pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
	virtual void do_PRE_RENDER_needsRC(PE::Events::Event* pEvt);

	

	static Handle s_hInstance;
	Array<Handle> m_physicsComponents;

	// Component ------------------------------------------------------------

	virtual void addDefaultComponents();
	virtual void addComponent(Handle hComponent, int* pAllowedEvents = NULL);
	
};

}; // namespace Components
}; // namespace PE
#endif

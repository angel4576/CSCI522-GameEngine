// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "../Lua/LuaEnvironment.h"

// Sibling/Children includes
#include "PhysicsManager.h"
#include "PhysicsComponent.h"

#include "../Sound/SoundManager.h"

using namespace PE::Components;
using namespace PE::Events;

namespace PE {
namespace Components {

// using namespace PE::Events;

PE_IMPLEMENT_CLASS1(PhysicsManager, Component);

// Define static member variables
Handle PhysicsManager::s_hInstance;

// Constructor -------------------------------------------------------------
PhysicsManager::PhysicsManager(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
	: Component(context, arena, hMyself)
	, m_physicsComponents(context, arena, 64) // initialize member class -> call construtor

{
}

// Singleton ------------------------------------------------------------------
void PhysicsManager::Construct(PE::GameContext &context, PE::MemoryArena arena)
{
	Handle handle("PhysicsManager", sizeof(PhysicsManager));
	PhysicsManager *pPhysicsManager = new(handle) PhysicsManager(context, arena, handle);
	pPhysicsManager->addDefaultComponents();

	// Singleton
	SetInstance(handle);

}


void PhysicsManager::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Event_PHYSICS_START, PhysicsManager::do_PHYSICS_START);
	PE_REGISTER_EVENT_HANDLER(Event_PHYSICS_END, PhysicsManager::do_PHYSICS_END);
	PE_REGISTER_EVENT_HANDLER(Event_UPDATE, PhysicsManager::do_UPDATE);
	
	PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, PhysicsManager::do_PRE_RENDER_needsRC);

}

void PhysicsManager::addComponent(Handle hComponent, int* pAllowedEvents)
{
	Component::addComponent(hComponent, pAllowedEvents);

	if (hComponent.getObject<Component>()->isInstanceOf<PhysicsComponent>())
	{
		m_physicsComponents.add(hComponent);
	}

}

// Individual events -------------------------------------------------------
void PhysicsManager::do_PHYSICS_START(PE::Events::Event *pEvt)
{
	for (int i = 0; i < m_physicsComponents.m_size; ++i)
	{
		PhysicsComponent* pPC = m_physicsComponents[i].getObject<PhysicsComponent>();
		for (int j = 0; j < m_physicsComponents.m_size; ++j)
		{
			// get other physics component
			if (pPC->m_isStatic || i == j){ continue; }
			PhysicsComponent* pOhterPC = m_physicsComponents[j].getObject<PhysicsComponent>();
			pPC->OnCollision(pOhterPC);
			
		}
	}

}

void PhysicsManager::do_UPDATE(PE::Events::Event* pEvt)
{
	OutputDebugStringA("Update Test");
}

void PhysicsManager::do_PHYSICS_END(PE::Events::Event* pEvt)
{
	for (int i = 0; i < m_physicsComponents.m_size; ++i)
	{
		PhysicsComponent* pPC = m_physicsComponents[i].getObject<PhysicsComponent>();
		pPC->UpdatePosition();
	}
}

void PhysicsManager::do_PRE_RENDER_needsRC(PE::Events::Event* pEvt)
{
	for (int i = 0; i < m_physicsComponents.m_size; i++) {
		PhysicsComponent* pPC = m_physicsComponents[i].getObject<PhysicsComponent>();
		
		if (pPC->m_isStatic)
			pPC->DrawStaticAABB();
		else
			pPC->DrawAABB();
	}
}



	
}; // namespace Components
}; //namespace PE

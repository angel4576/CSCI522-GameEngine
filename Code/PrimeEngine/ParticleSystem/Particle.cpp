// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Inter-Engine includes

#include "../Lua/LuaEnvironment.h"
#include "Particle.h"
#include "ParticleSystem.h"

using namespace PE::Components;
using namespace PE::Events;

namespace PE {
namespace Components {

	// PE_IMPLEMENT_CLASS1(Particle, Component);

	Particle::Particle(Vector3 spawnPosition, Vector3 direction, float lifetime)
		: m_position (spawnPosition)
		, m_direction (direction)
		, m_lifetime (lifetime)
		, m_isValid (false)
			
	{
		// ParticleSystem::Instance()->addComponent(hMyself); // add particle to particle system
	}

	void Particle::updateParticle()
	{
		updatePosition();

		updateLifetime();
	}

	void Particle::updatePosition()
	{
		m_position += m_direction * 0.033f; // 30 fps

	}

	void Particle::updateLifetime()
	{
		m_lifetime -= 0.033f;

		//if (m_lifetime <= 0.0f)
		//{
		//	//m_isValid = false;
		//	deactivate();
		//
		//}

	}

	void Particle::reset(Vector3 spawnPosition, Vector3 direction, float lifetime)
	{
		m_position = spawnPosition;
		m_direction = direction;
		m_lifetime = lifetime;
	}



	/*void Particle::do_UPDATE(PE::Events::Event* pEvt)
	{

	}*/


	/*void Particle::addDefaultComponents()
	{
		Component::addDefaultComponents();

		PE_REGISTER_EVENT_HANDLER(Event_UPDATE, Particle::do_UPDATE);
	}

	void Particle::addComponent()
	{

	}*/

}
}
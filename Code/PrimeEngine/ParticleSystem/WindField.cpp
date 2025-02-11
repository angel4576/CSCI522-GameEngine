// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Inter-Engine includes
#include "../Lua/LuaEnvironment.h"

#include "PrimeEngine/Game/Client/ClientGame.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

#include "ParticleSystem.h"
#include "Particle.h"

#include "iostream"
#include "WindField.h"

#define DB_PERLIN_IMPL
#include "db_perlin.hpp"

using namespace PE::Events;
using namespace PE::Components;

namespace PE {
namespace Components {

	PE_IMPLEMENT_CLASS1(WindField, Component);
	
	float WindField::m_windSpeed = 5.0f;
	float WindField::m_netForceDirection = 1.0f;
	float WindField::m_windStrength = 0.0f;
	float WindField::m_attractionStrength = 0.0f;
	
	Vector3 WindField::m_acceleration = Vector3(0, 0, 0);
	
	WindField::WindField(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
		: Component(context, arena, hMyself)
		, m_windDirection(Vector3(-1, 0, 0))
		// , m_windSpeed(50)
		, m_Clift(0.013)
		, m_Cdown(0.75)
		, m_resistDirection(Vector3(1, 0, 0))
		// , m_acceleration(Vector3(0, 0, 0))
		, m_area(0.02)
		, m_rhoAir(0.00237)
	{
	}

	void WindField::do_UPDATE(PE::Events::Event* pEvt)
	{

	}

	void WindField::do_PRE_RENDER_needsRC(PE::Events::Event* pEvt)
	{
	}

	void WindField::addDefaultComponents()
	{
		Component::addDefaultComponents();

		PE_REGISTER_EVENT_HANDLER(Event_UPDATE, WindField::do_UPDATE);
		PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, WindField::do_PRE_RENDER_needsRC);
	}

	void WindField::addComponent(Handle hComponent, int* pAllowedEvents)
	{
		Component::addComponent(hComponent, pAllowedEvents); 
	}

	Vector3 WindField::computeWindNetForce(Vector3 position)
	{
		float frequency = 0.7f;
		position *= frequency;
		float noise = db::perlin(position.m_x, position.m_y, position.m_z);

		float mass = 0.1f; // particle mass

		m_windSpeed = 5.0f + m_windStrength * noise; 
		// m_windSpeed = 0.0f + static_cast<float>(rand()) / RAND_MAX * (30.0f - 0.0f);
		
		m_windForce = 0.5f * m_Clift * m_rhoAir * m_windSpeed * m_windSpeed * m_area;
		m_resistForce = 0.5f * m_Cdown * m_rhoAir * m_windSpeed * m_windSpeed * m_area;

		// attraction force
		Vector3 toCenter = m_position - position;
		toCenter.normalize();
		Vector3 attractionForce = toCenter * m_attractionStrength;

		Vector3 windForce = m_windDirection * m_windForce;
		Vector3 resistForce = m_resistDirection * m_resistForce;
		Vector3 netForce = windForce + resistForce /*+ attractionForce*/;
		
		Vector3 acceleration = netForce / mass;
		return acceleration;
	}

	Vector3 WindField::computeAcceleration(float add)
	{
		float mass = 0.1f; // particle mass
		m_windSpeed += add;
		
		Vector3 windForce = Vector3(-1, 0, 0) * 0.5f * 0.013 * 0.00237 * m_windSpeed * m_windSpeed * 0.02;
		Vector3 resistForce = Vector3(1, 0, 0) * 0.5f * 0.75 * 0.00237 * m_windSpeed * m_windSpeed * 0.02;
		// Vector3 resistForce = -m_windDirection * m_resistForce;
		Vector3 netForce = windForce + resistForce;

		Vector3 acceleration = netForce / mass;
		m_acceleration = acceleration;

		return acceleration;
	}

	void WindField::enhanceWindStrength()
	{
		OutputDebugStringA("Add wind\n");
		if (m_windSpeed <= 200.0f) 
		{
			m_windStrength += 2.0f;
			// m_attractionStrength += 0.0001f;

		}

		/*if (m_windSpeed >= 0)
		{
			m_netForceDirection = 1.0f;
		}*/

		// std::cout << m_windStrength << std::endl;


	}

	void WindField::reduceWindStrength()
	{
		OutputDebugStringA("Reduce wind\n");
		if (m_windSpeed >= 0)
		{
			m_windStrength -= 2.0f;
			// m_attractionStrength -= 0.0001f;
		}

		/*if (m_windSpeed <= 0)
		{
			m_netForceDirection = -1.0f;
		}*/

		// std::cout << m_windStrength << std::endl;

	}



}
}

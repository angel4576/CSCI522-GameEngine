// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Inter-Engine includes

#include "../Lua/LuaEnvironment.h"
#include "Particle.h"
#include "ParticleSystem.h"

#include "iostream"

using namespace PE::Components;
using namespace PE::Events;

namespace PE {
namespace Components {

	// PE_IMPLEMENT_CLASS1(Particle, Component);

	Particle::Particle(Vector3 spawnPosition, Vector3 direction, float lifetime)
		: m_position (spawnPosition)
		, m_direction (direction)
		, m_gravity(Vector3(0, 0, 0))
		, m_size(1.0f)
		, m_lifetime (lifetime)
		, m_maxLifetime(lifetime)
		, m_speed(1.0f)
		, m_isValid (false)
		, m_startSize(1.0f)
		, m_endSize(1.0f)
		, m_color(Vector3(1, 1, 1))
			
	{
	}

	void Particle::updateParticle(WindField* windField)
	{
		updatePosition(windField);
		updateLifetime(windField);

		updateSizeOverLifetime(); 
		updateColorOverLifetime();
	}

	void Particle::updatePosition(WindField* windField)
	{
		// Vector3 speed = windField->m_windSpeed;
		Vector3 acceleration = windField->computeWindNetForce(m_position);
		Vector3 velocity = acceleration  * 0.033f;
		// Vector3 velocity = WindField::m_acceleration * 0.033f;

		m_direction += velocity * WindField::m_netForceDirection;

		m_position += m_direction * 0.033f; // 30 fps

		// std::cout << windField->m_windSpeed << std::endl;
		/*if (windSpeedSq >= 3000.0f)
		{
			deactivate();
		}
		else
		{
			activate();
		}*/


	}

	void Particle::updateLifetime(WindField* windField)
	{
		// m_lifetime -= 0.033f;
		adjustLifetime(windField);

	}

	void Particle::die()
	{
		m_lifetime = m_maxLifetime;
	}

	void Particle::adjustLifetime(WindField* windField)
	{
		float lifetimeFactor = 1.0f + (windField->m_windSpeed / 200.0f /*windmax*/);
		float adjustDeltaTime = 0.033f * lifetimeFactor;
		m_lifetime -= adjustDeltaTime;

		if (m_lifetime <= 0.0f)
		{
			m_lifetime = 0.0f;
		}
	
	}

	Vector3 linearColorInterpolation(float t, float maxTime, Vector3 startColor, Vector3 endColor)
	{
		t = t / maxTime;
		float r = startColor.m_x * t + endColor.m_x * (1 - t);
		float g = startColor.m_y * t + endColor.m_y * (1 - t);
		float b = startColor.m_z * t + endColor.m_z * (1 - t);

		return Vector3(r, g, b);
	}

	Vector3 nonLinearInterpolation(float t, float maxTime, Vector3 startColor, Vector3 endColor)
	{
		float progress = 1.0f - (t / maxTime);
		//float factor = progress * progress; // quad
		//factor = progress * progress * (3.0f - 2.0f * progress); // 三次
		float factor = pow(progress, 0.4f);

		return (1.0f - factor) * startColor + factor * endColor;
	}

	float linearInterpolation(float t, float start, float end)
	{
		return start * t + end * (1 - t);
	}

	// Todo: implement a better non-linear function for size change
	float expAttenuation(float t, float maxLifetime, float startSize)
	{
		float lifetimeRatio = t / maxLifetime;
		float baseSize = startSize * lifetimeRatio;
		float flameSize = baseSize * exp(-1.0f * (1.0f - lifetimeRatio));

		float wave = 0.05f * sin(10.0f * (1.0f - lifetimeRatio) * M_PI);

		return flameSize + wave;
	}

	float normalDistribution(float age, float life)
	{
		//float ratio = age / life;
		//if (ratio > 0.5f)
		//{
		//	float tNormalized = (ratio - 0.5f) * 2.0f; // 映射到 [0, 1]
		//	return 1.0f * (1.0f - pow(1.0f - tNormalized, 1.5));
		//}
		//else
		//{
		//	float tNormalized = ratio * 2.0f; // 映射到 [0, 1]
		//	return 0.8f * pow(tNormalized, 1.2f); // 缓和减小 
		//}
		
		float factor = 1.0 / (pow(age - life * 0.75f, 2) + 1.0);
		return factor;
	}

	void Particle::updateSizeOverLifetime()
	{
		// m_size = linearInterpolation(m_lifetime, m_startSize, m_endSize);
		// m_size = expAttenuation(m_lifetime, m_maxLifetime, m_startSize);
		m_size = m_startSize * normalDistribution(m_lifetime, m_maxLifetime);
	}

	void Particle::updateColorOverLifetime()
	{
		// Vector3 startColor = Vector3(1, 0.47, 0);
		Vector3 startColor = Vector3(1.0, 0.8, 0.0);
		Vector3 endColor = Vector3(0.6, 0.0, 0.0);
		//m_color = linearColorInterpolation(m_lifetime, m_maxLifetime, startColor, endColor); 
		m_color = nonLinearInterpolation(m_lifetime, m_maxLifetime, startColor, endColor);
		
	}

	void Particle::reset(Vector3 spawnPosition, Vector3 direction, Vector3 gravity, Vector3 color, float speed, float size, float maxSize, float lifetime)
	{
		m_position = spawnPosition;
		m_gravity = gravity;
		m_direction = direction + gravity;

		m_speed = speed;
		m_size = size;

		m_lifetime = lifetime;
		m_maxLifetime = lifetime;

		m_startSize = maxSize;
		m_endSize = 1.0f;

		m_color = color;
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
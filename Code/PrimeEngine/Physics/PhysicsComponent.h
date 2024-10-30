#ifndef __PYENGINE_2_0_PHYSICSCOMPONENT_H__
#define __PYENGINE_2_0_PHYSICSCOMPONENT_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"


// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"

#include "../Scene/DebugRenderer.h"


namespace PE {
namespace Components {

	struct PhysicsComponent : public Component
	{
		PE_DECLARE_CLASS(PhysicsComponent);

		// Constructor
		PhysicsComponent(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself,
			Handle hMeshInst, Handle hSceneNode, Vector3 minAABB, Vector3 maxAABB, bool isStatic);

		virtual ~PhysicsComponent() {}

		virtual void addDefaultComponent();

		// Methods
		void OnCollision(PhysicsComponent* other);
		void CheckSphereBoxCollision(PhysicsComponent* other);
		Vector3 FindNearestBoxPoint(PhysicsComponent *other);

		void UpdatePosition();

		// Debug draw
		void DrawAABB();
		void DrawStaticAABB();


		// Member variables
		Handle m_hMeshInst;
		Handle m_hSN; // handle to scene node

		Matrix4x4 m_modelToWorld;
		Vector3 m_aabb[2];
		Vector3 m_blockDir;

		Vector3* m_boundPts;

		float m_radius;
		bool m_isOnGround;
		bool m_isStatic;


	};

};
};


#endif

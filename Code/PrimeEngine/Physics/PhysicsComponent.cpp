// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Outer-Engine includes

// Inter-Engine includes
#include "../Lua/LuaEnvironment.h"

#include "../Scene/MeshInstance.h"
#include "../Scene/Mesh.h"

#include "PhysicsComponent.h"
#include "PhysicsManager.h"

using namespace PE::Components;
using namespace PE::Events;

namespace PE {
namespace Components {

	PE_IMPLEMENT_CLASS1(PhysicsComponent, Component);

	PhysicsComponent::PhysicsComponent(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, 
		Handle hMeshInst, Handle hSceneNode, Vector3 minAABB, Vector3 maxAABB, bool isStatic)
		: Component(context, arena, hMyself)
		, m_hMeshInst(hMeshInst)
		, m_hSN(hSceneNode)
	{
		SceneNode* pSN = m_hSN.getObject<SceneNode>();
		m_modelToWorld = pSN->m_worldTransform * pSN->m_base;

		// Get mesh
		/*MeshInstance* pMeshInst = m_hMeshInst.getObject<MeshInstance>();
		Mesh* pMesh = pMeshInst->m_hAsset.getObject<Mesh>();*/
		
		//m_aabb[0] = pMesh->m_bbx[0] + m_modelToWorld.getPos(); // min bound
		//m_aabb[1] = pMesh->m_bbx[1] + m_modelToWorld.getPos(); // max bound

		m_aabb[0] = minAABB + m_modelToWorld.getPos();
		m_aabb[1] = maxAABB + m_modelToWorld.getPos();

		/*m_aabb[0] = minAABB;
		m_aabb[1] = maxAABB;*/


		m_radius = (m_aabb[1] - m_aabb[0]).length() / 4;
		m_blockDir = Vector3(0, 0, 0);

		m_isStatic = isStatic;

		PhysicsManager::Instance()->addComponent(hMyself);
	}

	void PhysicsComponent::addDefaultComponent()
	{
		Component::addDefaultComponents();

		
	}
	
	Vector3 PhysicsComponent::FindNearestBoxPoint(PhysicsComponent* other)
	{
		Vector3 center = m_modelToWorld.getPos(); // sphere center
		Vector3 min_bbx = other->m_aabb[0];
		Vector3 max_bbx = other->m_aabb[1];

		Vector3 nearestPt = center;
		
		// if center is outside aabb, nearest pt to sphere is on the surface of aabb
		// if center is inside aabb, nearst pt is the center
		/*nearestPt.m_x = center.m_x < min_bbx.m_x ? min_bbx.m_x : center.m_x;
		nearestPt.m_x = center.m_x > max_bbx.m_x ? max_bbx.m_x : center.m_x;
 
		nearestPt.m_y = center.m_y < min_bbx.m_y ? min_bbx.m_y : center.m_y;
		nearestPt.m_y = center.m_y > max_bbx.m_y ? max_bbx.m_y : center.m_y;

		nearestPt.m_z = center.m_z < min_bbx.m_z ? min_bbx.m_z : center.m_z;
		nearestPt.m_z = center.m_z > max_bbx.m_z ? max_bbx.m_z : center.m_z;*/

		nearestPt.m_x = max(min_bbx.m_x, min(max_bbx.m_x, center.m_x));
		nearestPt.m_y = max(min_bbx.m_y, min(max_bbx.m_y, center.m_y));
		nearestPt.m_z = max(min_bbx.m_z, min(max_bbx.m_z, center.m_z));

		return nearestPt;

	}

	void PhysicsComponent::CheckSphereBoxCollision(PhysicsComponent *other)
	{
		Vector3 nearestPt = FindNearestBoxPoint(other);
		Vector3 blockDir = nearestPt - m_modelToWorld.getPos();

		// float dist = blockDir.length();
		float sqDist = blockDir.lengthSqr();
		if (sqDist < m_radius * m_radius) // collide
		{
			if (other->m_aabb[1].m_y <= m_modelToWorld.getPos().m_y) // other pc is under this pc
			{
				m_isOnGround = true;
				return;
			}
			// OutputDebugStringA("Collide with car!\n");
			if(sqDist != 0)
				blockDir.normalize();
			m_blockDir += Vector3(blockDir.m_x, 0, blockDir.m_z);
		}
		
	}

	void PhysicsComponent::OnCollision(PhysicsComponent *other)
	{	
		if (!m_isStatic && other->m_isStatic)
		{
			CheckSphereBoxCollision(other);

		}

	}

	void PhysicsComponent::UpdatePosition()
	{
		m_blockDir = Vector3(0, 0, 0);
		if (!m_isStatic)
		{
			SceneNode* pSN = m_hSN.getObject<SceneNode>();
			m_modelToWorld = pSN->m_worldTransform * pSN->m_base;
			m_isOnGround = false; // set to false every update
		}

	}

	void PhysicsComponent::DrawStaticAABB()
	{
		Vector3 min_bbx = m_aabb[0]; // only apply translation
		Vector3 max_bbx = m_aabb[1];


		float pMin[] = { min_bbx.m_x, min_bbx.m_y, min_bbx.m_z };
		float pMax[] = { max_bbx.m_x, max_bbx.m_y, max_bbx.m_z };
		// DebugRenderer::Instance()->createAABB(pMin, pMax);
		DebugRenderer::Instance()->myCreateAABB(min_bbx, max_bbx, m_modelToWorld);
		// DebugRenderer::Instance()->createOBB(m_boundPts, m_modelToWorld);
	}

	void PhysicsComponent::DrawAABB()
	{
		Vector3 min_bbx = m_aabb[0] + m_modelToWorld.getPos(); // only apply translation
		Vector3 max_bbx = m_aabb[1] + m_modelToWorld.getPos();

		/*Vector3 min_bbx = m_modelToWorld * m_aabb[0];
		Vector3 max_bbx = m_modelToWorld * m_aabb[1];*/

		float pMin[] = { min_bbx.m_x, min_bbx.m_y, min_bbx.m_z };
		float pMax[] = { max_bbx.m_x, max_bbx.m_y, max_bbx.m_z};
		// DebugRenderer::Instance()->createOBB(boundPts, m_modelToWorld);
		DebugRenderer::Instance()->myCreateAABB(min_bbx, max_bbx, m_modelToWorld);
	}
		
};
};
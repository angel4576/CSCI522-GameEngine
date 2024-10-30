#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "DefaultAnimationSM.h"
#include "Light.h"

#include "PrimeEngine/GameObjectModel/Camera.h"

// Sibling/Children includes
#include "MeshInstance.h"
#include "MeshManager.h"
#include "SceneNode.h"
#include "CameraManager.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "DebugRenderer.h"


namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(MeshInstance, Component);

MeshInstance::MeshInstance(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
, m_culledOut(false)
{
}

MeshInstance::MeshInstance(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, SceneNode* parentSN)
: Component(context, arena, hMyself)
, m_culledOut(false)
, m_sceneNode(parentSN)
{
	

}

void MeshInstance::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(PE::Events::Event_PRE_RENDER_needsRC, MeshInstance::do_PRE_RENDER_needsRC);
}

void MeshInstance::initFromFile(const char *assetName, const char *assetPackage,
		int &threadOwnershipMask)
{
	Handle h = m_pContext->getMeshManager()->getAsset(assetName, assetPackage, threadOwnershipMask);

	initFromRegisteredAsset(h);
}

bool MeshInstance::hasSkinWeights()
{
	Mesh *pMesh = m_hAsset.getObject<Mesh>();
	return pMesh->m_hSkinWeightsCPU.isValid();
}

void MeshInstance::initFromRegisteredAsset(const PE::Handle &h)
{
	m_hAsset = h;
	//add this instance as child to Mesh so that skin knows what to draw
	static int allowedEvts[] = {0};
	m_hAsset.getObject<Component>()->addComponent(m_hMyself, &allowedEvts[0]);
}

void MeshInstance::do_PRE_RENDER_needsRC(PE::Events::Event* pEvt)
{
	Mesh* pMesh = m_hAsset.getObject<Mesh>();

	if (m_sceneNode != NULL)
	{
		Vector3 min_bbx = m_sceneNode->m_worldTransform * pMesh->m_bbx[0];
		Vector3 max_bbx = m_sceneNode->m_worldTransform * pMesh->m_bbx[1];

		float pMin[] = { min_bbx.m_x, min_bbx.m_y, min_bbx.m_z };
		float pMax[] = { max_bbx.m_x, max_bbx.m_y, max_bbx.m_z };
		DebugRenderer::Instance()->createAABB(pMin, pMax);
		/*pMin = m_sceneNode->m_worldTransform * pMin;
		pMax = m_sceneNode->m_worldTransform * pMax;*/
	}

	//PE::Events::Event_PRE_RENDER_needsRC* pRealEvent = (PE::Events::Event_PRE_RENDER_needsRC*)(pEvt);

	//Mesh* pMesh = m_hAsset.getObject<Mesh>();
	//
	//Vector3 pMin = pMesh->m_bbx[0];
	//Vector3 pMax = pMesh->m_bbx[1];
	//Vector3 boundPts[8] =
	//{
	//	Vector3(pMax.m_x,pMin.m_y,pMax.m_z), 
	//	Vector3(pMax.m_x,pMin.m_y,pMin.m_z),
	//	Vector3(pMin.m_x,pMin.m_y,pMin.m_z),
	//	Vector3(pMin.m_x,pMin.m_y,pMax.m_z),

	//	Vector3(pMax.m_x,pMax.m_y,pMax.m_z),
	//	Vector3(pMax.m_x,pMax.m_y,pMin.m_z),
	//	Vector3(pMin.m_x,pMax.m_y,pMin.m_z),
	//	Vector3(pMin.m_x,pMax.m_y,pMax.m_z),
	//};

	//if (m_sceneNode != NULL)
	//{

	//		Vector3 l_color(1.0f, 1.0f, 0);

	//		// Draw down face
	//		int cur = 0; int next = cur + 1;
	//		for (int i = 0; i < 4; ++i)
	//		{
	//			cur = i; next = cur + 1;
	//			if (cur == 3)
	//			{
	//				next = 0;
	//			}
	//			Vector3 l_target = m_sceneNode->m_worldTransform.getPos() + boundPts[next];
	//			Vector3 l_pos = m_sceneNode->m_worldTransform.getPos() + boundPts[cur];
	//			Vector3 linepts[] = { l_pos, l_color, l_target, l_color };
	//			DebugRenderer::Instance()->createLineMesh(true, m_sceneNode->m_worldTransform, &linepts[0].m_x, 2, 0);
	//		}

	//		// Draw up face
	//		for (int i = 4; i < 8; ++i)
	//		{
	//			cur = i; next = cur + 1;
	//			if (cur == 7)
	//			{
	//				next = 4;
	//			}
	//			Vector3 l_target = m_sceneNode->m_worldTransform.getPos() + boundPts[next];
	//			Vector3 l_pos = m_sceneNode->m_worldTransform.getPos() + boundPts[cur];
	//			Vector3 linepts[] = { l_pos, l_color, l_target, l_color };
	//			DebugRenderer::Instance()->createLineMesh(true, m_sceneNode->m_worldTransform, &linepts[0].m_x, 2, 0);
	//		}

	//		// Draw vertical lines
	//		for (int i = 0; i < 4; ++i)
	//		{
	//			cur = i; next = i + 4;
	//			Vector3 l_target = m_sceneNode->m_worldTransform.getPos() + boundPts[next];
	//			Vector3 l_pos = m_sceneNode->m_worldTransform.getPos() + boundPts[cur];
	//			Vector3 linepts[] = { l_pos, l_color, l_target, l_color };
	//			DebugRenderer::Instance()->createLineMesh(true, m_sceneNode->m_worldTransform, &linepts[0].m_x, 2, 0);
	//		}
	//	

	//}
}

}; // namespace Components
}; // namespace PE

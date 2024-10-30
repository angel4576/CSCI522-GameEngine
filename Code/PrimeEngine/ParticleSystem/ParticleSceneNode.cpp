#include "../Lua/LuaEnvironment.h"
// #include "PrimeEngine/Scene/TextMesh.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Scene/MeshManager.h"
#include "PrimeEngine/Scene/MeshInstance.h"

#include "ParticleSceneNode.h"
#include "ParticleMesh.h"

namespace PE {
namespace Components {

	PE_IMPLEMENT_CLASS1(ParticleSceneNode, SceneNode);


	// Constructor -------------------------------------------------------------
	ParticleSceneNode::ParticleSceneNode(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
		: SceneNode(context, arena, hMyself)
	{
		m_cachedAspectRatio = 1.0f;
		m_scale = 1.0f;
		if (IRenderer* pS = context.getGPUScreen())
			m_cachedAspectRatio = float(pS->getWidth()) / float(pS->getHeight());
	}

	void ParticleSceneNode::addDefaultComponents()
	{
		SceneNode::addDefaultComponents();


		// event handlers
		PE_REGISTER_EVENT_HANDLER(Events::Event_PRE_GATHER_DRAWCALLS, ParticleSceneNode::do_PRE_GATHER_DRAWCALLS);
	}

	void ParticleSceneNode::setSelfAndMeshAssetEnabled(bool enabled)
	{
		setEnabled(enabled);

		if (m_hParticleMesh.isValid())
		{
			m_hParticleMesh.getObject<Component>()->setEnabled(enabled);
		}
	}


	void ParticleSceneNode::loadFromString_needsRC(const char* str, DrawType drawType, int& threadOwnershipMask)
	{
		m_drawType = drawType;

		ParticleMesh* pParticleMesh = NULL;
		if (m_hParticleMesh.isValid())
		{
			pParticleMesh = m_hParticleMesh.getObject<ParticleMesh>();
		}
		else
		{
			// create a particle mesh 
			m_hParticleMesh = PE::Handle("PARTICLEMESH", sizeof(ParticleMesh));
			pParticleMesh = new(m_hParticleMesh) ParticleMesh(*m_pContext, m_arena, m_hParticleMesh);
			pParticleMesh->addDefaultComponents();

			m_pContext->getMeshManager()->registerAsset(m_hParticleMesh);

			m_hParticleMeshInstance = PE::Handle("MeshInstance", sizeof(MeshInstance));
			MeshInstance* pInstance = new(m_hParticleMeshInstance) MeshInstance(*m_pContext, m_arena, m_hParticleMeshInstance);
			pInstance->addDefaultComponents();
			pInstance->initFromRegisteredAsset(m_hParticleMesh);


			addComponent(m_hParticleMeshInstance);
		}

		PE::IRenderer::checkForErrors("");

		const char* tech = 0;
		if (drawType == Overlay2D_3DPos || drawType == Overlay2D)
			tech = "StdMesh_2D_Diffuse_A_RGBIntensity_Tech";
		if (drawType == InWorldFacingCamera)
			tech = "StdMesh_Diffuse_Tech";

		pParticleMesh->loadFromString_needsRC(str, tech, threadOwnershipMask);
	}

	void ParticleSceneNode::do_PRE_GATHER_DRAWCALLS(Events::Event* pEvt)
	{
		Events::Event_PRE_GATHER_DRAWCALLS* pDrawEvent = NULL;
		pDrawEvent = (Events::Event_PRE_GATHER_DRAWCALLS*)(pEvt);

		Matrix4x4 projectionViewWorldMatrix = pDrawEvent->m_projectionViewTransform;
		Matrix4x4 worldMatrix;

		if (!m_hParticleMeshInstance.isValid())
			return;

		ParticleMesh* pParticleMesh = m_hParticleMesh.getObject<ParticleMesh>();

		if (m_drawType == InWorldFacingCamera)
		{
			m_worldTransform.turnTo(pDrawEvent->m_eyePos);
		}

		float numCharsInFullLine = 100.0f;
		numCharsInFullLine *= .5; // need to divide by 2.0 since screen goes from -1 to 1, not 0..1

		if (m_drawType == Overlay2D_3DPos)
		{
			worldMatrix = m_worldTransform;

			projectionViewWorldMatrix = projectionViewWorldMatrix * worldMatrix;

			Vector3 pos(0, 0, 0);
			pos = projectionViewWorldMatrix * pos;

			if (pos.m_x < -1.0f || pos.m_x > 1.0f || pos.m_z <= 0.0f || pos.m_z > 1.0f)
			{
				// this will cancel further event handling by this mesh only
				pEvt->m_cancelSiblingAndChildEventHandling = true;
				return;
			}

			float factor = 1.0f / (numCharsInFullLine);// * m_scale;
			Matrix4x4 scale;
			scale.importScale(factor, factor * m_cachedAspectRatio, 1.f);

			m_worldTransform.loadIdentity();
			m_worldTransform.setPos(Vector3(pos.m_x - factor * pParticleMesh->m_textLength * .5f, pos.m_y, 0));
			m_worldTransform = m_worldTransform * scale;
		}
		if (m_drawType == Overlay2D)
		{
			worldMatrix = m_worldTransform;

			float factor = 1.0f / (numCharsInFullLine)*m_scale;

			Matrix4x4 scale;
			scale.importScale(factor, factor * m_cachedAspectRatio, 1.f);
			m_worldTransform = worldMatrix * scale;
		}


	}

}; // namespace Components
}; // namespace PE

#ifndef __PYENGINE_2_0_PARTICLESCENENODE_H__
#define __PYENGINE_2_0_PARTICLESCENENODE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "PrimeEngine/Scene/SceneNode.h"

//#define USE_DRAW_COMPONENT

namespace PE {
namespace Components {
	struct ParticleSceneNode : public SceneNode
	{
		PE_DECLARE_CLASS(ParticleSceneNode);

		// Constructor -------------------------------------------------------------
		ParticleSceneNode(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);

		virtual ~ParticleSceneNode() {}

		void setSelfAndMeshAssetEnabled(bool enabled);

		// Component ------------------------------------------------------------

		virtual void addDefaultComponents();

		// Individual events -------------------------------------------------------

		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_GATHER_DRAWCALLS);
		virtual void do_PRE_GATHER_DRAWCALLS(Events::Event* pEvt);

		enum DrawType
		{
			InWorld,
			InWorldFacingCamera,
			Overlay2D,
			Overlay2D_3DPos
		};
		void loadFromString_needsRC(const char* str, DrawType drawType, int& threadOwnershipMask);

		DrawType m_drawType;
		float m_scale;
		/*Handle m_hMyTextMesh;
		Handle m_hMyTextMeshInstance;*/

		Handle m_hParticleMesh;
		Handle m_hParticleMeshInstance;
		float m_cachedAspectRatio;

		bool m_canBeRecreated;

	}; // class ParticleSceneNode

}; // namespace Components
}; // namespace PE
#endif

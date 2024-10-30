#ifndef __pe_meshinstance_h__
#define __pe_meshinstance_h__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes
#include "Mesh.h"
#include "SceneNode.h"

#include "CharacterControl/Events/Events.h"

namespace PE {
namespace Components {

struct MeshInstance : public Component
{
	PE_DECLARE_CLASS(MeshInstance);


	// Constructor -------------------------------------------------------------
	MeshInstance(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) ;

	MeshInstance(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, SceneNode* parentSN);

	void initFromFile(const char *assetName, const char *assetPackage,
		int &threadOwnershipMask);

	void initFromRegisteredAsset(const PE::Handle &h);

	virtual ~MeshInstance(){}

	virtual void addDefaultComponents();

	bool hasSkinWeights();
	
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
	void do_PRE_RENDER_needsRC(PE::Events::Event* pEvt);

    bool m_culledOut;
	Handle m_hAsset;

	int m_skinDebugVertexId;

	SceneNode* m_sceneNode;
};

}; // namespace Components
}; // namespace PE
#endif

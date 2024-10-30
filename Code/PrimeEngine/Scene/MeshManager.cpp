// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "MeshManager.h"
// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"

#include "PrimeEngine/Scene/RootSceneNode.h"

#include "Light.h"

// Sibling/Children includes

#include "MeshInstance.h"
#include "Skeleton.h"
#include "SceneNode.h"
#include "DrawList.h"
#include "SH_DRAW.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "DebugRenderer.h"

namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(MeshManager, Component);
MeshManager::MeshManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
	: Component(context, arena, hMyself)
	, m_assets(context, arena, 256)
{
}

PE::Handle MeshManager::getAsset(const char *asset, const char *package, int &threadOwnershipMask)
{
	char key[StrTPair<Handle>::StrSize];
	sprintf(key, "%s/%s", package, asset);
	
	int index = m_assets.findIndex(key);
	if (index != -1)
	{
		return m_assets.m_pairs[index].m_value;
	}
	Handle h;

	if (StringOps::endswith(asset, "skela"))
	{
		PE::Handle hSkeleton("Skeleton", sizeof(Skeleton));
		Skeleton *pSkeleton = new(hSkeleton) Skeleton(*m_pContext, m_arena, hSkeleton);
		pSkeleton->addDefaultComponents();

		pSkeleton->initFromFiles(asset, package, threadOwnershipMask);
		h = hSkeleton;
	}
	else if (StringOps::endswith(asset, "mesha")) // load mesh
	{
		MeshCPU mcpu(*m_pContext, m_arena);
		mcpu.ReadMesh(asset, package, "");
		
		PE::Handle hMesh("Mesh", sizeof(Mesh));
		Mesh *pMesh = new(hMesh) Mesh(*m_pContext, m_arena, hMesh);
		pMesh->addDefaultComponents();

		pMesh->loadFromMeshCPU_needsRC(mcpu, threadOwnershipMask);

#if PE_API_IS_D3D11
		// todo: work out how lods will work
		//scpu.buildLod();
#endif
        // generate collision volume here. or you could generate it in MeshCPU::ReadMesh()
		PositionBufferCPU* pPoss = pMesh->m_hPositionBufferCPU.getObject<PositionBufferCPU>();

		static float fVertexIndex = 0;
		static float fInc = 0.5f;
		fVertexIndex += fInc;

		static int iVertexIndex = 1150;
		int vertexIndex = iVertexIndex >= 0 ? iVertexIndex : int(fVertexIndex);

		vertexIndex = vertexIndex % (pPoss->m_values.m_size / 3);

		// HW3: Check each vertex to find min and max sbound
		int i = 0;
		Vector3 pos(pPoss->m_values[i * 3], pPoss->m_values[i * 3 + 1], pPoss->m_values[i * 3 + 2]);
		
		float minX = pos.getX(); float minY = pos.getY(); float minZ = pos.getZ();
		float maxX = pos.getX(); float maxY = pos.getY(); float maxZ = pos.getZ();
		
		for (int i = 0; i < pPoss->m_values.m_size / 3; ++i)
		{
			Vector3 newPos(pPoss->m_values[i * 3], pPoss->m_values[i * 3 + 1], pPoss->m_values[i * 3 + 2]);
			if (newPos.getX() < minX)
			{
				minX = newPos.getX();
			}
			else if (newPos.getX() > maxX)
			{
				maxX = newPos.getX();
			}

			if (newPos.getY() < minY)
			{
				minY = newPos.getY();
			}
			else if (newPos.getY() > maxY)
			{
				maxY = newPos.getY();
			}

			if (newPos.getZ() < minZ)
			{
				minZ = newPos.getZ();
			}
			else if (newPos.getZ() > maxZ)
			{
				maxZ = newPos.getZ();
			}
		}

		pMesh->m_bbx[0] = Vector3(minX, minY, minZ);
		pMesh->m_bbx[1] = Vector3(maxX, maxY, maxZ);

		
	


        pMesh->m_performBoundingVolumeCulling = true; // will now perform tests for this mesh

		h = hMesh;
	}


	PEASSERT(h.isValid(), "Something must need to be loaded here");

	RootSceneNode::Instance()->addComponent(h);
	m_assets.add(key, h);
	return h;
}

void MeshManager::registerAsset(const PE::Handle &h)
{
	static int uniqueId = 0;
	++uniqueId;
	char key[StrTPair<Handle>::StrSize];
	sprintf(key, "__generated_%d", uniqueId);
	
	int index = m_assets.findIndex(key);
	PEASSERT(index == -1, "Generated meshes have to be unique");
	
	RootSceneNode::Instance()->addComponent(h);
	m_assets.add(key, h);
}

}; // namespace Components
}; // namespace PE

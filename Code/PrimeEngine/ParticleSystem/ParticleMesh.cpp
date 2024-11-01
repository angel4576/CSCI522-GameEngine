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
#include "PrimeEngine/Scene/Light.h"
#include "PrimeEngine/GameObjectModel/Camera.h"
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Scene/CameraManager.h"

// Sibling/Children includes
#include "ParticleMesh.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
namespace PE {
namespace Components {

	PE_IMPLEMENT_CLASS1(ParticleMesh, Mesh);

	void ParticleMesh::addDefaultComponents()
	{
		//add this handler before Mesh's handlers so we can intercept draw and modify transform
		PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS, ParticleMesh::do_GATHER_DRAWCALLS);
		Mesh::addDefaultComponents();
	}

	void ParticleMesh::loadFromString_needsRC(const char* str, const char* techName, int& threadOwnershipMask)
	{
		// int len = StringOps::length(str);

		if (!m_meshCPU.isValid())
		{
			m_meshCPU = Handle("MeshCPU TextMesh", sizeof(MeshCPU));
			new (m_meshCPU) MeshCPU(*m_pContext, m_arena);
		}
		MeshCPU& mcpu = *m_meshCPU.getObject<MeshCPU>();

		if (!m_loaded)
			mcpu.createBillboardMeshWithColorTexture("white.dds", "Default", 16, 16, SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp);

		// this will cause not using the vertex buffer manager
		//so that engine always creates a new vertex buffer gpu and doesn't try to find and
		//existing one
		mcpu.m_manualBufferManagement = true;

		PositionBufferCPU* pVB = mcpu.m_hPositionBufferCPU.getObject<PositionBufferCPU>();
		TexCoordBufferCPU* pTCB = mcpu.m_hTexCoordBufferCPU.getObject<TexCoordBufferCPU>();
		NormalBufferCPU* pNB = mcpu.m_hNormalBufferCPU.getObject<NormalBufferCPU>();
		IndexBufferCPU* pIB = mcpu.m_hIndexBufferCPU.getObject<IndexBufferCPU>();
		
		// for a quad
		pVB->m_values.reset(4 * 3); // 4 verts * (x,y,z)
		pTCB->m_values.reset(4 * 2);
		pNB->m_values.reset(4 * 3);
		pIB->m_values.reset(6); // 2 tris
		// 6 vertics for 2 tris; 4 vertics for a quad
		pIB->m_indexRanges[0].m_start = 0;
		pIB->m_indexRanges[0].m_end = 5;
		pIB->m_indexRanges[0].m_minVertIndex = 0;
		pIB->m_indexRanges[0].m_maxVertIndex = 3;

		pIB->m_minVertexIndex = pIB->m_indexRanges[0].m_minVertIndex;
		pIB->m_maxVertexIndex = pIB->m_indexRanges[0].m_maxVertIndex;

		/*float w = 32.f / 2.0f;
		float h = 32.f;
		h = h / w;
		w = 1.0f;*/

		float quadSize = 0.5f;
		float halfSize = quadSize * 0.5f;

		/*m_charW = w;
		m_charH = h;
		m_charWAbs = fabs(w);
		m_textLength = (float)(len);
		float curX = 0;
		float curY = 0;
		float pixSize2 = 1.0f / 512.0f / 2.0f;*/
		
		// Defind quad vertex (clockwise)
		pVB->m_values.add(-halfSize, halfSize, 0); // top left
		pVB->m_values.add(halfSize, halfSize, 0);
		pVB->m_values.add(halfSize, -halfSize, 0); // bottom right
		pVB->m_values.add(-halfSize, -halfSize, 0);

		pIB->m_values.add(0, 1, 2);
		pIB->m_values.add(2, 3, 0);

		// UV
		pTCB->m_values.add(0, 1); // top left
		pTCB->m_values.add(1, 1); 
		pTCB->m_values.add(1, 0);
		pTCB->m_values.add(0, 0); // bottom left 

		pNB->m_values.add(0, 0, 1);
		pNB->m_values.add(0, 0, 1);
		pNB->m_values.add(0, 0, 1);
		pNB->m_values.add(0, 0, 1);
		

		if (!m_loaded)
		{
			// first time creating gpu mesh
			loadFromMeshCPU_needsRC(mcpu, threadOwnershipMask);

			if (techName)
			{
				Handle hEffect = EffectManager::Instance()->getEffectHandle(techName);

				for (unsigned int imat = 0; imat < m_effects.m_size; imat++)
				{
					if (m_effects[imat].m_size)
						m_effects[imat][0] = hEffect;
				}
			}
			m_loaded = true;
		}
		else
		{
			//just need to update vertex buffers gpu
			updateGeoFromMeshCPU_needsRC(mcpu, threadOwnershipMask);
		}
	}

	void ParticleMesh::do_GATHER_DRAWCALLS(Events::Event* pEvt)
	{

	}


}; // namespace Components
}; // namespace PE

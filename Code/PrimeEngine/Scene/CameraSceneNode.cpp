#include "CameraSceneNode.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "MeshInstance.h"
#include "DebugRenderer.h"

#define Z_ONLY_CAM_BIAS 0.0f
namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(CameraSceneNode, SceneNode);

CameraSceneNode::CameraSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : SceneNode(context, arena, hMyself)
{
	m_near = 0.05f;
	m_far = 2000.0f;
}
void CameraSceneNode::addDefaultComponents()
{
	Component::addDefaultComponents();
	PE_REGISTER_EVENT_HANDLER(Events::Event_CALCULATE_TRANSFORMATIONS, CameraSceneNode::do_CALCULATE_TRANSFORMATIONS);

	PE_REGISTER_EVENT_HANDLER(PE::Events::Event_PRE_RENDER_needsRC, CameraSceneNode::do_PRE_RENDER_needsRC);
}

void CameraSceneNode::do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt)
{
	Handle hParentSN = getFirstParentByType<SceneNode>();
	if (hParentSN.isValid())
	{
		Matrix4x4 parentTransform = hParentSN.getObject<PE::Components::SceneNode>()->m_worldTransform;
		m_worldTransform = parentTransform * m_base;
	}
	
	Matrix4x4 &mref_worldTransform = m_worldTransform;

	Vector3 pos = Vector3(mref_worldTransform.m[0][3], mref_worldTransform.m[1][3], mref_worldTransform.m[2][3]);
	Vector3 n = Vector3(mref_worldTransform.m[0][2], mref_worldTransform.m[1][2], mref_worldTransform.m[2][2]);
	Vector3 target = pos + n;
	Vector3 up = Vector3(mref_worldTransform.m[0][1], mref_worldTransform.m[1][1], mref_worldTransform.m[2][1]);

	m_worldToViewTransform = CameraOps::CreateViewMatrix(pos, target, up);

	m_worldTransform2 = mref_worldTransform;

	m_worldTransform2.moveForward(Z_ONLY_CAM_BIAS);

	Vector3 pos2 = Vector3(m_worldTransform2.m[0][3], m_worldTransform2.m[1][3], m_worldTransform2.m[2][3]);
	Vector3 n2 = Vector3(m_worldTransform2.m[0][2], m_worldTransform2.m[1][2], m_worldTransform2.m[2][2]);
	Vector3 target2 = pos2 + n2;
	Vector3 up2 = Vector3(m_worldTransform2.m[0][1], m_worldTransform2.m[1][1], m_worldTransform2.m[2][1]);

	m_worldToViewTransform2 = CameraOps::CreateViewMatrix(pos2, target2, up2);
    
    PrimitiveTypes::Float32 aspect = (PrimitiveTypes::Float32)(m_pContext->getGPUScreen()->getWidth()) / (PrimitiveTypes::Float32)(m_pContext->getGPUScreen()->getHeight());
    
    PrimitiveTypes::Float32 verticalFov = 0.33f * PrimitiveTypes::Constants::c_Pi_F32;
    if (aspect < 1.0f)
    {
        //ios portrait view
        static PrimitiveTypes::Float32 factor = 0.5f;
        verticalFov *= factor;
    }

	m_viewToProjectedTransform = CameraOps::CreateProjectionMatrix(verticalFov, 
		aspect,
		m_near, m_far);
	
	// HW3: frustum calculation
	Matrix4x4 worldToProjectedTransform = m_viewToProjectedTransform * m_worldToViewTransform;// P * V

	// Generate frustum plane information
	Vector3 cameraFront = pos - target; // point to camera (negative camera forward)
	cameraFront.normalize();

	Vector3 cameraForward = n;
	cameraForward.normalize();

	Vector3 cameraUp = up;
	cameraUp.normalize();

	// Vector3 cameraRight = Vector3(mref_worldTransform.m[0][0], mref_worldTransform.m[1][0], mref_worldTransform.m[2][0]);
	Vector3 cameraRight = -cameraForward.crossProduct(cameraUp); // left-handed coordinate
	cameraRight.normalize();

	float horizontalFov = 2 * atanf(tanf(verticalFov * 0.5f) * aspect);

	// Near plane
	// Vector3 nearCenter = pos - m_near * cameraFront;
	Vector3 nearCenter = pos + m_near * cameraForward;
	// m_frustum.m_nearFace = {cameraFront, -nearCenter.dotProduct(cameraFront)};
	m_frustum.m_nearFace = { -cameraForward, nearCenter };

	// Far plane
	Vector3 farCenter = pos + m_far * cameraForward;
	m_frustum.m_farFace = { cameraForward, farCenter};

	// Right plane
	float nearHS_H = tanf(horizontalFov * 0.5f) * m_near; // near face half size (horizontal
	Vector3 nearRightPt = nearCenter + cameraRight * nearHS_H; // point on the edge of left & near plane
	Vector3 rightNormal = (nearRightPt - pos).crossProduct(-cameraUp);
	m_frustum.m_rightFace = { rightNormal, nearRightPt }; // Right plane

	// Left Plane
	Vector3 nearLeftPt = nearCenter - cameraRight * nearHS_H; // point on the edge of left & near plane
	Vector3 leftNormal = (nearLeftPt - pos/*camera point to left edge of near plane*/).crossProduct(cameraUp); 
	m_frustum.m_leftFace = { leftNormal, nearLeftPt };
	
	// Bottom Plane
	float nearHS_V = tanf(verticalFov * 0.5f) * m_near;
	Vector3 nearBtmPt = nearCenter - cameraUp * nearHS_V;
	Vector3 btmNormal = (nearBtmPt - pos).crossProduct(-cameraRight);
	m_frustum.m_bottomFace = { btmNormal, nearBtmPt };

	// Top Plane
	Vector3 nearTopPt = nearCenter + cameraUp * nearHS_V;
	Vector3 topNormal = (nearTopPt - pos).crossProduct(cameraRight);
	m_frustum.m_topFace = { topNormal, nearTopPt };

	SceneNode::do_CALCULATE_TRANSFORMATIONS(pEvt);

}

bool CameraSceneNode::isWithinPlane(Plane plane, Vector3 min_bbx, Vector3 max_bbx)
{
	Vector3 boundPts[8] =
	{
		Vector3(min_bbx.m_x,min_bbx.m_y,min_bbx.m_z),
		Vector3(max_bbx.m_x,min_bbx.m_y,min_bbx.m_z),
		Vector3(min_bbx.m_x,min_bbx.m_y,max_bbx.m_z),
		Vector3(max_bbx.m_x,min_bbx.m_y,max_bbx.m_z),
		Vector3(min_bbx.m_x,max_bbx.m_y,min_bbx.m_z),
		Vector3(max_bbx.m_x,max_bbx.m_y,min_bbx.m_z),
		Vector3(min_bbx.m_x,max_bbx.m_y,max_bbx.m_z),
		Vector3(max_bbx.m_x,max_bbx.m_y,max_bbx.m_z),
	};

	for (int i = 0; i < 8; ++i)
	{
		plane.normal.normalize();
		// float dist = plane.normal.dotProduct(boundPts[i]) + plane.D;
		float dProduct = plane.normal.dotProduct(boundPts[i] - plane.point);

		if (dProduct > 0)
		{
			return false;
		}
	}

	return true;
}

bool CameraSceneNode::isAABBInFrustum(Vector3 min_bbx, Vector3 max_bbx)
{
	return isWithinPlane(m_frustum.m_nearFace, min_bbx, max_bbx) &&
			isWithinPlane(m_frustum.m_farFace, min_bbx, max_bbx) &&
			isWithinPlane(m_frustum.m_leftFace, min_bbx, max_bbx) &&
			isWithinPlane(m_frustum.m_rightFace, min_bbx, max_bbx) &&
			isWithinPlane(m_frustum.m_bottomFace, min_bbx, max_bbx) &&
			isWithinPlane(m_frustum.m_topFace, min_bbx, max_bbx);
}

void CameraSceneNode::do_PRE_RENDER_needsRC(PE::Events::Event* pEvt)
{
	/*PE::Events::Event_PRE_RENDER_needsRC* pRealEvent = (PE::Events::Event_PRE_RENDER_needsRC*)(pEvt);

	Vector3 l_color(1.0f, 1.0f, 1.0f);
	Vector3 l_target = m_worldToViewTransform.getN() + Vector3(0,0,0);
	Vector3 l_pos = Vector3(0, 0, 0);
	Vector3 linepts[] = { l_pos, l_color, l_target, l_color };
	DebugRenderer::Instance()->createLineMesh(true, m_worldTransform, &linepts[0].m_x, 2, 0);*/
}

}; // namespace Components
}; // namespace PE

#ifndef __PYENGINE_2_0_CAMERA_SCENE_NODE_H__
#define __PYENGINE_2_0_CAMERA_SCENE_NODE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "PrimeEngine/Math/CameraOps.h"

#include "SceneNode.h"


// Sibling/Children includes

namespace PE {
namespace Components {

struct Plane
{
	Vector3 normal = { 0, 0.1f, 0 };
	// float D = 0;
	Vector3 point = { 0, 0, 0 };
};

struct Frustum
{
	Plane m_leftFace;
	Plane m_rightFace;
	Plane m_bottomFace;
	Plane m_topFace;
	Plane m_nearFace;
	Plane m_farFace;
};

struct CameraSceneNode : public SceneNode
{

	PE_DECLARE_CLASS(CameraSceneNode);

	// Constructor -------------------------------------------------------------
	CameraSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual ~CameraSceneNode(){}

	// Component ------------------------------------------------------------
	virtual void addDefaultComponents();

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CALCULATE_TRANSFORMATIONS);
	virtual void do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
	void do_PRE_RENDER_needsRC(PE::Events::Event* pEvt);

	bool CameraSceneNode::isWithinPlane(Plane plane, Vector3 min_bbx, Vector3 max_bbx);
	bool CameraSceneNode::isAABBInFrustum(Vector3 min_bbx, Vector3 max_bbx);


	// Individual events -------------------------------------------------------
	
	Matrix4x4 m_worldToViewTransform; // objects in world space are multiplied by this to get them into camera's coordinate system (view space)
	Matrix4x4 m_worldToViewTransform2;
	Matrix4x4 m_worldTransform2;
	Matrix4x4 m_viewToProjectedTransform; // objects in local (view) space are multiplied by this to get them to screen space
	float m_near, m_far;

	float m_leftPlane[4];
	float m_rightPlane[4];
	float m_bottomPlane[4];
	float m_topPlane[4];
	float m_nearPlane[4];
	float m_farPlane[4];

	Frustum m_frustum;

};
}; // namespace Components
}; // namespace PE
#endif

#pragma once

#include <Renderer/Colours.h>

// This interface should be implemented in each API
// Only responsibility of this class is visualization of primitives:
// *  Point
// *  Line
// *  AABB-box
// *  Frustum
//
// This goal should be achieved as efficiently as possible.
// There could be be multiple instances of this class throughout the application
// e.g. one for debug purposes, one for editors purposes etc.
namespace GLEngine::Physics::Primitives {
struct S_AABB;
}

namespace GLEngine::Renderer {

namespace Animation {
struct S_Joint;
class C_Skeleton;
} // namespace Animation

class I_DebugDraw {
public:
	virtual void DrawPoint(const glm::vec3& point, const Colours::T_Colour& color = Colours::black, const glm::mat4& modelMatrix = glm::mat4(1.0f))					= 0;
	virtual void DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const Colours::T_Colour& color = Colours::black)										= 0;
	virtual void DrawLine(const glm::vec3& pointA, const glm::vec3& pointB, const Colours::T_Colour& colorA, const Colours::T_Colour& colorB)						= 0;
	virtual void DrawLines(const std::vector<glm::vec4>& pairs, const Colours::T_Colour& color = Colours::black)													= 0;
	virtual void DrawAABB(const Physics::Primitives::S_AABB& bbox, const Colours::T_Colour& color = Colours::black, const glm::mat4& modelMatrix = glm::mat4(1.0f)) = 0;
	virtual void DrawAxis(const glm::vec3& origin, const glm::vec3& up, const glm::vec3& foreward, const glm::mat4& modelMatrix = glm::mat4(1.0f))					= 0;

	virtual void DrawBone(const glm::vec3& position, const Animation::S_Joint& joint)		= 0;
	virtual void DrawSkeleton(const glm::vec3& root, const Animation::C_Skeleton& skeleton) = 0;
};
} // namespace GLEngine::Renderer

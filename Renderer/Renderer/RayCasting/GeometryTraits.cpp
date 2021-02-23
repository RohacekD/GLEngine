#include <RendererStdAfx.h>

#include <Renderer/RayCasting/Frame.h>
#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/GeometryTraits.h>
#include <Renderer/RayCasting/Sampling.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Shapes.h>
#include <Physics/Primitives/Triangle.h >

namespace GLEngine::Renderer::RayTracing {

//=================================================================================
float T_GeometryTraits::GetArea(const Physics::Primitives::S_Triangle& triangle)
{
	return triangle.GetArea();
}

//=================================================================================
float T_GeometryTraits::GetArea(const Physics::Primitives::S_Sphere&)
{
	return std::numeric_limits<float>::infinity();
}

//=================================================================================
float T_GeometryTraits::GetArea(const Physics::Primitives::S_Disc& disc)
{
	return disc.radius * disc.radius;
}

//=================================================================================
float T_GeometryTraits::GetArea(const Physics::Primitives::S_Plane&)
{
	return std::numeric_limits<float>::infinity();
}

//=================================================================================
glm::vec3 T_GeometryTraits::SamplePoint(const Physics::Primitives::S_Disc& disc, I_Sampler* rnd)
{
	const auto ligthRadius = disc.radius;
	const auto samplePoint = ligthRadius * SampleConcentricDisc(rnd->GetV2());
	const auto lightFrame  = S_Frame(disc.plane.normal);
	return disc.plane.origin + samplePoint.x * lightFrame.Tangnt() + samplePoint.y * lightFrame.Bitangent();
}

//=================================================================================
glm::vec3 T_GeometryTraits::SamplePoint(const Physics::Primitives::S_Triangle& triangle, I_Sampler* rnd)
{
	auto areaX = rnd->GetD();
	auto areaY = rnd->GetD();

	if (areaX + areaY >= 1.f)
	{
		areaX = 1 - areaX;
		areaY = 1 - areaY;
	}

	const auto e1 = triangle.m_p[1] - triangle.m_p[0];
	const auto e2 = triangle.m_p[2] - triangle.m_p[0];

	return triangle.m_p[0] + (areaX * e1) + (areaY * e2);
}

} // namespace GLEngine::Renderer::RayTracing

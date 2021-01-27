#pragma once

namespace GLEngine::Renderer
{
struct S_Frame;

class I_ReflectionModel {
public:
	virtual ~I_ReflectionModel() = default;

	virtual glm::vec3 f(const glm::vec3& wi, const glm::vec3& wo) const = 0;
	virtual glm::vec3 SampleF(const glm::vec3& wi, glm::vec3& wo, const S_Frame& frame, const glm::vec2& rng, float* pdf) const = 0;
	virtual float Pdf(const glm::vec3& wi, const glm::vec3& wo) const = 0;

protected:
	static float FresnelDieletrics(float cosThetaI, float etaI, float etaO);
};
}
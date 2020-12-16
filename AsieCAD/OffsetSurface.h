#pragma once
#include "surface.h"
#include "torus.h"

class OffsetSurface : public Surface
{
	std::shared_ptr<Surface> surface;
	float distance;
public:
	OffsetSurface(std::shared_ptr<Surface> _surface, float _distance) :
		surface(_surface), distance(_distance) {}
	glm::vec3 GetPointAt(float u, float v) override
	{
		auto tangent = surface->GetTangentAt(u, v);
		if (!glm::isnan(tangent.normal.x))
			return tangent.pos + distance * tangent.normal;
		else
			return tangent.pos;
	}
	TngSpace GetTangentAt(float u, float v) override
	{
		if (std::dynamic_pointer_cast<Torus>(surface)) {
			glm::vec3 diffU[2], diffV[2], antiNormal;
			TngSpace result;
			result.pos = GetPointAt(u, v);
			antiNormal = GetPointAt(u, v + 0.5f);
			diffU[0] = GetPointAt(u + 0.25f, v);
			diffU[1] = GetPointAt(u - 0.25f, v);
			diffV[0] = GetPointAt(u, v + 0.25f);
			diffV[1] = GetPointAt(u, v - 0.25f);
			result.diffU = (diffU[1] - diffU[0]) * (float)-M_PI;
			result.diffV = (diffV[1] - diffV[0]) * (float)-M_PI;
			result.normal = glm::normalize(glm::cross(result.diffV, result.diffU));
			return result;			
		}
		float d = 0.000001;
		auto tangent = surface->GetTangentAt(u, v);
		auto tangentDU = surface->GetTangentAt(u + d, v);
		auto tangentDV = surface->GetTangentAt(u, v + d);
		if (!glm::isnan(tangent.normal.x)) {
			tangent.pos += tangent.normal * distance;
			tangent.diffU = (tangentDU.pos + tangentDU.normal * distance - tangent.pos) / d;
			tangent.diffV = (tangentDV.pos + tangentDV.normal * distance - tangent.pos) / d;
		}
		return tangent;
	}
	bool RollU() override { return surface->RollU(); }
	bool RollV() override { return surface->RollV(); }
};

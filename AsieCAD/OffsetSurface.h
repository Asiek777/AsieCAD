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
		if (std::dynamic_pointer_cast<Torus>(surface) != nullptr) 
			if (v < 0.5f) {
				tangent = surface->GetTangentAt(u, 0.5f);
				tangent.pos.z *= std::max(v / 0.5f, 0.f);
			}
		
		
		if (!glm::isnan(tangent.normal.x))
			return tangent.pos + distance * tangent.normal;
		else
			return tangent.pos;
	}
	TngSpace GetTangentAt(float u, float v) override
	{
		float d = 0.000001;
		auto tangent = surface->GetTangentAt(u, v);
		auto tangentDU = surface->GetTangentAt(u + d, v);
		auto tangentDV = surface->GetTangentAt(u, v + d);
		if (std::dynamic_pointer_cast<Torus>(surface) != nullptr) {			
			if (v < 0.5f) {
				tangent = surface->GetTangentAt(u, 0.5f);
				tangent.pos.z *= std::max(v / 0.5f, 0.f);
			}
			if (v < 0.5f) {
				tangentDU = surface->GetTangentAt(u + d, 0.5f);
				tangentDU.pos.z *= std::max(v / 0.5f, 0.f);
			}
			if (v + d < 0.5f) {
				tangentDV = surface->GetTangentAt(u, 0.5f);
				tangentDV.pos.z *= (v + d) / 0.5f;
			}			
		}
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

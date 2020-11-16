#pragma once
#include "surface.h"

class InboxSurface : public Surface
{
	std::shared_ptr<Surface> surface;
	float distance;
public:
	InboxSurface(std::shared_ptr<Surface> _surface, float _distance) :
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
		auto tangent = surface->GetTangentAt(u, v);
		if (!glm::isnan(tangent.normal.x))
			tangent.pos += tangent.normal * distance;
		return tangent;
	}
	bool RollU() override { return surface->RollU(); }
	bool RollV() override { return surface->RollV(); }
};
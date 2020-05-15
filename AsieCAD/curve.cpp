#include "curve.h"

#include "point.h"

bool Curve::AddPoint(std::shared_ptr<SceneObject>& point)
{
	if (!point->IsPoint())
		return false;
	points.emplace_back(ControlPoint{ std::weak_ptr<SceneObject>(point), false });
	hasChanged = true;
	return true;
}

void Curve::RenderMenu()
{
	ImGui::Checkbox("Render broken line", &drawBroken);
	PointObject::RenderMenu();
}

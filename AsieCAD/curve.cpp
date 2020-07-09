#include "curve.h"

#include "point.h"

bool Curve::AddPoint(std::shared_ptr<SceneObject> point)
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

void Curve::Serialize(std::string type, tinyxml2::XMLElement* scene)
{
	auto ptr = scene->InsertNewChildElement(type.c_str());
	ptr->SetAttribute("Name", name.c_str());
	ptr->SetAttribute("ShowControlPolygon", drawBroken);
	auto pointRefs = ptr->InsertNewChildElement("Points");
	for (int i = 0; i < points.size(); i++) {
		auto pRef = pointRefs->InsertNewChildElement("PointRef");
		pRef->SetAttribute("Name", points[i].point.lock()->name.c_str());
	}
}

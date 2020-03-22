#include "bezierCurve.h"
#include <functional>

int BezierCurve::SelectedCount()
{
	int result = 0;
	for (int i = 0; i < points.size(); i++)
		if (points[i].isSelected)
			result++;
	return result;
}
void BezierCurve::clearExpired()
{
	auto end = std::remove_if(points.begin(), points.end(),
		[](auto const& object) { return object.point.expired(); });
	points.erase(end, points.end());
}
bool BezierCurve::AddPoint(std::shared_ptr<SceneObject> &point)
{
	if (!point->IsPoint())
		return false;
	points.emplace_back(CurvePoint{ std::weak_ptr<SceneObject>(point), false });
}
void BezierCurve::RenderMenu()
{
	ImGui::ListBoxHeader("");
	for (int i = 0; i < points.size(); i++) {
		std::shared_ptr<SceneObject> point = points[i].point.lock();
		if (ImGui::Selectable(
		(std::to_string(i) + ": " + point->name).c_str(),
			points[i].isSelected)) {
			if (ImGui::GetIO().KeyCtrl) {
				points[i].isSelected = !points[i].isSelected;
				int selectedCount = SelectedCount();
				if (selectedCount > 1) {
					glm::vec3 centerLoc = glm::vec3(0);
					for (int j = 0; j < points.size(); j++)
						if (points[j].isSelected) {
							auto point = points[j].point.lock();
							centerLoc += point->GetCenter();
						}
					selectedCenter = Position(centerLoc / (float)selectedCount);
				}
			}
			else {
				for (int j = 0; j < points.size(); j++)
					points[j].isSelected = false;
				points[i].isSelected = true;
			}
		}
	}
	ImGui::ListBoxFooter();
	int selectedCount = SelectedCount();
	if (selectedCount > 0) {
		if (ImGui::Button(selectedCount > 1 ? "Delete points" : "Delete point")) {
			auto end = std::remove_if(points.begin(), points.end(),
				[](auto const& object) { return object.isSelected; });
			points.erase(end, points.end());
		}
		else if (selectedCount == 1) {
			int i;
			for (i = 0; !points[i].isSelected; i++);
			std::shared_ptr<SceneObject> point(points[i].point);
			if (ImGui::CollapsingHeader("Point Properties"))
				point->RenderFullMenu();			
		}
		else {			
			if (selectedCenter.RenderMenu(GetCursorCenter()))
				for (int i = 0; i < points.size(); i++)
					if (points[i].isSelected) {
						auto point = points[i].point.lock();
						point->UpdatePosition(selectedCenter.newPos(point->GetCenter()),
							selectedCenter.scaleChange, selectedCenter.rotationChange);
					}
		}
	}

}
void BezierCurve::Render()
{
	clearExpired();
}
glm::vec3 BezierCurve::GetCenter()
{
	if (SelectedCount() > 1)
		return selectedCenter.location;
	glm::vec3 center = glm::vec3(0);
	if (points.size() == 0)
		return center;
	for (int i = 0; i < points.size(); i++) {
		auto point = points[i].point.lock();
		center += point->GetCenter();
	}
	return center / (float)points.size();
}

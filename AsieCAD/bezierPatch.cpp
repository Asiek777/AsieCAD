#include "bezierPatch.h"


int BezierPatch::Number = 0;
int BezierPatch::PatchCount[2] = { 1,1 };
float BezierPatch::PointDistance[2] = { 1,1 };

BezierPatch::BezierPatch(std::vector<std::shared_ptr<Point>> _points) :
	PointObject(("Bezier patch " +	std::to_string(Number)).c_str())
{
	for (int i = 0; i < _points.size(); i++)
		points.emplace_back(
			ControlPoint{ std::weak_ptr<SceneObject>(_points[i]), false });
	isEditable = false;
	Number++;
}

BezierPatch::~BezierPatch()
{
	for (int i = 0; i < points.size(); i++)
		static_cast<Point*> (points[i].point.lock().get())->SetDeletability(true);
}

void BezierPatch::Render()
{
	if (isSelected)
		RenderSelectedPoints();
}

void BezierPatch::RenderCreationMenu()
{
	if (ImGui::CollapsingHeader("Bezier patch creation")) {
		ImGui::DragInt2("Patches count", PatchCount, 1, 1, 10);
		ImGui::DragFloat2("Patches size", PointDistance,0.005, 0);
		if(ImGui::Button("Add patch")) {
			int size[2] = { PatchCount[0] * 3 + 1, PatchCount[1] * 3 + 1 };
			std::vector<std::shared_ptr<Point>>	points(size[0] * size[1]);
			glm::vec3 begin = GetCursorCenter();
			for (int i = 0; i < size[0]; i++)
				for (int j = 0; j < size[1]; j++)
					points[i * size[1] + j] = std::make_shared<Point>(begin +
						glm::vec3(PointDistance[0] * i, 0, PointDistance[1] * j));
			SceneObjects.emplace_back(std::make_shared<BezierPatch>(points));
			for (int i = 0; i < points.size(); i++) {
				points[i]->SetDeletability(false);
				SceneObjects.push_back(points[i]);
			}
		}
	}
}
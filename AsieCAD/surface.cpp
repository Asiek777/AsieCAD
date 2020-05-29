#include "surface.h"

#include "bezierPatch.h"


int Surface::PatchCount[2] = { 1,1 };
float Surface::PatchSize[2] = { 3,3 };
float Surface::CylinderLength = 1;
float Surface::CylinderRadius = 3;

Surface::Surface(const char* _name) : PointObject(_name), curveCount{ 4,4 }
{
	patchCount[0] = PatchCount[0];
	patchCount[1] = PatchCount[1];
	isEditable = false;	
}


void Surface::RenderCreationMenu()
{
	if (ImGui::CollapsingHeader("Bezier patch creation")) {
		ImGui::DragInt2("Patches count", PatchCount, 1, 1, 20);
		ImGui::DragFloat2("Patches patchCount", PatchSize, 0.02, 0);
		ImGui::DragFloat("Cylinder length", &CylinderLength, 0.02, 0);
		ImGui::DragFloat("Cylinder radius", &CylinderRadius, 0.001, 0);

		if (ImGui::Button("Add patch")) {
			int size[2] = { PatchCount[0] * 3 + 1, PatchCount[1] * 3 + 1 };
			auto points = PrepareFlatVertices(size);
			SceneObjects.emplace_back(std::make_shared<BezierPatch>(points, false));
		}
		ImGui::SameLine();
		if (ImGui::Button("Add cylinder patch")) {
			int size[2] = { PatchCount[0] * 3 + 1, PatchCount[1] * 3 };
			auto points = PrepareRoundVertices(size);
			SceneObjects.emplace_back(std::make_shared<BezierPatch>(points, true));
		}
	}
}

std::vector<std::shared_ptr<Point>> Surface::PrepareFlatVertices(int size[2])
{
	std::vector<std::shared_ptr<Point>>	points(size[0] * size[1]);
	glm::vec3 begin = GetCursorCenter();
	glm::vec2 PointDistance(PatchSize[0] / (size[0] - 1), PatchSize[1] / (size[1] - 1));
	for (int i = 0; i < size[0]; i++)
		for (int j = 0; j < size[1]; j++)
			points[i * size[1] + j] = std::make_shared<Point>(begin +
				glm::vec3(PointDistance.x * i, 0, PointDistance.y * j));
	for (int i = 0; i < points.size(); i++) {
		points[i]->SetDeletability(false);
		SceneObjects.push_back(points[i]);
	}
	return  points;
}

std::vector<std::shared_ptr<Point>> Surface::PrepareRoundVertices(int size[2])
{
	std::vector<std::shared_ptr<Point>>	points(size[0] * size[1]);
	glm::vec3 begin = GetCursorCenter();
	for (int i = 0; i < size[0]; i++)
		for (int j = 0; j < size[1]; j++) {
			float angle = j * glm::two_pi<float>() / size[1];
			glm::vec3 offset = glm::vec3(i * CylinderLength / size[0],
				std::sinf(angle) * CylinderRadius, std::cosf(angle) * CylinderRadius);
			points[i * size[1] + j] = std::make_shared<Point>(begin + offset);
		}
	for (int i = 0; i < points.size(); i++) {
		points[i]->SetDeletability(false);
		SceneObjects.push_back(points[i]);
	}
	return  points;
}
#include "surface.h"

#include "bezierPatch.h"
#include "splinePatch.h"


int Surface::PatchCount[2] = { 1,1 };
float Surface::PatchSize[2] = { 3,3 };
float Surface::CylinderLength = 1;
float Surface::CylinderRadius = 3;
std::unique_ptr<Shader> Surface::meshShader;

Surface::Surface(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder, const char* _name) :
	PointObject(_name), curveCount{ 4,4 }
{
	if (!meshShader)
		meshShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	patchCount[0] = PatchCount[0];
	patchCount[1] = PatchCount[1];
	isEditable = false;
	for (int i = 0; i < _points.size(); i++)
		points.emplace_back(
			ControlPoint{ std::weak_ptr<SceneObject>(_points[i]), false });
	isCylinder = _isCylinder;
}

Surface::~Surface()
{
	for (int i = 0; i < points.size(); i++)
		static_cast<Point*> (points[i].point.lock().get())->SetDeletability(true);
}

void Surface::RenderMesh(int pointCount[2])
{
	std::vector<glm::vec3> knots(points.size());
	std::vector<unsigned> indices;
	for (int i = 0; i < points.size(); i++)
		knots[i] = points[i].point.lock()->GetCenter();
	for (int i = 0; i < pointCount[0]; i++)
		for (int j = 0; j < pointCount[1]; j++) {
			if (i < pointCount[0] - 1) {
				indices.emplace_back(i * pointCount[1] + j);
				indices.emplace_back((i + 1) * pointCount[1] + j);
			}
			if (isCylinder || j < pointCount[1] - 1) {
				indices.emplace_back(i * pointCount[1] + j);
				indices.emplace_back(i * pointCount[1] + (j + 1) % pointCount[1]);
			}
		};
	auto mesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(knots), 1, indices);
	meshShader->use();
	meshShader->setMat4("model", glm::mat4(1.0f));
	meshShader->setMat4("viewProjection", viewProjection);
	meshShader->setVec3("color", COLORS::BROKEN);
	glBindVertexArray(mesh->GetVAO());
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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
		if (ImGui::Button("Add spline patch")) {
			int size[2] = { PatchCount[0] + 3, PatchCount[1] + 3 };
			auto points = PrepareFlatVertices(size);
			SceneObjects.emplace_back(std::make_shared<SplinePatch>(points, false));
		}
		ImGui::SameLine();
		if (ImGui::Button("Add cylinder spline patch")) {
			int size[2] = { PatchCount[0] + 3, PatchCount[1] };
			auto points = PrepareRoundVertices(size);
			SceneObjects.emplace_back(std::make_shared<SplinePatch>(points, true));
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
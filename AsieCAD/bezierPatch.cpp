#include "bezierPatch.h"


int BezierPatch::Number = 0;
int BezierPatch::PatchCount[2] = { 1,1 };
float BezierPatch::PointDistance[2] = { 1,1 };
std::unique_ptr<Shader> BezierPatch::meshShader;

BezierPatch::BezierPatch(std::vector<std::shared_ptr<Point>> _points) :
	PointObject(("Bezier patch " +	std::to_string(Number)).c_str())
{
	if(!meshShader)
		meshShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	for (int i = 0; i < _points.size(); i++)
		points.emplace_back(
			ControlPoint{ std::weak_ptr<SceneObject>(_points[i]), false });
	isEditable = false;
	size[0] = PatchCount[0];
	size[1] = PatchCount[1];
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
	if (showMesh) {
		int pointCount[2] = { size[0] * 3 + 1, size[1] * 3 + 1 };
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
				if (j < pointCount[1] - 1) {
					indices.emplace_back(i * pointCount[1] + j);
					indices.emplace_back(i * pointCount[1] + j + 1);
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
}

void BezierPatch::RenderMenu()
{
	ImGui::Checkbox("Show mesh", &showMesh);
	PointObject::RenderMenu();
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

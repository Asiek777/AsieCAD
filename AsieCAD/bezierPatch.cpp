#include "bezierPatch.h"



#include <iosfwd>
#include <iosfwd>
#include <vector>
#include <vector>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat.hpp>


int BezierPatch::Number = 0;
int BezierPatch::PatchCount[2] = { 1,1 };
float BezierPatch::PointDistance[2] = { 1,1 };
std::unique_ptr<Shader> BezierPatch::meshShader;
std::unique_ptr<Shader> BezierPatch::patchShader;


BezierPatch::BezierPatch(std::vector<std::shared_ptr<Point>> _points) :
	PointObject(("Bezier patch " +	std::to_string(Number)).c_str())
{
	if (!meshShader) {
		meshShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
		patchShader = std::make_unique<Shader>("shaders/bezierPatch.vert",
			"shaders/torus.frag", "shaders/bezierPatch.geom");
	}
	for (int i = 0; i < _points.size(); i++)
		points.emplace_back(
			ControlPoint{ std::weak_ptr<SceneObject>(_points[i]), false });
	isEditable = false;
	size[0] = PatchCount[0];
	size[1] = PatchCount[1];
	curveCount[0] = 4;
	curveCount[1] = 4;
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
	if (showMesh)
		RenderMesh();
	std::vector<glm::vec3> knots(points.size());
	for (int i = 0; i < points.size(); i++)
		knots[i] = points[i].point.lock()->GetCenter();
	
	for (int i = 0; i < size[0]; i++) 
		for (int j = 0; j < size[1]; j++) {
			int offset[2] = { 3 * i, 3 * j };
			DrawPatch(offset, knots);
		}	
}

std::unique_ptr<MeshBuffer> BezierPatch::countCurvesPositions(int dim)
{
	std::vector<float> curves(curveCount[dim]);
	for (int i = 0; i < curves.size(); i++)
		curves[i] = (float)i / (curves.size() - 1);
	return std::make_unique<MeshBuffer>(curves, 1);
}

void BezierPatch::DrawPatch(int offset[2], std::vector<glm::vec3>& knots)
{
	int pointCount[2] = { size[0] * 3 + 1, size[1] * 3 + 1 };
	glm::mat4 coords[3];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			coords[0][j][i] = knots[(i + offset[0]) * pointCount[1] + j + offset[1]].x;
			coords[1][j][i] = knots[(i + offset[0]) * pointCount[1] + j + offset[1]].y;
			coords[2][j][i] = knots[(i + offset[0]) * pointCount[1] + j + offset[1]].z;
		}
	
	patchShader->use();
	patchShader->setMat4("Knots", coords[0], 3);
	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	patchShader->setVec3("color", color);
	patchShader->setBool("isForward", 1);
	patchShader->setMat4("viewProjection", viewProjection);

	std::unique_ptr<MeshBuffer> curveIndexes = countCurvesPositions(0);
	glBindVertexArray(curveIndexes->GetVAO());
	glDrawArrays(GL_POINTS, 0, curveCount[0]);

	patchShader->setBool("isForward", 0);
	curveIndexes = countCurvesPositions(1);
	glBindVertexArray(curveIndexes->GetVAO());
	glDrawArrays(GL_POINTS, 0, curveCount[1]);
	glBindVertexArray(0);

}

void BezierPatch::RenderMesh()
{
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

void BezierPatch::RenderMenu()
{
	ImGui::Checkbox("Show mesh", &showMesh);
	ImGui::DragInt2("Curves count", curveCount, 1, 2, 50);
	PointObject::RenderMenu();
}

void BezierPatch::RenderCreationMenu()
{
	if (ImGui::CollapsingHeader("Bezier patch creation")) {
		ImGui::DragInt2("Patches count", PatchCount, 1, 1, 20);
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

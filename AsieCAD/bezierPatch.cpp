#include "bezierPatch.h"



#include <iosfwd>
#include <iosfwd>
#include <vector>
#include <vector>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat.hpp>


int BezierPatch::Number = 0;
int BezierPatch::PatchCount[2] = { 1,1 };
float BezierPatch::PatchSize[2] = { 3,3 };
float BezierPatch::CylinderLength = 1;
float BezierPatch::CylinderRadius = 3;
std::unique_ptr<Shader> BezierPatch::meshShader;
std::unique_ptr<Shader> BezierPatch::patchShader;


BezierPatch::BezierPatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder) :
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
	isCylinder = _isCylinder;
	patchCount[0] = PatchCount[0];
	patchCount[1] = PatchCount[1];
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
	if (showMesh)
		RenderMesh();
	std::vector<glm::vec3> knots(points.size());
	for (int i = 0; i < points.size(); i++)
		knots[i] = points[i].point.lock()->GetCenter();
	
	for (int i = 0; i < patchCount[0]; i++) 
		for (int j = 0; j < patchCount[1]; j++) {
			int offset[2] = { 3 * i, 3 * j };
			DrawPatch(offset, knots);
		}
	if (isSelected)
		RenderSelectedPoints();
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
	int pointCount[2] = { patchCount[0] * 3 + 1, isCylinder ? patchCount[1] * 3 : patchCount[1] * 3 + 1 };
	glm::mat4 coords[3];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			int index = (i + offset[0]) * pointCount[1] + (j + offset[1]) % pointCount[1];
			coords[0][j][i] = knots[index].x;
			coords[1][j][i] = knots[index].y;
			coords[2][j][i] = knots[index].z;
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
	int pointCount[2] = { patchCount[0] * 3 + 1, isCylinder ? patchCount[1] * 3 : patchCount[1] * 3 + 1 };
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
		ImGui::DragFloat2("Patches patchCount", PatchSize,0.02, 0);
		ImGui::DragFloat("Cylinder length", &CylinderLength, 0.02, 0);
		ImGui::DragFloat("Cylinder radius", &CylinderRadius, 0.001, 0);
		
		if(ImGui::Button("Add patch")) {
			int size[2] = { PatchCount[0] * 3 + 1, PatchCount[1] * 3 + 1 };
			std::vector<std::shared_ptr<Point>>	points(size[0] * size[1]);
			glm::vec3 begin = GetCursorCenter();
			glm::vec2 PointDistance(PatchSize[0] / (size[0]-1), PatchSize[1] / (size[1] - 1));
			for (int i = 0; i < size[0]; i++)
				for (int j = 0; j < size[1]; j++)
					points[i * size[1] + j] = std::make_shared<Point>(begin +
						glm::vec3(PointDistance.x * i, 0, PointDistance.y * j));
			SceneObjects.emplace_back(std::make_shared<BezierPatch>(points, false));
			for (int i = 0; i < points.size(); i++) {
				points[i]->SetDeletability(false);
				SceneObjects.push_back(points[i]);
			}
		}
		
		ImGui::SameLine();
		if (ImGui::Button("Add cylinder patch")) {
			int size[2] = { PatchCount[0] * 3 + 1, PatchCount[1] * 3 };
			std::vector<std::shared_ptr<Point>>	points(size[0] * size[1]);
			glm::vec3 begin = GetCursorCenter();
			for (int i = 0; i < size[0]; i++)
				for (int j = 0; j < size[1]; j++) {
					float angle = j * glm::two_pi<float>() / size[1];
					glm::vec3 offset = glm::vec3(i * CylinderLength / size[0],
						std::sinf(angle) * CylinderRadius, std::cosf(angle) * CylinderRadius);
					points[i * size[1] + j] = std::make_shared<Point>(begin + offset);
				}
			SceneObjects.emplace_back(std::make_shared<BezierPatch>(points, true));
			for (int i = 0; i < points.size(); i++) {
				points[i]->SetDeletability(false);
				SceneObjects.push_back(points[i]);
			}
		}
	}
}

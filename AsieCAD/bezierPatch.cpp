#include "bezierPatch.h"



#include <iosfwd>
#include <iosfwd>
#include <vector>
#include <vector>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat.hpp>


int BezierPatch::Number = 0;
std::unique_ptr<Shader> BezierPatch::meshShader;
std::unique_ptr<Shader> BezierPatch::patchShader;


BezierPatch::BezierPatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder) :
	Surface(("Bezier patch " +	std::to_string(Number)).c_str())
{
	if (!meshShader) {
		meshShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
		patchShader = std::make_unique<Shader>("shaders/bezierPatch.vert",
			"shaders/torus.frag", "shaders/bezierPatch.geom");
	}
	for (int i = 0; i < _points.size(); i++)
		points.emplace_back(
			ControlPoint{ std::weak_ptr<SceneObject>(_points[i]), false });
	isCylinder = _isCylinder;
	curveIndexes[0] = std::make_unique<MeshBuffer>(std::vector<float>{ 1.f }, 1);
	curveIndexes[1] = std::make_unique<MeshBuffer>(std::vector<float>{ 1.f }, 1);
	UpdateCurvesBuffers();
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

	glBindVertexArray(curveIndexes[0]->GetVAO());
	glDrawArrays(GL_POINTS, 0, curveCount[0]);

	patchShader->setBool("isForward", 0);
	glBindVertexArray(curveIndexes[1]->GetVAO());
	glDrawArrays(GL_POINTS, 0, curveCount[1]);
	glBindVertexArray(0);

}

void BezierPatch::UpdateCurvesBuffers()
{
	for (int dim = 0; dim < 2; dim++) {
		std::vector<float> curves(curveCount[dim]);
		for (int i = 0; i < curves.size(); i++)
			curves[i] = (float)i / (curves.size() - 1);
		curveIndexes[dim]->UpdateBuffer(curves);
	}
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
	if(ImGui::DragInt2("Curves count", curveCount, 1, 2, 50))
		UpdateCurvesBuffers();
	PointObject::RenderMenu();
}

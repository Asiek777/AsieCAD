#include "gregoryPatch.h"
#include <vector>
#include <glm/detail/type_mat.hpp>

int GregoryPatch::Number = 0;
std::unique_ptr<Shader> GregoryPatch::patchShader;
std::unique_ptr<Shader> GregoryPatch::meshShader;

GregoryPatch::GregoryPatch(std::shared_ptr<BezierPatch> _patches[3], Border _border[3],
                           int _index) : SceneObject(("Gregory patch " + std::to_string(Number)).c_str())
{
	if (!patchShader) {
		patchShader = std::make_unique<Shader>("shaders/patch.vert",
			"shaders/torus.frag", "shaders/bezierPatch.geom");
		meshShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	}
	Number++;
	for (int i = 0; i < 3; i++) {
		patches[i] = _patches[i];
		border[i] = _border[i];
	}
	index = _index;
}

void GregoryPatch::UpdatePoints()
{
	auto curves = patches[index].lock()->GetBorderPoints(
		(Border)(4 ^ (int)border[index]));
	auto curveIn = CalcBezierHalf(
		std::vector<glm::vec3>(curves.begin(), curves.begin() + 4));
	auto curveOut = CalcBezierHalf(
		std::vector<glm::vec3>(curves.begin() + 4, curves.begin() + 8));
	for (int i = 0; i < 4; i++)
		curveOut[i] = 2.0f * curveIn[i] - curveOut[i];
	points[0].p = curveIn[0];
	points[0].e[1] = curveIn[1];
	points[1].e[0] = curveIn[2];
	points[1].p = curveIn[3];
	points[0].f[1] = curveOut[1];
	points[1].f[0] = points[1].f[1] = curveOut[2];
	points[1].e[1] = curveOut[3];
	curves = patches[(index + 1) % 3].lock()->GetBorderPoints(
		border[(index + 1) % 3]);
	curveIn = CalcBezierHalf(
		std::vector<glm::vec3>(curves.begin(), curves.begin() + 4));
	curveOut = CalcBezierHalf(
		std::vector<glm::vec3>(curves.begin() + 4, curves.begin() + 8));
	for (int i = 0; i < 4; i++)
		curveOut[i] = 2.0f * curveIn[i] - curveOut[i];
	points[0].e[0] = curveIn[1];
	points[3].e[1] = curveIn[2];
	points[3].p = curveIn[3];
	points[0].f[0] = curveOut[1];
	points[3].f[0] = points[3].f[1] = curveOut[2];
	points[3].e[0] = curveOut[3];

}
std::vector<glm::vec3> GregoryPatch::CalcBezierHalf(std::vector<glm::vec3> curve)
{
	for (int i = 3; i > 0; i--)
		for (int j = 0; j < i; j++)
			curve[j] = (curve[j] + curve[j + 1]) / 2.0f;
	std::reverse(curve.begin(), curve.end());
	return curve;
}


void GregoryPatch::Render()
{
	for (int i = 0; i < 3; i++)
		if (patches[i].expired())
			return;
	UpdatePoints();
	if (showMesh)
		RenderMesh();
}

void GregoryPatch::RenderMesh()
{
	std::vector<glm::vec3> vertices;
	for (int i = 0; i < 4; i++) {
		vertices.emplace_back(points[i].p);
		vertices.emplace_back(points[i].e[0]);
		vertices.emplace_back(points[i].e[0]);
		vertices.emplace_back(points[i].f[0]);
		vertices.emplace_back(points[i].p);
		vertices.emplace_back(points[i].e[1]);
		vertices.emplace_back(points[i].e[1]);
		vertices.emplace_back(points[i].f[1]);
		vertices.emplace_back(points[i].e[1]);
		vertices.emplace_back(points[(i + 1) % 4].e[0]);

	}
	auto mesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(vertices));
	meshShader->use();
	meshShader->setMat4("model", glm::mat4(1.0f));
	meshShader->setMat4("viewProjection", viewProjection);
	meshShader->setVec3("color", COLORS::BROKEN);
	glBindVertexArray(mesh->GetVAO());
	glDrawArrays(GL_LINES, 0, vertices.size());
	glBindVertexArray(0);
}


void GregoryPatch::RenderMenu()
{
	ImGui::Checkbox("Show mesh", &showMesh);
	ImGui::DragInt2("Curves count", curveCount, 0.3, 2, 64);
}


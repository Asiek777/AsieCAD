#include "BezierC0.h"
#include <functional>

std::unique_ptr<Shader> BezierC0::bezierShader;
std::unique_ptr<Shader> BezierC0::brokenShader;
glm::mat4 BezierC0::viewProjection;
int BezierC0::Number = 0;
BezierC0::BezierC0() : BezierC0(("C0 Curve " + std::to_string(Number)).c_str())
{
	Number++;
}
BezierC0::BezierC0(const char* _name) : Curve(_name)
{
	if (!bezierShader) {
		bezierShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag",
			"shaders/bezier.geom");
		brokenShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	}
}

void BezierC0::Render()
{
	clearExpired();
	if(isSelected)
		RenderSelectedPoints();
	int pointCount = points.size();
	if (pointCount == 0)
		return;
	UpdateIndices();
	RenderCurve();
	if(drawBroken)
		RenderBroken();
}
void BezierC0::RenderCurve()
{
	int pointCount = points.size();
	std::vector<glm::vec3> nodes(pointCount);
	for (int i = 0; i < pointCount; i++)
		nodes[i] = points[i].point.lock()->GetCenter();
	curveMesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(nodes), 
		true, indices);
	bezierShader->use();
	bezierShader->setMat4("model", glm::identity<glm::mat4>());
	bezierShader->setMat4("viewProjection", viewProjection);
	if (isSelected)
		bezierShader->setVec3("color", 1.f, 0.f, 0.f);
	else
		bezierShader->setVec3("color", 1.f, 1.f, 1.f);
	glBindVertexArray(curveMesh->GetVAO());
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void BezierC0::UpdateIndices()
{
	indices = std::vector<unsigned int>();
	int pointCount = points.size();
	int from = 0;
	while (from < pointCount - 1) {
		if (pointCount - from >= 4) {
			indices.emplace_back(from++);
			indices.emplace_back(from++);
			indices.emplace_back(from++);
			indices.emplace_back(from);
		}
		else if (pointCount - from == 3) {
			indices.emplace_back(from++);
			indices.emplace_back(from++);
			indices.emplace_back(from);
			indices.emplace_back(from);
		}
		else if (pointCount - from == 2) {
			indices.emplace_back(from++);
			indices.emplace_back(from);
			indices.emplace_back(from);
			indices.emplace_back(from);
		}
	}
}
void BezierC0::RenderBroken()
{
	//brokenMesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(nodes));
	brokenShader->use();
	brokenShader->setMat4("model", glm::identity<glm::mat4>());
	brokenShader->setMat4("viewProjection", viewProjection);
	brokenShader->setVec3("color", 0.f, 0.f, 1.f);
	glBindVertexArray(curveMesh->GetVAO());
	glDrawArrays(GL_LINE_STRIP, 0, points.size());
	glBindVertexArray(0);
}

#include "BezierC0.h"
#include <functional>

std::unique_ptr<Shader> BezierC0::shader;
glm::mat4 BezierC0::viewProjection;
int BezierC0::Number = 0;
BezierC0::BezierC0() : Curve(("C0 Curve " + std::to_string(Number)).c_str())
{
	if (!shader)
		shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag", 
			"shaders/bezier.geom");
	Number++;
}

void BezierC0::Render()
{
	clearExpired();
	if(isSelected)
		RenderSelectedPoints();
	int pointCount = points.size();
	if (pointCount == 0)
		return;
	std::vector<glm::vec3> nodes(pointCount);
	for (int i = 0; i < pointCount; i++)
		nodes[i] = points[i].point.lock()->GetCenter();
	std::vector<unsigned int> indices;
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
	curveMesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(nodes), 
		true, indices);
	shader->use();
	shader->setMat4("model", glm::identity<glm::mat4>());
	if (isSelected)
		shader->setVec3("color", 1.f, 0.f, 0.f);
	else
		shader->setVec3("color", 1.f, 1.f, 1.f);
	shader->setInt("Segments", 30);
	glBindVertexArray(curveMesh->GetVAO());
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	//std::vector<float> floatNodes = MeshBuffer::Vec3ToFloats(nodes);
	/*if (RenderBroken) {
		brokenMesh = std::make_unique<MeshBuffer>(floatNodes);
		glBindVertexArray(brokenMesh->GetVAO());
		shader->setVec3("color", 0.f, 0.f, 1.f);
		glDrawArrays(GL_LINE_STRIP, 0, pointCount);
		glBindVertexArray(0);
	}*/
	//std::vector<glm::vec3> curvePoints;
	//curvePoints.emplace_back(nodes[0]);

	//curveMesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(curvePoints));
	//if (isSelected)
	//	shader->setVec3("color", 1.f, 0.f, 0.f);
	//else
	//	shader->setVec3("color", 1.f, 1.f, 1.f);
	//glBindVertexArray(curveMesh->GetVAO());
	//glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, curvePoints.size());
	//glBindVertexArray(0);
}
void BezierC0::CalcCubic(int from, std::vector<glm::vec3>& nodes,
	std::vector<glm::vec3>& curvePoint)
{
	int steps = CalcStepCount(from, 3, nodes);
	std::cout << steps << std::endl;
	for (int i = 1; i <= steps; i++) {
		float t = (float)i / steps;
		float w = 1 - t;
		curvePoint.emplace_back(nodes[from] * w * w * w + 3.f * nodes[from + 1] * w * w * t +
			3.f * nodes[from + 2] * w * t * t + nodes[from + 3] * t * t * t);
	}
}
void BezierC0::CalcQuadratic(int from, std::vector<glm::vec3>& nodes,
	std::vector<glm::vec3>& curvePoint)
{
	int steps = CalcStepCount(from, 2, nodes);
	std::cout << steps << std::endl;
	for (int i = 1; i <= steps; i++) {
		float t = (float)i / steps;
		float w = 1 - t;
		curvePoint.emplace_back(nodes[from] * w * w  + 2.f * nodes[from + 1] * w * t +
			nodes[from + 2] * t * t);
	}
}
int BezierC0::CalcStepCount(int from, int degree, std::vector<glm::vec3>& nodes)
{
	std::vector<glm::vec2> screenCoord(points.size());
	for (int i = from; i <= from + degree; i++)
		screenCoord[i] = glm::vec2(viewProjection * glm::vec4(nodes[i], 1));
	float brokenLength = 0;
	for (int i = from; i <= from + degree - 1; i++)
		brokenLength += glm::distance(screenCoord[i], screenCoord[i + 1]);
	return  brokenLength * 10;
}


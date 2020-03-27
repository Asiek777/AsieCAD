#include "bezierCurve.h"
#include <functional>

std::unique_ptr<Shader> BezierCurve::shader;
glm::mat4 BezierCurve::viewProjection;
BezierCurve::BezierCurve() : Curve("Curve")
{
	if (!shader)
		shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
}

void BezierCurve::Render()
{
	clearExpired();
	int pointCount = points.size();
	if (pointCount == 0)
		return;
	std::vector<glm::vec3> nodes(pointCount);
	for (int i = 0; i < pointCount; i++)
		nodes[i] = points[i].point.lock()->GetCenter();
	std::vector<float> floatNodes = MeshBuffer::Vec3ToFloats(nodes);
	
	shader->use();
	shader->setMat4("model", glm::identity<glm::mat4>());
	if (RenderBroken) {
		brokenMesh = std::make_unique<MeshBuffer>(floatNodes);
		glBindVertexArray(brokenMesh->GetVAO());
		shader->setVec3("color", 0.f, 0.f, 1.f);
		glDrawArrays(GL_LINE_STRIP, 0, pointCount);
		glBindVertexArray(0);
	}
	std::vector<glm::vec3> curvePoints;
	curvePoints.emplace_back(nodes[0]);
	int from = 0;
	while (from < pointCount - 1) {
		if (pointCount - from >= 4) {
			CalcCubic(from, nodes, curvePoints);
			from += 3;
		}
		else if (pointCount - from == 3) {
			CalcQuadratic(from, nodes, curvePoints);
			from += 2;
		}
		else if (pointCount - from == 2) {
			curvePoints.emplace_back(nodes[from + 1]);
			from++;
		}
	}
	curveMesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(curvePoints));
	if (isSelected)
		shader->setVec3("color", 1.f, 0.f, 0.f);
	else
		shader->setVec3("color", 1.f, 1.f, 1.f);
	glBindVertexArray(curveMesh->GetVAO());
	glDrawArrays(GL_LINE_STRIP, 0, curvePoints.size());
	glBindVertexArray(0);
}
void BezierCurve::CalcCubic(int from, std::vector<glm::vec3>& nodes,
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
void BezierCurve::CalcQuadratic(int from, std::vector<glm::vec3>& nodes,
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
int BezierCurve::CalcStepCount(int from, int degree, std::vector<glm::vec3>& nodes)
{
	std::vector<glm::vec2> screenCoord(points.size());
	for (int i = from; i <= from + degree; i++)
		screenCoord[i] = glm::vec2(viewProjection * glm::vec4(nodes[i], 1));
	float brokenLength = 0;
	for (int i = from; i <= from + degree - 1; i++)
		brokenLength += glm::distance(screenCoord[i], screenCoord[i + 1]);
	return  brokenLength * 10;
}


#include "BezierC0.h"
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

std::unique_ptr<Shader> BezierC0::bezierShader;
std::unique_ptr<Shader> BezierC0::brokenShader;
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

BezierC0::BezierC0(tinyxml2::XMLElement* data) : BezierC0()
{
	drawBroken = data->BoolAttribute("ShowControlPolygon");
}

void BezierC0::Render()
{
	clearExpired();
	if(isSelected)
		RenderSelectedPoints();
	int pointCount = points.size();
	if (pointCount == 0)
		return;
	indices = CalcIndices(pointCount);
	RenderCurve();
	if(drawBroken)
		RenderBroken();
}

void BezierC0::Serialize(tinyxml2::XMLElement* scene)
{
	Curve::Serialize("BezierC0", scene);		
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
	bezierShader->setMat4("model", glm::mat4(1.0f));
	bezierShader->setMat4("viewProjection", viewProjection);
	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	bezierShader->setVec3("color", color);
	glBindVertexArray(curveMesh->GetVAO());
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
std::vector<unsigned int> BezierC0::CalcIndices(int pointCount)
{
	std::vector<unsigned int> result;
	int from = 0;
	while (from < pointCount - 1) {
		if (pointCount - from >= 4) {
			result.emplace_back(from++);
			result.emplace_back(from++);
			result.emplace_back(from++);
			result.emplace_back(from);
		}
		else if (pointCount - from == 3) {
			result.emplace_back(from++);
			result.emplace_back(from++);
			result.emplace_back(from);
			result.emplace_back(from);
		}
		else if (pointCount - from == 2) {
			result.emplace_back(from++);
			result.emplace_back(from);
			result.emplace_back(from);
			result.emplace_back(from);
		}
	}
	return result;
}
void BezierC0::RenderBroken()
{
	//brokenMesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(nodes));
	brokenShader->use();
	brokenShader->setMat4("model", glm::mat4(1.0f));
	brokenShader->setMat4("viewProjection", viewProjection);
	brokenShader->setVec3("color", COLORS::BROKEN);
	glBindVertexArray(curveMesh->GetVAO());
	glDrawArrays(GL_LINE_STRIP, 0, points.size());
	glBindVertexArray(0);
}

void BezierC0::DrawBezierCurve(std::vector<glm::vec3> &nodes, glm::vec3 color)
{
	std::vector<unsigned> indices = CalcIndices(nodes.size());
	auto mesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(nodes),
		true, indices);
	bezierShader->use();
	bezierShader->setMat4("model", glm::mat4(1.0f));
	bezierShader->setMat4("viewProjection", viewProjection);
	bezierShader->setVec3("color", color);
	glBindVertexArray(mesh->GetVAO());
	glDrawElements(GL_LINES_ADJACENCY, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void BezierC0::DrawBroken(std::vector<glm::vec3>& nodes, glm::vec3 color)
{
	auto mesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(nodes));
	brokenShader->use();
	brokenShader->setMat4("model", glm::mat4(1.0f));
	brokenShader->setMat4("viewProjection", viewProjection);
	brokenShader->setVec3("color", color);
	glBindVertexArray(mesh->GetVAO());
	glDrawArrays(GL_LINE_STRIP, 0, nodes.size());
	glBindVertexArray(0);
}

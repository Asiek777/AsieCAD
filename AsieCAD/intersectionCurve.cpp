#include "intersectionCurve.h"

std::unique_ptr<Shader> IntersectionCurve::shader;
int IntersectionCurve::Number = 0;

IntersectionCurve::IntersectionCurve(std::vector<IntersectionPoint> _points,
	std::shared_ptr<Surface> _s1, std::shared_ptr<Surface> _s2) :
	SceneObject(("Interpolation curve " + std::to_string(Number)).c_str()),
	s1(_s1), s2(_s2)
{
	Number++;
	if (!shader)
		shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	points = _points;
	std::vector < glm::vec3> pointsPos(points.size());
	for (int i = 0; i < points.size(); i++) {
		pointsPos[i] = points[i].location;
	}
	mesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(pointsPos));
}

void IntersectionCurve::Render()
{
	glBindVertexArray(mesh->GetVAO());
	shader->use();
	shader->setMat4("model", glm::mat4(1));
	shader->setMat4("viewProjection", viewProjection);
	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	shader->setVec3("color", color);
	glDrawArrays(GL_LINE_STRIP, 0, points.size());
	glBindVertexArray(0);
}

glm::vec3 IntersectionCurve::GetCenter()
{
	glm::vec3 center = glm::vec3(0);
	for (int i = 0; i < points.size(); i++) {
		center += points[i].location;
	}
	return center / (float)points.size();
}

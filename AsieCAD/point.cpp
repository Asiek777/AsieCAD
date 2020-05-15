#include "point.h"

std::unique_ptr<Shader> Point::shader;
std::unique_ptr<MeshBuffer> Point::mesh;
int Point::Number = 0;

Point::Point(const char* _name) : Clicable(_name)
{
}
Point::Point(glm::vec3 _location) : Clicable(("Point " + std::to_string(Number)).c_str())
{
	if (!shader) {
		shader = std::make_unique<Shader>("shaders/point.vert", "shaders/torus.frag");
		std::vector<float> vertex{ 0.f,0.f, 0.f };
		mesh = std::make_unique<MeshBuffer>(vertex);
	}
	location = _location;
	Number++;
}
void Point::Render()
{
	if (hasChanged > 3)
		hasChanged--;
	else if (hasChanged == 2)
		hasChanged = 0;
	glBindVertexArray(mesh->GetVAO());
	shader->use();
	shader->setVec3("position", location);
	shader->setMat4("viewProjection", viewProjection);
	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	shader->setVec3("color", color);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
void Point::RenderMenu()
{
	hasChanged = 0;
	if (ImGui::DragFloat3("location", &location.x, 0.02f))
		hasChanged = 1;
}
void Point::UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange)
{
	hasChanged = 3;
	Clicable::UpdatePosition(pos, scaleChange, rotChange);
	//hasChanged = 1;
}
void Point::DrawPoint(glm::vec3 position, glm::vec3 color)
{
	if (!shader) {
		shader = std::make_unique<Shader>("shaders/point.vert", "shaders/torus.frag");
		std::vector<float> vertex{ 0.f,0.f, 0.f };
		mesh = std::make_unique<MeshBuffer>(vertex);
	}
	glBindVertexArray(mesh->GetVAO());
	shader->use();
	shader->setVec3("position", position);
	shader->setVec3("color", color);
	shader->setMat4("viewProjection", viewProjection);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
std::shared_ptr<Point> Point::FakePoint(glm::vec3 _location, std::string name)
{
	Point* point = new Point(name.c_str());
	std::shared_ptr<Point> result(point);
	result->location = _location;
	return result;
}

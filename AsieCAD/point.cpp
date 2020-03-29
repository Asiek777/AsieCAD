#include "point.h"

std::unique_ptr<Shader> Point::shader;
std::unique_ptr<MeshBuffer> Point::mesh;
int Point::Number = 0;

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
	glBindVertexArray(mesh->GetVAO());
	shader->use();
	shader->setVec3("position", location);
	if (isSelected)
		shader->setVec3("color", 1.f, 0.f, 0.f);
	else
		shader->setVec3("color", 1.f, 1.f, 1.f);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
void Point::RenderMenu()
{
	hasChanged = false;
	hasChanged |= ImGui::DragFloat3("location", &location.x, 0.02f);
}
void Point::UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange)
{
	hasChanged = false;
	Clicable::UpdatePosition(pos, scaleChange, rotChange);
}
void Point::DrawPoint(glm::vec3 position, glm::vec3 color)
{
	glBindVertexArray(mesh->GetVAO());
	shader->use();
	shader->setVec3("position", position);
	shader->setVec3("color", color);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

#include "point.h"

std::unique_ptr<Shader> Point::shader;
std::unique_ptr<MeshBuffer> Point::mesh;

Point::Point(glm::vec3 location) : SceneObject("punkcik")
{
	if (!shader) {
		shader = std::make_unique<Shader>("shaders/point.vert", "shaders/torus.frag");
		std::vector<float> vertex{ 0.f,0.f, 0.f };
		mesh = std::make_unique<MeshBuffer>(vertex);
	}
	location = location;
}
void Point::Render()
{

	glBindVertexArray(mesh->GetVAO());
	shader->use();
	shader->setVec3("position", location);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
void Point::RenderMenu()
{
	ImGui::DragFloat3("location", &location.x, 0.02f);
}

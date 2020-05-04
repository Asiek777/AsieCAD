#include "cursor.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

std::unique_ptr<Shader> Cursor::shader;

glm::vec3 Cursor::GetRotationCenter()
{
	if (isRotationCenter)
		return location;
	else
		return glm::vec3(0);
}
Cursor::Cursor() : Clicable("Kursor")
{
	if (!shader)
		shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	std::vector<float> vertices{
		0.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f
	};
	std::vector<unsigned> indices{0, 1, 0, 2, 0, 3};
	mesh = std::make_unique<MeshBuffer>(vertices, 1, indices);
	location = glm::vec3(0);
}
void Cursor::Render()
{
	glBindVertexArray(mesh->GetVAO());
	shader->use();
	shader->setMat4("model", modelMatrix);
	shader->setMat4("viewProjection", viewProjection);
	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	shader->setVec3("color", color);
	glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void Cursor::RenderMenu()
{
	ImGui::Checkbox("Rotate around cursor", &isRotationCenter);
	if (ImGui::DragFloat3("location", &location.x, 0.02f))
		modelMatrix = glm::translate(glm::mat4(1), location);
}
void Cursor::UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange)
{
	location = pos;
	modelMatrix = glm::translate(glm::mat4(1), location);
}

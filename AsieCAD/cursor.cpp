#include "cursor.h"
#include <glm/ext/matrix_transform.inl>

std::unique_ptr<Shader> Cursor::shader;

Cursor::Cursor() :SceneObject("Kursor")
{
	if (!shader)
		shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	std::vector<float> vertices{ 0.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f };
	std::vector<unsigned> indices{ 0,1,0,2,0,3 };
	mesh = std::make_unique<MeshBuffer>(vertices, 1, indices);
	location = glm::vec3(0);
}
void Cursor::Render()
{	
	glBindVertexArray(mesh->GetVAO());
	shader->use();
	shader->setMat4("model", modelMatrix);
	glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void Cursor::RenderMenu()
{
	if (ImGui::DragFloat3("location", &location.x, 0.02f))
		modelMatrix = glm::translate(glm::mat4(1), location);
}

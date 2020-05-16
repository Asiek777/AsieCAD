#include "point.h"

std::unique_ptr<Shader> Point::shader;
int Point::Number = 0;

Point::Point(const char* _name) : Clicable(_name)
{
}

void Point::InitShader()
{
	if (!shader) {
		shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
		shader->use();
		shader->setMat4("model", glm::mat4(1));
	}
}

Point::Point(glm::vec3 _location) : Clicable(("Point " + std::to_string(Number)).c_str())
{
	InitShader();
	location = _location;
	Number++;
}
void Point::Render()
{
	if (hasChanged > 3)
		hasChanged--;
	else if (hasChanged == 2)
		hasChanged = 0;
	std::vector<float> coords = { location.x, location.y, location.z };
	auto mesh = std::make_unique<MeshBuffer>(coords);
	shader->use();
	shader->setMat4("viewProjection", viewProjection);
	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	shader->setVec3("color", color);
	glBindVertexArray(mesh->GetVAO());
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

void Point::RenderPoints()
{
	std::vector<glm::vec3> selected, notSelected;
	for (int i = 0; i < SceneObjects.size(); i++)
		if (SceneObjects[i]->IsPoint())
			if (SceneObjects[i]->isSelected)
				selected.emplace_back(SceneObjects[i]->GetCenter());
			else
				notSelected.emplace_back(SceneObjects[i]->GetCenter());
	DrawManyPoints(notSelected, COLORS::BASE);
	DrawManyPoints(selected, COLORS::HIGHLIGHT);
}

void Point::DrawPoint(glm::vec3 position, glm::vec3 color)
{
	InitShader();
	std::vector<float> coords = { position.x, position.y, position.z };
	auto mesh = std::make_unique<MeshBuffer>(coords);
	shader->use();
	shader->setVec3("color", color);
	shader->setMat4("viewProjection", viewProjection);
	glBindVertexArray(mesh->GetVAO());
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void Point::DrawManyPoints(std::vector<glm::vec3> coords, glm::vec3 color)
{
	InitShader();
	auto mesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(coords));
	shader->use();
	shader->setVec3("color", color);
	shader->setMat4("viewProjection", viewProjection);
	glBindVertexArray(mesh->GetVAO());
	glDrawArrays(GL_POINTS, 0, coords.size());
	glBindVertexArray(0);
}
std::shared_ptr<Point> Point::FakePoint(glm::vec3 _location, std::string name)
{
	Point* point = new Point(name.c_str());
	std::shared_ptr<Point> result(point);
	result->location = _location;
	return result;
}

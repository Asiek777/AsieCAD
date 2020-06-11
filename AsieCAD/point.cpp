#include "point.h"

#include "pointObject.h"

std::unique_ptr<Shader> Point::shader;
std::unique_ptr<MeshBuffer> Point::points;
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
		std::vector<float> point = { 0,0,0 };
		points = std::make_unique<MeshBuffer>(point);
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
	//if (hasChanged > 3)
	//	hasChanged--;
	//else if (hasChanged == 2)
	//	hasChanged = 0;
	//std::vector<float> coords = { location.x, location.y, location.z };
	//auto mesh = std::make_unique<MeshBuffer>(coords);
	//shader->use();
	//shader->setMat4("viewProjection", viewProjection);
	//glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	//shader->setVec3("color", color);
	//glBindVertexArray(mesh->GetVAO());
	//glDrawArrays(GL_POINTS, 0, 1);
	//glBindVertexArray(0);
}
void Point::RenderMenu()
{
	hasChanged = 0;
	if (ImGui::DragFloat3("location", &location.x, 0.02f))
		hasChanged = 1;
}

void Point::Serialize(tinyxml2::XMLElement* scene)
{
	auto ptr = scene->InsertNewChildElement("Point");
	ptr->SetAttribute("Name", name.c_str());
	auto pos = ptr->InsertNewChildElement("Position");
	pos->SetAttribute("X", location.x);
	pos->SetAttribute("Y", location.y);
	pos->SetAttribute("Z", location.z);
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
	points->UpdateBuffer(coords);
	shader->use();
	shader->setVec3("color", color);
	shader->setMat4("viewProjection", viewProjection);
	glBindVertexArray(points->GetVAO());
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void Point::DrawManyPoints(std::vector<glm::vec3> coords, glm::vec3 color)
{
	InitShader();
	points->UpdateBuffer(MeshBuffer::Vec3ToFloats(coords));
	shader->use();
	shader->setVec3("color", color);
	shader->setMat4("viewProjection", viewProjection);
	glBindVertexArray(points->GetVAO());
	glDrawArrays(GL_POINTS, 0, coords.size());
	glBindVertexArray(0);
}


void Point::MergePointsMenu()
{
	if(selectedCount == 2) {
		for (int i = 0; i < SceneObjects.size(); i++)
			if (SceneObjects[i]->isSelected && !SceneObjects[i]->IsPoint())
				return;
		if (ImGui::Button("Merge points"))
			MergePoints();
	}
}

void Point::MergePoints()
{
	int first = -1, second;
	for (int i = 0; i < SceneObjects.size(); i++)
		if(SceneObjects[i]->isSelected) {
			if (first == -1)
				first = i;
			else {
				second = i;
				break;
			}
		}
	for(int i=0;i<SceneObjects.size();i++) {
		if(SceneObjects[i]->IsPointObject()) {
			auto object = static_cast<PointObject*>(SceneObjects[i].get());
			object->SetPointRefToPoint(SceneObjects[first], SceneObjects[second]);			
		}
	}
	SceneObjects.erase(SceneObjects.begin() + second);
	Select(first);
}

std::shared_ptr<Point> Point::FakePoint(glm::vec3 _location, std::string name)
{
	Point* point = new Point(name.c_str());
	std::shared_ptr<Point> result(point);
	result->location = _location;
	return result;
}

#include "point.h"

std::unique_ptr<Shader> Point::shader;
void Point::Render(Shader& shader)
{
	//this->shader
}
void Point::RenderMenu()
{
	ImGui::DragFloat3("Location", &Location.x, 0.02f);
}

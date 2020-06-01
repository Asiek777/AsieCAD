#include "torus.h"
#include "point.h"
#include "toolXML.h"
#include "Imgui/imgui.h"

std::unique_ptr<Shader> Torus::shader;
int Torus::Number = 0;

Torus::Torus(int _smallCircle, int _bigCircle, float _smallRadius, float _bigRadius) :
	SceneObject(("Torus " + std::to_string(Number)).c_str())
{
	if(!shader)
        shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
    bigCount = _bigCircle;
    smallCount = _smallCircle;
    smallRadius = _smallRadius;
    bigRadius = _bigRadius;
    position.location = GetCursorCenter();
    position.UpdateMatrix();
    prepareBuffers();
    Number++;
}

Torus::Torus(tinyxml2::XMLElement* data):
	Torus(data->IntAttribute("VerticalSlices"), data->IntAttribute("HorizontalSlices"),
     data->FloatAttribute("MinorRadius"), data->FloatAttribute("MajorRadius"))
{
    position.location = ToolXML::ReadVector(data->FirstChildElement("Position"));
    position.rotation = ToolXML::ReadVector(data->FirstChildElement("Rotation"));
    position.scale = ToolXML::ReadVector(data->FirstChildElement("Scale"));
    position.UpdateMatrix();	
}

void Torus::prepareBuffers()
{
    std::vector<float> vertices;
    std::vector<unsigned> indices;
    for (int i = 0; i < bigCount; i++)
    {
        float bigAngle = 2 * M_PI * i / bigCount;
        for (int j = 0; j < smallCount; j++)
        {
            float smallAngle = 2 * M_PI * j / smallCount;
            vertices.emplace_back((bigRadius + smallRadius * std::cosf(smallAngle))
                * std::cosf(bigAngle));
            vertices.emplace_back(smallRadius * std::sinf(smallAngle));
            vertices.emplace_back((bigRadius + smallRadius * std::cosf(smallAngle))
                * std::sinf(bigAngle));
        }
        for (int i = 0; i < bigCount; i++)
            for (int j = 0; j < smallCount; j++)
            {
                indices.emplace_back(i * smallCount + j);
                indices.emplace_back(((i + 1) % bigCount * smallCount) + j);
                indices.emplace_back(i * smallCount + j);
                indices.emplace_back(i * smallCount + ((j + 1) % smallCount));
            }
    }
    mesh = std::make_unique<MeshBuffer>(vertices, true, indices);
}

void Torus::Render()
{
    glBindVertexArray(mesh->GetVAO());
    shader->use();
    shader->setMat4("model", position.GetModelMatrix());
    shader->setMat4("viewProjection", viewProjection);
    glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
    shader->setVec3("color", color);
    glDrawElements(GL_LINES, smallCount * bigCount * 4, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
	
}

void Torus::RenderMenu()
{
    bool hasChanged = false;
    hasChanged |= ImGui::SliderInt("Ring vertice count", &smallCount, 3, 70);
	hasChanged |= ImGui::SliderInt("Ring count", &bigCount, 3, 200);
    hasChanged |= ImGui::SliderFloat("Small radius", &smallRadius, 0, 10);
    hasChanged |= ImGui::SliderFloat("Big radius", &bigRadius, 0, 15);
    if (hasChanged)
        prepareBuffers();
    if (ImGui::CollapsingHeader("Position"))
        position.RenderMenu(GetCursorCenter());
}
glm::vec3 Torus::GetCenter()
{
    return position.location;
}
void Torus::UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange)
{
    position.UpdatePosition(pos, scaleChange, rotChange);
}

void Torus::Serialize(tinyxml2::XMLElement* scene)
{
    auto ptr = scene->InsertNewChildElement("Torus");
    ptr->SetAttribute("Name", name.c_str());
    ToolXML::SaveVec3(position.location, "Position", ptr);
    ToolXML::SaveVec3(position.rotation, "Rotation", ptr);
    ToolXML::SaveVec3(position.scale, "Scale", ptr);
    ptr->SetAttribute("MajorRadius", bigRadius);
    ptr->SetAttribute("MinorRadius", smallRadius);

    ptr->SetAttribute("VerticalSlices", smallCount);
    ptr->SetAttribute("HorizontalSlices", bigCount);
}

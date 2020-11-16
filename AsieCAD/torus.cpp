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
        shader = std::make_unique<Shader>("shaders/torusTrim.vert", "shaders/trim.frag");
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
    std::vector<glm::vec2> izolinesSmall, izolinesBig;
	if (isTrimmed) {
        auto curve = trimCurve.lock();
        izolinesSmall = curve->CalcTrimming(smallCount + 1, 0, isFirst);
        izolinesBig = curve->CalcTrimming(bigCount + 1, 1, isFirst);
	}
    std::vector<float> vertices;
    std::vector<unsigned> indices[2];
    for (int i = 0; i <= bigCount; i++)
    {
        float bigAngle = 2 * M_PI * i / bigCount;
        for (int j = 0; j <= smallCount; j++)
        {
            float smallAngle = 2 * M_PI * j / smallCount;
            vertices.emplace_back((bigRadius + smallRadius * std::cosf(smallAngle))
                * std::cosf(bigAngle));
            vertices.emplace_back(smallRadius * std::sinf(smallAngle));
            vertices.emplace_back((bigRadius + smallRadius * std::cosf(smallAngle))
                * std::sinf(bigAngle));
            vertices.emplace_back(i / (float)bigCount);
            vertices.emplace_back(j / (float)smallCount);    	
        }
    }
    for (int i = 0; i < bigCount; i++)
        for (int j = 0; j <= smallCount; j++)
        {
            indices[0].emplace_back(i * (smallCount + 1) + j);
            indices[0].emplace_back((i + 1) * (smallCount + 1) + j);
        }
    for (int i = 0; i <= bigCount; i++)
		for (int j = 0; j < smallCount; j++) {
            indices[1].emplace_back(i * (smallCount + 1) + j);
            indices[1].emplace_back(i * (smallCount + 1) + (j + 1));
        }
    size = indices[0].size();
    mesh[0] = std::make_unique<MeshBuffer>(vertices, true, indices[0], "TODO - po");
    mesh[1] = std::make_unique<MeshBuffer>(vertices, true, indices[1], "TODO - po");
}

void Torus::Render()
{
    glBindVertexArray(mesh[0]->GetVAO());
    shader->use();
    if (trimCurve.expired())
        isTrimmed = false;
    if (isTrimmed) {
        auto curve = trimCurve.lock();
        glBindTexture(GL_TEXTURE_2D, curve->GetTexture(isFirst));
        shader->setBool("isTrimmed", true);
    }
    else
        shader->setBool("isTrimmed", false);
    shader->setMat4("model", position.GetModelMatrix());
    shader->setMat4("viewProjection", viewProjection);
    shader->setBool("reverseTrimming", reverseTrimming && isTrimmed);
    glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
    shader->setVec3("color", color);
    glDrawElements(GL_LINES, bigCount * (smallCount + 1) * 2, GL_UNSIGNED_INT, 0);

    glBindVertexArray(mesh[1]->GetVAO());
    glDrawElements(GL_LINES, smallCount * (bigCount + 1) * 2, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
	
}

void Torus::RenderMenu()
{
    ImGui::Checkbox("Reverse trimming", &reverseTrimming);
    bool hasChanged = false;
    hasChanged |= ImGui::SliderInt("Ring vertice count", &smallCount, 3, 70);
	hasChanged |= ImGui::SliderInt("Ring count", &bigCount, 3, 200);
    hasChanged |= ImGui::SliderFloat("Small radius", &smallRadius, 0, 10);
    hasChanged |= ImGui::SliderFloat("Big radius", &bigRadius, 0, 15);
    if (hasChanged)
        prepareBuffers();
    if (ImGui::CollapsingHeader("Position"))
        position.RenderMenu(GetCursorCenter());
    TestSurfaceMenu();
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

glm::vec3 Torus::GetPointAt(float u, float v)
{
    float bigAngle = 2 * M_PI * u;
    float smallAngle = 2 * M_PI * v;
    glm::vec3 pos;
    if (bigRadius > 0)
        pos = glm::vec3(
            (bigRadius + smallRadius * std::cosf(smallAngle)) * std::cosf(bigAngle),
            smallRadius * std::sinf(smallAngle),
            (bigRadius + smallRadius * std::cosf(smallAngle)) * std::sinf(bigAngle));
    else
        pos = glm::vec3(
            smallRadius * std::sinf(smallAngle/2 + M_PI) * std::cosf(bigAngle),
            smallRadius * std::cosf(smallAngle/2 + M_PI),
            smallRadius * std::sinf(smallAngle/2 + M_PI) * std::sinf(bigAngle));
    return position.GetModelMatrix() * glm::vec4(pos, 1.0f);
}

TngSpace Torus::GetTangentAt(float u, float v)
{
    glm::vec3 diffU[2], diffV[2], antiNormal;
    TngSpace result;
    result.pos = GetPointAt(u, v);
    antiNormal = GetPointAt(u, v + 0.5f);
    diffU[0] = GetPointAt(u + 0.25f, v);
    diffU[1] = GetPointAt(u - 0.25f, v);
    diffV[0] = GetPointAt(u, v + 0.25f);
    diffV[1] = GetPointAt(u, v - 0.25f);
    result.diffU = (diffU[1] - diffU[0]) * (float)-M_PI;
    result.diffV = (diffV[1] - diffV[0]) * (float)-M_PI;
    if (bigRadius == 0)
        result.diffV /= 2;
    result.normal = glm::normalize(glm::cross(result.diffV, result.diffU));
    return result;
}

void Torus::SetTrimCurve(std::shared_ptr<IntersectionCurve> curve, bool _isFirst)
{
    isTrimmed = true;
    Surface::SetTrimCurve(curve, _isFirst);
    curve->CalcTrimming(20, 0, isFirst);
    curve->CalcTrimming(20, 1, isFirst);
}

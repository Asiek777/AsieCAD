#include "intersectionCurve.h"

#include "cubicInterpolated.h"
#include "Imgui/implot.h"

std::unique_ptr<Shader> IntersectionCurve::shader;
int IntersectionCurve::Number = 0;
std::weak_ptr<IntersectionCurve> IntersectionCurve::newest;

IntersectionCurve::IntersectionCurve(std::vector<IntersectionPoint> _points,
	std::shared_ptr<Surface> _s1, std::shared_ptr<Surface> _s2) :
	SceneObject(("Intersection curve " + std::to_string(Number)).c_str()),
	s1(_s1), s2(_s2)
{
	Number++;
	if (!shader)
		shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	points = _points;
	std::vector < glm::vec3> pointsPos(points.size());
	for (int i = 0; i < points.size(); i++) {
		pointsPos[i] = points[i].location;
	}
	mesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(pointsPos));
}

void IntersectionCurve::Render()
{
	glBindVertexArray(mesh->GetVAO());
	shader->use();
	shader->setMat4("model", glm::mat4(1));
	shader->setMat4("viewProjection", viewProjection);
	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	shader->setVec3("color", color);
	glDrawArrays(GL_LINE_STRIP, 0, points.size());
	glBindVertexArray(0);
}

void IntersectionCurve::RenderMenu()
{
	if (ImGui::Button("Convert to interpolation curve")) {
		auto curve = std::make_shared<CubicInterpolated>();
		for (int i = 0; i < points.size(); i++) {
			auto point = std::make_shared<Point>(points[i].location);
			curve->AddPoint(point);
			SceneObjects.emplace_back(point);
		}
		SceneObjects.emplace_back(curve);
	}
	if (ImGui::Button("Show plot"))
		for (int i = 0; i < SceneObjects.size(); i++)
			if (SceneObjects[i].get() == this) {
				newest = std::static_pointer_cast<IntersectionCurve>(SceneObjects[i]);
				break;
			}
}

glm::vec3 IntersectionCurve::GetCenter()
{
	glm::vec3 center = glm::vec3(0);
	for (int i = 0; i < points.size(); i++) {
		center += points[i].location;
	}
	return center / (float)points.size();
}

void IntersectionCurve::RenderPlot()
{
	if (newest.expired())
		return;
	ImGui::Begin("Intersection plot");
	auto curve = newest.lock();
	ImPlot::SetNextPlotLimits(0, 1, 0, 1);
	std::vector<float> x(curve->points.size());
	std::vector<float> y(curve->points.size());
	float minU[] = { 0,0,-1,-1 }, minV[] = { 0,-1,0,-1 };
	ImVec4 colors[] = { ImVec4(1,1,1,1), ImVec4(1,1,1,1) };
	ImPlot::SetColormap(&colors[0], 2);
	if (ImPlot::BeginPlot("plot 1", "U", "V", ImVec2(-1, 0),
		ImPlotFlags_Default ^ ImPlotFlags_Legend))
	{
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < curve->points.size(); i++) {
				x[i] = curve->points[i].coords.s + minU[j];
				y[i] = curve->points[i].coords.t + minV[j];
			}
			ImPlot::PlotLine("#polygon" , 
				x.data(), y.data(), x.size(), 0);
		}
		ImPlot::EndPlot();
	}
	//ImGui::SameLine();
	if (ImPlot::BeginPlot("plot 2", "U", "V", ImVec2(-1, 0),
		ImPlotFlags_Default ^ ImPlotFlags_Legend))
	{
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < curve->points.size(); i++) {
				x[i] = curve->points[i].coords.p + minU[j];
				y[i] = curve->points[i].coords.q + minV[j];
			}
			ImPlot::PlotLine(("#polygon 2" + std::to_string(j)).c_str(), 
				x.data(), y.data(), x.size(), 0);
		}
		ImPlot::EndPlot();
	}
	if (ImGui::Button("Close window"))
		newest.reset();
	ImGui::End();
}

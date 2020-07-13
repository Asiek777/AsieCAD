#include "intersectionCurve.h"

#include <algorithm>


#include "cubicInterpolated.h"
#include "Imgui/implot.h"

std::unique_ptr<Shader> IntersectionCurve::shader;
int IntersectionCurve::Number = 0;
bool IntersectionCurve::intersectionNotFound = false;
std::weak_ptr<IntersectionCurve> IntersectionCurve::newest;

IntersectionCurve::IntersectionCurve(std::vector<IntersectionPoint> _points, bool isClosed,
	std::shared_ptr<Surface> _s1, std::shared_ptr<Surface> _s2) :
	SceneObject(("Intersection curve " + std::to_string(Number)).c_str()),
	s1(_s1), s2(_s2)
{
	Number++;
	if (!shader)
		shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	points = _points;
	if (isClosed)
		points.emplace_back(points[0]);
	std::vector <glm::vec3> pointsPos(points.size());
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


std::vector<glm::vec2> IntersectionCurve::CalcTrimming(int lineCount, bool alongU,
                                                       bool isFirst)
{	
	std::vector<glm::vec2> coords;
	std::vector<glm::vec2> result(lineCount);
	for (int i = 0; i < points.size(); i++) {
		if (isFirst)
			coords.emplace_back(glm::vec2(points[i].coords.s, points[i].coords.t));
		else
			coords.emplace_back(glm::vec2(points[i].coords.p, points[i].coords.q));
		if (alongU)
			std::swap(coords[i].s, coords[i].t);
	}

	for (int i = 0; i < lineCount; i++) {
		float line = i / (float)(lineCount - 1);
		std::vector<float> intersections = std::vector<float>();
		for (int j = 0; j < coords.size() - 1; j++) {
			glm::vec2 p1(coords[j]), p2(coords[j + 1]);
			IzolineIntersection(line, intersections, p1, p2);
			//IzolineIntersection(line + 1, intersections, p1, p2);
			//IzolineIntersection(line - 1, intersections, p1, p2);
			
		}
		std::sort(intersections.begin(), intersections.end());
		if (intersections.empty()) {
			result[i] = glm::vec2(-1, -1);
		}
		else if(intersections.size() % 2 == 0) {
			result[i] = glm::vec2(intersections[0], 
				intersections[intersections.size() - 1]);
		}
		else {
			result[i] = glm::vec2(intersections[0], 1);
		}
	}
	return result;
}

void IntersectionCurve::IzolineIntersection(float line, std::vector<float>& intersections, 
	glm::vec2 p1, glm::vec2 p2)
{
	if ((p1.y - line) * (p2.y - line) < 0) {				
		float x = (p2.y * p1.x - line * p1.x + p2.x * line - p2.x * p1.y) /
			(p2.y - p1.y);
		intersections.emplace_back(x);
	}
}

void IntersectionCurve::RenderPlot()
{
	if(intersectionNotFound) {
		ImGui::Begin("Information");
		ImGui::Text("Intersection not found");
		if (ImGui::Button("OK"))
			intersectionNotFound = false;
		ImGui::End();
	}
	if (newest.expired())
		return;
	ImGui::Begin("Intersection plot");
	auto curve = newest.lock();
	ImPlot::SetNextPlotLimits(0, 1, 0, 1);
	std::vector<float> x(curve->points.size());
	std::vector<float> y(curve->points.size());
	float offset[] = { 1,0,-1 };
	ImVec4 colors[] = { ImVec4(1,1,1,1), ImVec4(1,1,1,1) };
	ImPlot::SetColormap(&colors[0], 2);
	if (ImPlot::BeginPlot("plot 1", "U", "V", ImVec2(-1, 0),
		ImPlotFlags_Default ^ ImPlotFlags_Legend))
	{
		for (int j = 0; j < 3; j++) 
			for (int k = 0; k < 3; ++k) {
				for (int i = 0; i < curve->points.size(); i++) {
					x[i] = curve->points[i].coords.s + offset[j];
					y[i] = curve->points[i].coords.t + offset[k];
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
		for (int j = 0; j < 3; j++) 
			for (int k = 0; k < 3; ++k){
				for (int i = 0; i < curve->points.size(); i++) {
					x[i] = curve->points[i].coords.p + offset[j];
					y[i] = curve->points[i].coords.q + offset[k];
				}
				ImPlot::PlotLine("#polygon" , 
					x.data(), y.data(), x.size(), 0);
			}
		ImPlot::EndPlot();
	}
	if (ImGui::Button("Close window"))
		newest.reset();
	ImGui::End();
}

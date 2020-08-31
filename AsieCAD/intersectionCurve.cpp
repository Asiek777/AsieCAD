#include "intersectionCurve.h"

#include <algorithm>


#include "cubicInterpolated.h"
#include "Imgui/implot.h"

std::unique_ptr<Shader> IntersectionCurve::shader;
int IntersectionCurve::Number = 0;
bool IntersectionCurve::intersectionNotFound = false;
std::weak_ptr<IntersectionCurve> IntersectionCurve::newest;

IntersectionCurve::IntersectionCurve(std::vector<IntersectionPoint> _points, Openness isClosed,
	std::shared_ptr<Surface> _s1, std::shared_ptr<Surface> _s2) :
	SceneObject(("Intersection curve " + std::to_string(Number)).c_str()),
	s1(_s1), s2(_s2)
{
	Number++;
	if (!shader)
		shader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	points = _points;
	if (isClosed == ClosedBoth) {
		points.emplace_back(points[0]);
		for (int i = 0; i < 4; i++)
			points[points.size() - 1].coords[i] += std::floorf(points[points.size() - 2].coords[i]);
	}
	if (s1 == s2)
		isClosed = Open;
	if(isClosed == Closed0 || isClosed == ClosedBoth)
		GenerateTextures(1);
	if (isClosed == Closed1 || isClosed == ClosedBoth)
		GenerateTextures(0);
	std::vector <glm::vec3> pointsPos(points.size());
	for (int i = 0; i < points.size(); i++) {
		pointsPos[i] = points[i].location;
	}
	mesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(pointsPos));
}

IntersectionCurve::~IntersectionCurve()
{
	glDeleteTextures(2, tex);
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

void IntersectionCurve::GenerateTextures(int texNumber)
{
	glGenTextures(1, tex + texNumber);
	static const int TEX_SIZE = 8192;
	static unsigned char texData[TEX_SIZE][TEX_SIZE];
	CalcTrimming(TEX_SIZE, 1, texNumber);
	for (int i = 0; i < TEX_SIZE; i++) {
		bool isIn = false;
		int vecInx = 0;
		std::vector<float>& line = grid[1 + 2 * !texNumber][i];
		for (int j = 0; j < TEX_SIZE; j++) {
			float linePix = (j + 1) / (float)(TEX_SIZE + 2) + 0.5f / TEX_SIZE;
			while (vecInx < line.size() && line[vecInx] < linePix) {
				isIn = !isIn;
				vecInx++;
			}
			texData[i][j] = isIn ? 0 : 255;
		}
	}
	glBindTexture(GL_TEXTURE_2D, tex[texNumber]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, TEX_SIZE, TEX_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, texData);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::vector<glm::vec2> IntersectionCurve::CalcTrimming(int lineCount, 
	bool alongU, bool isFirst)
{
	grid[alongU + 2*!isFirst] = std::vector<std::vector<float>>(lineCount);
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
	std::vector<float> startLine;
	CalcLineInteresctions(0, coords, startLine, true);
	if (!alongU && startLine.size() % 2 == 1 && (isFirst ? s1 : s2)->RollV())
		(isFirst ? s1 : s2)->isTrimmed = false;
	if (alongU && startLine.size() % 2 == 1 && (isFirst ? s1 : s2)->RollU())
		(isFirst ? s1 : s2)->isTrimmed = false;
	
	for (int i = 0; i < lineCount; i++) {
		std::vector<float> intersections = std::vector<float>();
		float line = i / (float)(lineCount - 1);
		for (int j = 0; j < startLine.size(); j += 2)
			if (line > startLine[j] && (j == startLine.size() - 1 || line < startLine[j + 1])) {
				intersections.emplace_back(0);
				break;
			}				
		
		CalcLineInteresctions(line, coords, intersections, false);
		
		if (intersections.empty()) {
			result[i] = glm::vec2(-1, -1);
		}
		else {
			if (intersections.size() % 2 == 1) 
				intersections.emplace_back(1);			
			result[i] = glm::vec2(intersections[0],
				intersections[intersections.size() - 1]);
		}
		grid[alongU + 2*!isFirst][i] = intersections;
	}
	return result;
}

void IntersectionCurve::CalcLineInteresctions(float line, std::vector<glm::vec2>& coords, 
	std::vector<float>& intersections, bool isReversed)
{
	for (int j = 0; j < coords.size() - 1; j++) {
		glm::vec2 p1(coords[j]), p2(coords[j + 1]);
		IzolineIntersection(line, intersections, p1, p2, isReversed);
		if (p1.x != 1.001f && p1.x != -0.001f && p1.y != 1.001f && p1.y != -0.001f &&
			p2.x != 1.001f && p2.x != -0.001f && p2.y != 1.001f && p2.y != -0.001f) {
			IzolineIntersection(line + 1, intersections, p1, p2, isReversed);
			IzolineIntersection(line - 1, intersections, p1, p2, isReversed);
		}
			
	}
	std::sort(intersections.begin(), intersections.end());
}

void IntersectionCurve::IzolineIntersection(float line, std::vector<float>& intersections,
	glm::vec2 p1, glm::vec2 p2, bool isReversed)
{
	if(isReversed) {
		std::swap(p1.x, p1.y);
		std::swap(p2.x, p2.y);
	}
	if ((p1.y - line) * (p2.y - line) < 0) {				
		float x = (p2.y * p1.x - line * p1.x + p2.x * line - p2.x * p1.y) /
			(p2.y - p1.y);
		if (x < 0 || x > 1)
			x -= std::floorf(x);
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
		if(curve->s1->isTrimmed)
			RenderPlotGrid(curve, 0);		
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
		if (curve->s2->isTrimmed)
			RenderPlotGrid(curve, 2);
		ImPlot::EndPlot();
	}
	if (ImGui::Button("Close window"))
		newest.reset();
	ImGui::End();
}

void IntersectionCurve::RenderPlotGrid(std::shared_ptr<IntersectionCurve> curve, int offset)
{
	float x[2], y[2];
	std::vector<std::vector<float>> *grid = &curve->grid[offset];
	for (int i = 0; i < (*grid).size(); i++)
		for (int j = 0; j < (*grid)[i].size(); j += 2) {
			x[0] = (*grid)[i][j];
			x[1] = (*grid)[i][j + 1];
			y[0] = i / (float)((*grid).size() - 1);
			y[1] = i / (float)((*grid).size() - 1);
			ImPlot::PlotLine("#grid", x, y, 2);
		}
	grid = &curve->grid[offset + 1];
	for (int i = 0; i < (*grid).size(); i++)
		for (int j = 0; j < (*grid)[i].size(); j += 2) {
			y[0] = (*grid)[i][j];
			y[1] = (*grid)[i][j + 1];
			x[0] = i / (float)((*grid).size() - 1);
			x[1] = i / (float)((*grid).size() - 1);
			ImPlot::PlotLine("#grid", x, y, 2);
		}
}

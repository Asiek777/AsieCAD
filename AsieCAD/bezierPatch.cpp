#include "bezierPatch.h"



#include <iosfwd>
#include <iosfwd>
#include <vector>
#include <vector>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat.hpp>


int BezierPatch::Number = 0;
std::unique_ptr<Shader> BezierPatch::patchShader;


BezierPatch::BezierPatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder) :
	Surface(_points, _isCylinder, ("Bezier patch " +	std::to_string(Number)).c_str())
{
	if (!patchShader) {
		patchShader = std::make_unique<Shader>("shaders/patch.vert",
			"shaders/torus.frag", "shaders/bezierPatch.geom");
	}
	Number++;
}

BezierPatch::BezierPatch(std::vector<std::shared_ptr<Point>> _points, 
	tinyxml2::XMLElement* data, bool _isCylinder, int rows, int cols):
	BezierPatch(_points, _isCylinder)	
{
	patchCount[0] = rows / 3;
	patchCount[1] = cols / 3;
	showMesh = data->BoolAttribute("ShowControlPolygon");
	curveCount[0] = data->IntAttribute("RowSlices") + 1;
	curveCount[1] = data->IntAttribute("ColumnSlices") + 1;
	UpdateCurvesBuffers();
}

void BezierPatch::Render()
{
	std::vector<glm::vec3> knots(points.size());
	for (int i = 0; i < points.size(); i++)
		knots[i] = points[i].point.lock()->GetCenter();
	
	for (int i = 0; i < patchCount[0]; i++) 
		for (int j = 0; j < patchCount[1]; j++) {
			int offset[2] = { 3 * i, 3 * j };
			DrawPatch(offset, knots);
		}
	if (showMesh)
		RenderMesh();
	if (isSelected)
		RenderSelectedPoints();
}

void BezierPatch::Serialize(tinyxml2::XMLElement* scene)
{
	int pointCount[2] = { patchCount[0] * 3 + 1, isCylinder ? patchCount[1] * 3 : patchCount[1] * 3 + 1 };
	Surface::Serialize(pointCount, scene, "PatchC0");
}

void BezierPatch::DrawPatch(int offset[2], std::vector<glm::vec3>& knots)
{
	int pointCount[2] = { patchCount[0] * 3 + 1, isCylinder ? patchCount[1] * 3 : patchCount[1] * 3 + 1 };
	glm::mat4 coords[3];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			int index = (i + offset[0]) * pointCount[1] + (j + offset[1]) % pointCount[1];
			coords[0][j][i] = knots[index].x;
			coords[1][j][i] = knots[index].y;
			coords[2][j][i] = knots[index].z;
		}
	
	patchShader->use();
	patchShader->setMat4("Knots", coords[0], 3);
	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	patchShader->setVec3("color", color);
	patchShader->setBool("isForward", 1);
	patchShader->setMat4("viewProjection", viewProjection);

	glBindVertexArray(curveIndexes[0]->GetVAO());
	glDrawArrays(GL_POINTS, 0, curveCount[0]);

	patchShader->setBool("isForward", 0);
	glBindVertexArray(curveIndexes[1]->GetVAO());
	glDrawArrays(GL_POINTS, 0, curveCount[1]);
	glBindVertexArray(0);

}

void BezierPatch::RenderMesh()
{
	int pointCount[2] = { patchCount[0] * 3 + 1, isCylinder ? patchCount[1] * 3 : patchCount[1] * 3 + 1 };
	Surface::RenderMesh(pointCount);
}

void BezierPatch::FillSurfaceMenu()
{
	if (selectedCount == 3) {
		std::shared_ptr<BezierPatch> patch[3];
		for (int i = 0; i < SceneObjects.size(); i++)
			if (SceneObjects[i]->isSelected) {
				if(!SceneObjects[i]->IsBezierPatch())
					return;
				if (!patch[0])
					patch[0] = std::static_pointer_cast<BezierPatch>(SceneObjects[i]);
				else if (!patch[1])
					patch[1] = std::static_pointer_cast<BezierPatch>(SceneObjects[i]);
				else {
					patch[2] = std::static_pointer_cast<BezierPatch>(SceneObjects[i]);
					break;
				}
			}
		for (int i = 0; i < 3; i++)
			if (patch[i]->patchCount[0] != 1 || patch[i]->patchCount[1] != 1)
				return;
		std::vector<std::shared_ptr<Point>> commonPoint[3];
		for (int i = 0; i < 3; i++) {
			commonPoint[i] = CommonPoints(patch[i], patch[(i + 1) % 3]);
			if (commonPoint[i].empty())
				return;
		}
		std::shared_ptr<Point> corner[3];
		for (int i = 0; i < 3; i++) {
			auto p1 = patch[i]->GetCornerOnePatch(commonPoint[i]);
			auto p2 = patch[(i + 1) % 3]->GetCornerOnePatch(commonPoint[i]);
			if (p1 != p2)
				return;
			corner[i] = p1;
		}
		Border border[3];
		for (int i = 0; i < 3; i++) {
			border[i] = patch[i]->GetBorder(corner[i], corner[(i + 2) % 3]);
			if (border[i] == none)
				return;
		}
		
		if(ImGui::Button("Fill hole")) {
			std::cout << "whole filled :P\n";
		}
		
	}
}

std::shared_ptr<Point> BezierPatch::GetCornerOnePatch(
	std::vector<std::shared_ptr<Point>> _points)
{
	if (patchCount[0] != 1 || patchCount[1] != 1)
		throw "za duzo platkow";
	int corners[] = { 0,3,12,15 };
	for (int i = 0; i < _points.size(); i++)
		for (int j = 0; j < 4; j++)
			if (_points[i] == points[corners[j]].point.lock())
				return _points[i];
	return std::shared_ptr<Point>();	
}

Border BezierPatch::GetBorder(std::shared_ptr<Point> p1, std::shared_ptr<Point> p2)
{
	if(p1 == points[0].point.lock()) {
		if (p2 == points[3].point.lock())
			return b03;
		if (p2 == points[12].point.lock())
			return b012;
	}
	if (p1 == points[3].point.lock()) {
		if (p2 == points[0].point.lock())
			return b30;
		if (p2 == points[15].point.lock())
			return b315;
	}
	if (p1 == points[12].point.lock()) {
		if (p2 == points[15].point.lock())
			return b1215;
		if (p2 == points[0].point.lock())
			return b120;
	}
	if (p1 == points[15].point.lock()) {
		if (p2 == points[12].point.lock())
			return b1512;
		if (p2 == points[3].point.lock())
			return b153;
	}
	return none;
}


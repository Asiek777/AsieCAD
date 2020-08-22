#include "bezierPatch.h"

#include <algorithm>
#include <iosfwd>
#include <vector>
#include <glm/detail/type_mat.hpp>

#include "gregoryPatch.h"


int BezierPatch::Number = 0;
std::unique_ptr<Shader> BezierPatch::patchShader;


BezierPatch::BezierPatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder) :
	PointSurface(_points, _isCylinder, ("Bezier patch " +	std::to_string(Number)).c_str())
{
	if (!patchShader) {
		patchShader = std::make_unique<Shader>("shaders/patch.vert",
			"shaders/trim.frag", "shaders/bezierPatch.geom");
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
	if(isTrimmed && trimCurve.expired()) {
		isTrimmed = false;
		UpdateCurvesBuffers();
	}
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
	PointSurface::Serialize(pointCount, scene, "PatchC0");
}

void BezierPatch::DrawPatch(int offset[2], std::vector<glm::vec3>& knots)
{
	int pointCount[2] = { patchCount[0] * 3 + 1,
		isCylinder ? patchCount[1] * 3 : patchCount[1] * 3 + 1 };
	glm::mat4 coords[3];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			int index = (i + offset[0]) * pointCount[1] + (j + offset[1]) % pointCount[1];
			coords[0][j][i] = knots[index].x;
			coords[1][j][i] = knots[index].y;
			coords[2][j][i] = knots[index].z;
		}
	
	patchShader->use();
	if (!trimCurve.expired()) {
		auto curve = trimCurve.lock();
		glBindTexture(GL_TEXTURE_2D, curve->GetTexture(isFirst));
		patchShader->setBool("isTrimmed", true);
	}
	else
		patchShader->setBool("isTrimmed", false);
	patchShader->setMat4("Knots", coords[0], 3);
	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	patchShader->setVec3("color", color);
	patchShader->setBool("isForward", 1);
	patchShader->setBool("reverseTrimming", reverseTrimming && isTrimmed);
	patchShader->setMat4("viewProjection", viewProjection);
	glm::vec4 coordsRange(offset[0] / 3 / (float)patchCount[0],
		(offset[0] / 3 + 1) / (float)patchCount[0],
		offset[1] / 3 / (float)patchCount[1],
		(offset[1] / 3 + 1) / (float)patchCount[1]);
	patchShader->setVec4("coordsRange", coordsRange);
	glBindVertexArray(curveIndexes[1]->GetVAO());
	glDrawArrays(GL_POINTS, curveCount[1] * offset[1] / 3, curveCount[1]);

	patchShader->setBool("isForward", 0);
	glBindVertexArray(curveIndexes[0]->GetVAO());
	glDrawArrays(GL_POINTS, curveCount[0] * offset[0] / 3, curveCount[0]);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void BezierPatch::RenderMesh()
{
	int pointCount[2] = { patchCount[0] * 3 + 1, isCylinder ? patchCount[1] * 3 : patchCount[1] * 3 + 1 };
	PointSurface::RenderMesh(pointCount);
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
			auto p1 = patch[i]->GetCorner(commonPoint[i]);
			auto p2 = patch[(i + 1) % 3]->GetCorner(commonPoint[i]);
			if (p1 != p2)
				return;
			corner[i] = p1;
		}
		Border border[3];
		for (int i = 0; i < 3; i++) {
			border[i] = patch[i]->GetBorderEnum(corner[i], corner[(i + 2) % 3]);
			if (border[i] == none)
				return;
		}
		
		if (ImGui::Button("Fill hole")) {
			std::cout << "whole filled :P\n";
			for (int i = 0; i < 3; i++)
				SceneObjects.emplace_back(std::make_shared<GregoryPatch>(patch, border, i));
		}		
	}
}

std::shared_ptr<Point> BezierPatch::GetCorner(
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

Border BezierPatch::GetBorderEnum(std::shared_ptr<Point> p1, std::shared_ptr<Point> p2)
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

std::vector<glm::vec3> BezierPatch::GetBorderPoints(Border border)
{
	std::vector<int> pointIndices;
	switch (border) {
	case b30:
		pointIndices = {3, 2, 1, 0, 7, 6, 5, 4};
		break;
	case b012:
		pointIndices = {0, 4, 8, 12, 1, 5, 9, 13};
		break;
	case b1215:
		pointIndices = {12, 13, 14, 15, 8, 9, 10, 11};
		break;
	case b153:
		pointIndices = {15, 11, 7, 3, 14, 10, 6, 2};
		break;
	case b03:
		pointIndices = {0, 1, 2, 3, 4, 5, 6, 7};
		break;
	case b120:
		pointIndices = {12, 8, 4, 0, 13, 9, 5, 1};
		break;
	case b1512:
		pointIndices = {15, 14, 13, 12, 11, 10, 9, 8};
		break;
	case b315:
		pointIndices = {3, 7, 11, 15, 2, 6, 10, 14};
		break;
	}
	auto result = std::vector<glm::vec3>(pointIndices.size());
	for (int i = 0; i < pointIndices.size(); i++)
		result[i] = points[pointIndices[i]].point.lock()->GetCenter();
	return result;

}

glm::vec3 BezierPatch::GetPointAt(float u, float v)
{
	u = glm::clamp(u, 0.f, 1.f);
	v = isCylinder ? v - std::floorf(v) : glm::clamp(v, 0.f, 1.f);
	int pointCount[2] = { patchCount[0] * 3 + 1,
		isCylinder ? patchCount[1] * 3 : patchCount[1] * 3 + 1 };
	u *= patchCount[0];
	v *= patchCount[1];
	int offset[2] = { std::min((int)std::floor(u),patchCount[0] - 1),
		std::min((int)std::floor(v),patchCount[1] - 1) };
	glm::vec4 uCoords = Bernstein3(u - offset[0]);
	glm::vec4 vCoords = Bernstein3(v - offset[1]);
	glm::vec3 sum = glm::vec3(0);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			int index = (i + 3 * offset[0]) * pointCount[1] + (j + 3 * offset[1]) % pointCount[1];
			sum += points[index].point.lock()->GetCenter() * uCoords[i] * vCoords[j];
		}
	return sum;
}

glm::vec4 BezierPatch::Bernstein3(float t)
{
	float t2 = t * t;
	float one_minus_t = 1.0 - t;
	float one_minus_t2 = one_minus_t * one_minus_t;
	return glm::vec4(one_minus_t2 * one_minus_t, 3.0 * t * one_minus_t2, 
		3.0 * t2 * one_minus_t, t2 * t);
}

glm::vec4 BezierPatch::BezierDiff(float t)
{
	float t2 = t * t;
	return glm::vec4(-3 + 6 * t - 3 * t2,
		3 - 12 * t + 9 * t2, 6 * t - 9 * t2, 3 * t2);
}

TngSpace BezierPatch::GetTangentAt(float u, float v)
{
	u = glm::clamp(u, 0.f, 1.f);
	v = isCylinder ? v - std::floorf(v) : glm::clamp(v, 0.f, 1.f);
	int pointCount[2] = { patchCount[0] * 3 + 1,
		isCylinder ? patchCount[1] * 3 : patchCount[1] * 3 + 1 };
	u *= patchCount[0];
	v *= patchCount[1];
	int offset[2] = { std::min((int)std::floor(u),patchCount[0] - 1),
		std::min((int)std::floor(v),patchCount[1] - 1) };
	glm::vec4 uCoords = Bernstein3(u - offset[0]);
	glm::vec4 vCoords = Bernstein3(v - offset[1]);
	glm::vec4 uBezDiff = BezierDiff(u - offset[0]);
	glm::vec4 vBezDiff = BezierDiff(v - offset[1]);
	TngSpace result;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			int index = (i + 3 * offset[0]) * pointCount[1] + (j + 3 * offset[1]) % pointCount[1];
			//sum += points[index].point.lock()->GetCenter() * uCoords[i] * vCoords[j];
			glm::vec3 pos = points[index].point.lock()->GetCenter();
			result.pos += pos * uCoords[i] * vCoords[j];
			result.diffU += pos * uBezDiff[i] * vCoords[j];
			result.diffV += pos * uCoords[i] * vBezDiff[j];
		}
	result.normal = normalize(glm::cross(result.diffU, result.diffV));
	result.diffU *= patchCount[0];
	result.diffV *= patchCount[1];
	return result;
}


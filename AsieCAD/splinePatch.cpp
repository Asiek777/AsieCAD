#include "splinePatch.h"

#include <algorithm>

#include "bezierPatch.h"


int SplinePatch::Number = 0;
std::unique_ptr<Shader> SplinePatch::patchShader;

SplinePatch::SplinePatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder):
	PointSurface(_points, _isCylinder, ("Spline patch " + std::to_string(Number)).c_str())
{
	if (!patchShader) {
		patchShader = std::make_unique<Shader>("shaders/patch.vert",
			"shaders/torus.frag", "shaders/splinePatch.geom");
	}
	Number++;
}

SplinePatch::SplinePatch(std::vector<std::shared_ptr<Point>> _points,
	tinyxml2::XMLElement* data, bool _isCylinder, int rows, int cols) :
	SplinePatch(_points, _isCylinder)
{
	patchCount[0] = rows - 3;
	patchCount[1] = cols - (isCylinder ? 0 : 3);
	showMesh = data->BoolAttribute("ShowControlPolygon");
	curveCount[0] = data->IntAttribute("RowSlices") + 1;
	curveCount[1] = data->IntAttribute("ColumnSlices") + 1;
	UpdateCurvesBuffers();
}

void SplinePatch::Render()
{
	std::vector<glm::vec3> knots(points.size());
	for (int i = 0; i < points.size(); i++)
		knots[i] = points[i].point.lock()->GetCenter();

	for (int i = 0; i < patchCount[0]; i++)
		for (int j = 0; j < patchCount[1]; j++) {
			int offset[2] = { i, j };
			DrawPatch(offset, knots);
		}
	if (showMesh)
		RenderMesh();
	if (isSelected)
		RenderSelectedPoints();
}

void SplinePatch::Serialize(tinyxml2::XMLElement* scene)
{
	int pointCount[2] = { patchCount[0] + 3, isCylinder ? patchCount[1] : patchCount[1] + 3 };
	PointSurface::Serialize(pointCount, scene, "PatchC2");
}

void SplinePatch::DrawPatch(int offset[2], std::vector<glm::vec3> knots)
{
	int pointCount[2] = { patchCount[0] + 3, isCylinder ? patchCount[1] : patchCount[1] + 3 };
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

void SplinePatch::RenderMesh()
{
	int pointCount[2] = { patchCount[0] + 3, isCylinder ? patchCount[1] : patchCount[1] + 3 };
	PointSurface::RenderMesh(pointCount);
}

glm::vec3 SplinePatch::GetPointAt(float u, float v)
{
	u = glm::clamp(u, 0.f, 1.f);
	v = glm::clamp(v, 0.f, 1.f);
	int pointCount[2] = { patchCount[0] + 3, isCylinder ? patchCount[1] : patchCount[1] + 3 };
	u *= patchCount[0];
	v *= patchCount[1];
	int offset[2] = { std::min((int)std::floor(u),patchCount[0] - 1),
		std::min((int)std::floor(v),patchCount[1] - 1) };
	u -= offset[0];
	v -= offset[1];
	glm::vec4 uCoords = glm::vec4(Spline(u, -2, 3), Spline(u, -1, 3), 
		Spline(u, 0, 3), Spline(u, 1, 3));
	glm::vec3 Boors[4] = { glm::vec4(0), glm::vec4(0), glm::vec4(0), glm::vec4(0) };
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			int index = (i + offset[0]) * pointCount[1] + (j + offset[1]) % pointCount[1];
			Boors[j] += uCoords[i] * points[index].point.lock()->GetCenter();
		}
	glm::vec3 B[4], result(0);
	B[0] = Boors[0] * 1.f / 3.f + Boors[1] * 2.f / 3.f;
	B[1] = Boors[1] * 2.f / 3.f + Boors[2] * 1.f / 3.f;
	B[2] = Boors[1] * 1.f / 3.f + Boors[2] * 2.f / 3.f;
	B[3] = Boors[2] * 2.f / 3.f + Boors[3] * 1.f / 3.f;
	B[0] = B[0] * 1.f / 2.f + B[1] * 1.f / 2.f;
	B[3] = B[2] * 1.f / 2.f + B[3] * 1.f / 2.f;
	glm::vec4 bezier = BezierPatch::Bernstein3(v);
	for (int i = 0; i < 4; i++)
		result += B[i] * bezier[i];
	return result;
}

float SplinePatch::Spline(float t, float ti, int n)
{
	static float intval = 1.0f;
	if(n == 0) {
		if (ti > t && ti - intval <= t)
			return 1;
		else
			return 0;
	}
	float val1 = Spline(t, ti, n - 1) * (t - ti + intval);
	float val2 = Spline(t, ti + intval, n - 1) * (ti + n * intval - t);
	return (val1 + val2) / (n * intval);
}

TngSpace SplinePatch::GetTangentAt(float u, float v)
{
	u = glm::clamp(u, 0.f, 1.f);
	v = glm::clamp(v, 0.f, 1.f);
	int pointCount[2] = { patchCount[0] + 3, isCylinder ? patchCount[1] : patchCount[1] + 3 };
	u *= patchCount[0];
	v *= patchCount[1];
	TngSpace result;
	int offset[2] = { std::min((int)std::floor(u),patchCount[0] - 1),
		std::min((int)std::floor(v),patchCount[1] - 1) };
	u -= offset[0];
	v -= offset[1];
	glm::vec4 uCoords = glm::vec4(Spline(u, -2, 3), Spline(u, -1, 3),
		Spline(u, 0, 3), Spline(u, 1, 3));
	glm::vec4 vCoords = glm::vec4(Spline(v, -2, 3), Spline(v, -1, 3),
		Spline(v, 0, 3), Spline(v, 1, 3));
	glm::vec3 uBoors[4], vBoors[4];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int index = (i + offset[0]) * pointCount[1] + (j + offset[1]) % pointCount[1];
			uBoors[j] += uCoords[i] * points[index].point.lock()->GetCenter();
			vBoors[i] += vCoords[j] * points[index].point.lock()->GetCenter();
		}
	}
	glm::vec3 B[4];
	B[0] = uBoors[0] * 1.f / 3.f + uBoors[1] * 2.f / 3.f;
	B[1] = uBoors[1] * 2.f / 3.f + uBoors[2] * 1.f / 3.f;
	B[2] = uBoors[1] * 1.f / 3.f + uBoors[2] * 2.f / 3.f;
	B[3] = uBoors[2] * 2.f / 3.f + uBoors[3] * 1.f / 3.f;
	B[0] = B[0] * 1.f / 2.f + B[1] * 1.f / 2.f;
	B[3] = B[2] * 1.f / 2.f + B[3] * 1.f / 2.f;
	glm::vec4 bezier = BezierPatch::Bernstein3(v);
	glm::vec4 bezierDiff = BezierPatch::BezierDiff(v);
	for (int i = 0; i < 4; i++) {
		result.pos += B[i] * bezier[i];
		result.diffV += B[i] * bezierDiff[i];
	}
	B[0] = vBoors[0] * 1.f / 3.f + vBoors[1] * 2.f / 3.f;
	B[1] = vBoors[1] * 2.f / 3.f + vBoors[2] * 1.f / 3.f;
	B[2] = vBoors[1] * 1.f / 3.f + vBoors[2] * 2.f / 3.f;
	B[3] = vBoors[2] * 2.f / 3.f + vBoors[3] * 1.f / 3.f;
	B[0] = B[0] * 1.f / 2.f + B[1] * 1.f / 2.f;
	B[3] = B[2] * 1.f / 2.f + B[3] * 1.f / 2.f;
	bezierDiff = BezierPatch::BezierDiff(u);
	for (int i = 0; i < 4; i++) 
		result.diffU += B[i] * bezierDiff[i];
	result.normal = glm::normalize(glm::cross(result.diffU, result.diffV));
	result.diffU *= patchCount[0];
	result.diffV *= patchCount[1];
	return result;
}

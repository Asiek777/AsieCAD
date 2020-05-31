#include "splinePatch.h"


int SplinePatch::Number = 0;
std::unique_ptr<Shader> SplinePatch::patchShader;

SplinePatch::SplinePatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder):
	Surface(_points, _isCylinder, ("Spline patch " + std::to_string(Number)).c_str())
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
	Surface::RenderMesh(pointCount);
}

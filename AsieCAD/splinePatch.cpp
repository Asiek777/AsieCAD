#include "splinePatch.h"


int SplinePatch::Number = 0;

SplinePatch::SplinePatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder):
	Surface(_points, _isCylinder, ("Spline patch " + std::to_string(Number)).c_str())
{
	Number++;
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
}

void SplinePatch::RenderMesh()
{
	int pointCount[2] = { patchCount[0] + 3, isCylinder ? patchCount[1] : patchCount[1] + 3 };
	Surface::RenderMesh(pointCount);
}

void SplinePatch::RenderMenu()
{
	ImGui::Checkbox("Show mesh", &showMesh);
	ImGui::DragInt2("Curves count", curveCount, 1, 2, 50);
	PointObject::RenderMenu();
}

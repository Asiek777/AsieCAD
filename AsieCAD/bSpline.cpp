#include "bSpline.h"

#include "BezierC0.h"

int BSpline::Number = 0;
BSpline::BSpline() : Curve(("Spline " + std::to_string(Number)).c_str())
{
	if (!BezierC0::bezierShader) {
		BezierC0::bezierShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag",
			"shaders/bezier.geom");
		BezierC0::brokenShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	}
	Number++;
}
void BSpline::Render()
{
	clearExpired();
	if (isSelected)
		RenderSelectedPoints();
	int pointCount = points.size();
	if (pointCount < 4)
		return;
	std::vector<glm::vec3> deBors(pointCount);
	for (int i = 0; i < pointCount; i++)
		deBors[i] = points[i].point.lock()->GetCenter();
	std::vector<glm::vec3> midPoints, bezierPoints;
	for (int i = 1; i < pointCount; i++) {
		midPoints.emplace_back(deBors[i - 1] * 2.f / 3.f + deBors[i] * 1.f / 3.f);
		midPoints.emplace_back(deBors[i - 1] * 1.f / 3.f + deBors[i] * 2.f / 3.f);
	}
	for (int i = 1; i < midPoints.size() - 2; i++) {
		if (i > 1)
			bezierPoints.emplace_back(midPoints[i]);
		if (i % 2 == 1)
			bezierPoints.emplace_back((midPoints[i] + midPoints[i + 1]) / 2.f);
	}
	glm::vec3 color = isSelected ? glm::vec3(1, 0, 0) : glm::vec3(1);
	BezierC0::DrawBezierCurve(bezierPoints, color);
	if(drawBroken) {
		BezierC0::DrawBroken(deBors, glm::vec3(0.3, 0.3, 1));
		BezierC0::DrawBroken(midPoints, glm::vec3(0.3, 0.3, 1));
	}

}

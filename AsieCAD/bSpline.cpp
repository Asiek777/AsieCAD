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
	if (!isBezier) {
		int pointCount = points.size();
		if (pointCount < 4)
			return;
		std::vector<glm::vec3> deBors(pointCount);
		std::vector<glm::vec3> midPoints;
		for (int i = 0; i < pointCount; i++)
			deBors[i] = points[i].point.lock()->GetCenter();
		CalcBezierCoordinates(pointCount, deBors, midPoints);
		glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
		BezierC0::DrawBezierCurve(bezierCoords, color);
		if(drawBroken) {
			BezierC0::DrawBroken(deBors);
			BezierC0::DrawBroken(midPoints);
		}
		if (isSelected)
			RenderSelectedPoints();
	}
	else {
		UpdateBezierContinuity();
		bezierCoords = std::vector<glm::vec3>();
		for (int i = 0; i < bezierPoints.size(); i++)
			bezierCoords.emplace_back(bezierPoints[i]->GetCenter());
		glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
		if (isSelected) {
			Point::DrawManyPoints(bezierCoords, COLORS::BASE);
			RenderSelectedPoints();
		}
		BezierC0::DrawBezierCurve(bezierCoords, color);
		if (drawBroken)
			BezierC0::DrawBroken(bezierCoords);
	}

}
void BSpline::CalcBezierCoordinates(int pointCount, std::vector<glm::vec3>& deBors, 
	std::vector<glm::vec3>& midPoints)
{
	bezierCoords = std::vector<glm::vec3>();
	for (int i = 1; i < pointCount; i++) {
		midPoints.emplace_back(deBors[i - 1] * 2.f / 3.f + deBors[i] * 1.f / 3.f);
		midPoints.emplace_back(deBors[i - 1] * 1.f / 3.f + deBors[i] * 2.f / 3.f);
	}
	for (int i = 1; i < midPoints.size() - 2; i++) {
		if (i > 1)
			bezierCoords.emplace_back(midPoints[i]);
		if (i % 2 == 1)
			bezierCoords.emplace_back((midPoints[i] + midPoints[i + 1]) / 2.f);
	}
}
void BSpline::RenderMenu()
{
	if (isBezier) {
		if(ImGui::Button("Return to de Boor's coordinates'")) {
			isBezier = false;
			isEditable = false;
			ableMultiSelect = true;
			points = pointsCopy;
			bezierPoints.erase(bezierPoints.begin(), bezierPoints.end());			
		}
	}
	else
		if(ImGui::Button("Edit as beziers")) {
			isBezier = true;
			isEditable = false;
			ableMultiSelect = false;
			pointsCopy = points;
			points = std::vector<ControlPoint>();
			for (int i = 0; i < bezierCoords.size(); i++) {
				bezierPoints.emplace_back(Point::FakePoint(bezierCoords[i],
					"bezierPoint " + std::to_string(i)));
				AddPoint(bezierPoints[i]);
			}
		}		
	Curve::RenderMenu();
}
void BSpline::UpdateBezierContinuity()
{
	int i;
	for (i = 0; i < bezierPoints.size() && 1 != bezierPoints[i]->HasChanged(); i++);
	if(i<bezierPoints.size()) {
		int closestDeBoor = (i + 1) / 3 + 1;
		auto deboor0 = pointsCopy[closestDeBoor - 1].point.lock();
		auto deboor1 = pointsCopy[closestDeBoor].point.lock();
		auto deboor2 = pointsCopy[closestDeBoor + 1].point.lock();
		glm::vec3 scaleMid;
		switch (i % 3) {
		case 0:
			scaleMid = (deboor0->GetCenter() + deboor2->GetCenter()) / 2.f;
			break;
		case 1:
			scaleMid = deboor2->GetCenter();
			break;
		case 2:
			scaleMid = deboor0->GetCenter();
			break;
		}
		deboor1->UpdatePosition(3.f / 2 * (bezierCoords[i] - scaleMid) + scaleMid);
		int pointCount = pointsCopy.size();
		std::vector<glm::vec3> deBors(pointCount),midPoints;
		for (int j = 0; j < pointCount; j++)
			deBors[j] = pointsCopy[j].point.lock()->GetCenter();
		CalcBezierCoordinates(pointCount, deBors, midPoints);
		for (int j = 0; j < bezierCoords.size(); j++)
			if (j != i)
				bezierPoints[j]->UpdatePosition(bezierCoords[j]);
		
	}
}

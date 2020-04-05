#include "bezierC2.h"

int BezierC2::Number = 0;

BezierC2::BezierC2() : BezierC0(("C2 Curve " + std::to_string(Number)).c_str())
{
	Number++;
}
void BezierC2::Update()
{
	for (int i = 4; i < points.size(); i += 3) {
		auto P0 = points[i - 2].point.lock();
		auto P1 = points[i - 1].point.lock();
		auto P2 = points[i - 0].point.lock();
		if(!P2->HasChanged()) {
			glm::vec3 pos0 = P0->GetCenter();
			glm::vec3 pos1 = P1->GetCenter();
			P2->UpdatePosition(2.f * pos1 - pos0);
		}
		else if(!P1->HasChanged()) {
			glm::vec3 pos0 = P0->GetCenter();
			glm::vec3 pos1 = P2->GetCenter();
			P1->UpdatePosition((pos1 + pos0) / 2.f);
		} else {
			glm::vec3 pos1 = P1->GetCenter();
			glm::vec3 pos2 = P2->GetCenter();
			P0->UpdatePosition(2.f * pos1 - pos2);
		};
	}
}
void BezierC2::Render()
{
	clearExpired();
	Update();
	if (isSelected)
		RenderSelectedPoints();
	int pointCount = points.size();
	if (pointCount == 0)
		return;
	indices = CalcIndices(pointCount);
	RenderCurve();
	if (drawBroken)
		RenderBroken();
}

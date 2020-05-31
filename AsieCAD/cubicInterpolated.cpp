#include "cubicInterpolated.h"
#include <iosfwd>
#include <vector>
#include <glm/detail/type_mat.hpp>
#include "BezierC0.h"


int CubicInterpolated::Number = 0;

CubicInterpolated::CubicInterpolated() : Curve(("Cubic curve " + 
	std::to_string(Number)).c_str())
{
	if (!BezierC0::bezierShader) {
		BezierC0::bezierShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag",
			"shaders/bezier.geom");
		BezierC0::brokenShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	}
	Number++;
}

CubicInterpolated::CubicInterpolated(tinyxml2::XMLElement* data) : CubicInterpolated()
{
	drawBroken = data->BoolAttribute("ShowControlPolygon");
}

void CubicInterpolated::Render()
{
	clearExpired();
	if (points.size() == 0)
		return;
	if (HasChanged())
		CalcBezierPoints();

	glm::vec3 color = isSelected ? COLORS::HIGHLIGHT : COLORS::BASE;
	BezierC0::DrawBezierCurve(bezierPoints, color);
	if (drawBroken)
		BezierC0::DrawBroken(bezierPoints);
	if(isSelected)
		RenderSelectedPoints();
	hasChanged = 0;
}

void CubicInterpolated::Serialize(tinyxml2::XMLElement* scene)
{
	Curve::Serialize("BezierInter", scene);
}

void CubicInterpolated::CalcBezierPoints()
{
	std::vector<glm::vec3> knots;
	for (int i = 0; i < points.size(); i++) {
		glm::vec3 knot = points[i].point.lock()->GetCenter();
		if (i == 0 || knot != knots[knots.size() - 1])
			knots.emplace_back(knot);
	}
	int knotCount = knots.size();
	std::vector<glm::vec3> a(knotCount - 1), b(knotCount - 1), c(knotCount), d(knotCount - 1);
	std::vector<float> alfa(knotCount - 1), beta(knotCount - 1), diag(knotCount - 1),
	                   dist(knotCount - 1);
	bezierPoints = std::vector<glm::vec3>(3 * knotCount - 2);
	for (int i = 0; i < knotCount - 1; i++)
		dist[i] = glm::distance(knots[i], knots[i + 1]);
	for (int i = 1; i < knotCount - 1; i++) {
		alfa[i] = dist[i - 1] / (dist[i - 1] + dist[i]);
		beta[i] = dist[i] / (dist[i - 1] + dist[i]);
		diag[i] = 2;
		c[i] = 3.f * ((knots[i + 1] - knots[i]) / dist[i] - 
			(knots[i] - knots[i - 1]) / dist[i - 1]) / (dist[i - 1] + dist[i]);
	}

	//solving equation
    if (knotCount > 2) {
      beta[1] /= diag[1];
      c[1] /= diag[1];
      for (int i = 2; i < knotCount - 1; i++) {
        float m = 1.0f / (diag[i] - alfa[i] * beta[i - 1]);
        beta[i] *= m;
        c[i] = (c[i] - alfa[i] * c[i - 1]) * m;
      }
      for (int i = knotCount - 3; i > 1; i--)
        c[i] -= beta[i] * c[i + 1];	
      c[1] -= beta[1] * c[2];
    }
	c[0] = c[knotCount - 1] = glm::vec3(0);
	
	for (int i = 0; i < knotCount - 1; i++) {
		a[i] = knots[i];
		d[i] = (c[i + 1] - c[i]) / (3.f * dist[i]);
		c[i] *= dist[i] * dist[i];
		d[i] *= dist[i] * dist[i] * dist[i];
		b[i] = knots[i + 1] - a[i] - c[i] - d[i];
		
		bezierPoints[3 * i] = knots[i];
		bezierPoints[3 * i + 1] = a[i] + 1.f / 3.f * b[i];
		bezierPoints[3 * i + 2] = a[i] + 2.f / 3.f * b[i] + 1.f / 3.f * c[i];
	}
	bezierPoints[3 * knotCount - 3] = knots[knotCount - 1];
	return;
}

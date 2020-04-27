#include "cubicInterpolated.h"

#include "BezierC0.h"


int CubicInterpolated::Number = 0;

CubicInterpolated::CubicInterpolated() : Curve(("Cubic curve " + std::to_string(Number)).c_str())
{
	if (!BezierC0::bezierShader) {
		BezierC0::bezierShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag",
			"shaders/bezier.geom");
		BezierC0::brokenShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	}
	Number++;
}

void CubicInterpolated::Render()
{
	clearExpired();
	if (points.size() == 0)
		return;
	int knotCount = points.size();
	std::vector<glm::vec3> knots(knotCount);
	for (int i = 0; i < knotCount; i++)
		knots[i] = points[i].point.lock()->GetCenter();
	std::vector<glm::vec3> a(knotCount - 1), b(knotCount - 1), c(knotCount - 1), d(knotCount - 1);
	std::vector<glm::vec3> alfa(knotCount), beta(knotCount), diag(knotCount), D(knotCount);
	std::vector<glm::vec3> bezierPoints(3 * knotCount - 2);
	for (int i = 0; i < knotCount; i++) {
		alfa[i] = glm::vec3(1);
		beta[i] = glm::vec3(1);
		diag[i] = glm::vec3(4);
		D[i] = 3.f * (knots[i == knotCount - 1 ? knotCount - 1 : i + 1] - knots[i == 0 ? i : i - 1]);
	}
	diag[0] = diag[knotCount - 1] = glm::vec3(2);

	//solving equation
	beta[0] /= diag[0];
	D[0] /= diag[0];
	for (int i = 1; i < knotCount; i++) {
		glm::vec3 m = 1.0f / (diag[i] - alfa[i] * beta[i - 1]);
		beta[i] *= m;
		D[i] = (D[i] - alfa[i] * D[i - 1]) * m;
	}
	for (int i = knotCount - 2; i > 0; i--)
		D[i] -= beta[i] * D[i + 1];	
	D[0] -= beta[0] * D[1];
	
	for (int i = 0; i < knotCount - 1; i++) {
		a[i] = knots[i];
		b[i] = D[i];
		c[i] = 3.f * (knots[i + 1] - knots[i]) - 2.f * D[i] - D[i + 1];
		d[i] = 2.f * (knots[i] - knots[i + 1]) + D[i] + D[i + 1];
		bezierPoints[3 * i] = knots[i];
		bezierPoints[3 * i + 1] = a[i] + 1.f / 3.f * b[i];
		bezierPoints[3 * i + 2] = a[i] + 2.f / 3.f * b[i] + 1.f / 3.f * c[i];
	}
	bezierPoints[3 * knotCount - 3] = knots[knotCount - 1];

	glm::vec3 color = isSelected ? glm::vec3(1, 0, 0) : glm::vec3(1);
	BezierC0::DrawBezierCurve(bezierPoints, color);
	if (drawBroken)
		BezierC0::DrawBroken(bezierPoints);

}

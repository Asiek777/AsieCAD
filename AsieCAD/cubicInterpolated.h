#pragma once
#include "curve.h"
class CubicInterpolated :
	public Curve
{
	static int Number;
	std::vector<glm::vec3> CalcBezierPoints();
public:
	CubicInterpolated();
	void Render() override;	
};


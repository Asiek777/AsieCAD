#pragma once
#include "curve.h"
class BSpline :
	public Curve
{
	static int Number;
public:
	static glm::mat4 viewProjection;
	BSpline();
	void Render() override;
};


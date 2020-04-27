#pragma once
#include "curve.h"
class CubicInterpolated :
	public Curve
{
	static int Number;
public:
	CubicInterpolated();
	void Render() override;	
};


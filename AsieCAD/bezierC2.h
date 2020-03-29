#pragma once
#include "BezierC0.h"
class BezierC2 :
	public BezierC0
{
protected:
	static int Number;
	void Update();
public:
	BezierC2();
	void Render() override;
};


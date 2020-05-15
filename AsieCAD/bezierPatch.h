#pragma once
#include "point.h"
#include "pointObject.h"
class BezierPatch :
	public PointObject
{
	static int Number;
	static int PatchCount[2];
	static float PointDistance[2];
public:
	BezierPatch(std::vector<std::shared_ptr<Point>> _points);
	~BezierPatch();
	void Render();	
	static void RenderCreationMenu();
};


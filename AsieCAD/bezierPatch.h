#pragma once
#include "point.h"
#include "pointObject.h"
class BezierPatch :
	public PointObject
{
	bool showMesh = false;
	int size[2];
	
	static int Number;
	static int PatchCount[2];
	static float PointDistance[2];
	static std::unique_ptr<Shader> meshShader;
	
public:
	BezierPatch(std::vector<std::shared_ptr<Point>> _points);
	~BezierPatch();
	void Render();
	void RenderMenu() override;
	static void RenderCreationMenu();
};


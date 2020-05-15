#pragma once
#include "point.h"
#include "pointObject.h"

class BezierPatch :
	public PointObject
{
	bool showMesh = false;
	int size[2];
	int curveCount[2];
	
	static int Number;
	static int PatchCount[2];
	static float PointDistance[2];
	static std::unique_ptr<Shader> meshShader;
	static std::unique_ptr<Shader> patchShader;
	
	void DrawPatch(int offset[], std::vector<glm::vec3>& knots);
public:
	BezierPatch(std::vector<std::shared_ptr<Point>> _points);
	~BezierPatch();
	void RenderMesh();
	void Render();
	std::unique_ptr<MeshBuffer> countCurvesPositions(int dim);
	void RenderMenu() override;
	static void RenderCreationMenu();
};


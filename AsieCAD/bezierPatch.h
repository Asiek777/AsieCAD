#pragma once
#include "point.h"
#include "pointObject.h"

class BezierPatch :
	public PointObject
{
	bool showMesh = false;
	int patchCount[2];
	int curveCount[2];
	bool isCylinder;
	
	static int Number;
	static int PatchCount[2];
	static float PatchSize[2];
	static float CylinderRadius, CylinderLength;
	static std::unique_ptr<Shader> meshShader;
	static std::unique_ptr<Shader> patchShader;
	
	void DrawPatch(int offset[], std::vector<glm::vec3>& knots);
public:
	BezierPatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder);
	~BezierPatch();
	void RenderMesh();
	void Render();
	std::unique_ptr<MeshBuffer> countCurvesPositions(int dim);
	void RenderMenu() override;
	static void RenderCreationMenu();
};


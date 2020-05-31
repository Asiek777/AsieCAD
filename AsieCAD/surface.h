#pragma once
#include "point.h"
#include "pointObject.h"

class Surface : public PointObject
{
protected:
	bool showMesh = false;
	bool isCylinder = false;
	int patchCount[2];
	int curveCount[2];
	std::shared_ptr<MeshBuffer> curveIndexes[2];
	
	static int PatchCount[2];
	static float PatchSize[2];
	static float CylinderRadius, CylinderLength;
	static std::unique_ptr<Shader> meshShader;
	
	void RenderMesh(int pointCount[2]);
	void UpdateCurvesBuffers();

	static std::vector<std::shared_ptr<Point>> PrepareFlatVertices(int size[]);
	static std::vector<std::shared_ptr<Point>> PrepareRoundVertices(int size[2]);
	
public:	
	Surface(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder, const char* _name);
	~Surface();
	void RenderMenu() override;
	
	static void RenderCreationMenu();
};


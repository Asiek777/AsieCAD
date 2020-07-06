#pragma once
#include "point.h"
#include "pointObject.h"
#include "surface.h"

class PointSurface : public PointObject, public Surface
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
	PointSurface(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder, const char* _name);
	~PointSurface();
	void RenderMenu() override;
	bool IsSurface() override { return true; }
	void Serialize(int pointCount[], tinyxml2::XMLElement* scene, std::string type);
	static void RenderCreationMenu();
};


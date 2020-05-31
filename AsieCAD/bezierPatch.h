#pragma once
#include "surface.h"
#include "point.h"
#include "pointObject.h"

class BezierPatch :
	public Surface
{
	//std::unique_ptr<MeshBuffer> curveIndexes[2];
	
	static int Number;
	static std::unique_ptr<Shader> patchShader;
	
	void DrawPatch(int offset[], std::vector<glm::vec3>& knots);
public:
	BezierPatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder);
	BezierPatch(std::vector<std::shared_ptr<Point>> _points,
		tinyxml2::XMLElement* data, bool _isCylinder, int rows, int cols);
	void RenderMesh();
	void Render() override;
};


#pragma once
#include "PointSurface.h"
#include "point.h"
#include "pointObject.h"

enum Border;

class BezierPatch :	public PointSurface
{
	static int Number;
	static std::unique_ptr<Shader> patchShader;
	
	void DrawPatch(int offset[], std::vector<glm::vec3>& knots);
	std::shared_ptr<Point> GetCorner(std::vector<std::shared_ptr<Point>> _points);
	Border GetBorderEnum(std::shared_ptr<Point> p1, std::shared_ptr<Point> p2);
	
	
public:
	BezierPatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder);
	BezierPatch(std::vector<std::shared_ptr<Point>> _points,
		tinyxml2::XMLElement* data, bool _isCylinder, int rows, int cols);
	void RenderMesh();
	void Render() override;
	bool IsBezierPatch() override { return true; }
	void Serialize(tinyxml2::XMLElement* scene) override;

	std::vector<glm::vec3> GetBorderPoints(Border border);
	static void FillSurfaceMenu();
	glm::vec3 GetPointAt(float u, float v) override;
	TngSpace GetTangentAt(float u, float v) override;
	static glm::vec4 Bernstein3(float t);
	static glm::vec4 BezierDiff(float t);
};

enum Border
{
	b30, b012, b1215, b153, b03, b120, b1512, b315, none
};
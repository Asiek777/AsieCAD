#pragma once
#include "PointSurface.h"
class SplinePatch :
	public PointSurface
{
	static int Number;
	static std::unique_ptr<Shader> patchShader;
public:
	SplinePatch(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder);
	SplinePatch(std::vector<std::shared_ptr<Point>> _points, tinyxml2::XMLElement* data,
	            bool _isCylinder, int rows, int cols);
	void RenderMesh();
	void DrawPatch(int offset[2], std::vector<glm::vec3> knots);
	void Render() override;
	void Serialize(tinyxml2::XMLElement* scene) override;

	glm::vec3 GetPointAt(float u, float v) override;
	float Spline(float t, float ti, int n);
	TngSpace GetTangentAt(float u, float v) override;
};


#pragma once
#include "sceneObject.h"
#include "point.h"
#include "curve.h"

class BezierCurve :
	public Curve
{
	std::unique_ptr<MeshBuffer> brokenMesh;
	std::unique_ptr<MeshBuffer> curveMesh;
	void CalcCubic(int from, std::vector<glm::vec3> &nodes, std::vector<glm::vec3> &curvePoint);
	void CalcQuadratic(int from, std::vector<glm::vec3>& nodes, std::vector<glm::vec3>& curvePoint);
	int CalcStepCount(int from, int degree, std::vector<glm::vec3>& nodes);
public:
	static glm::mat4 viewProjection;
	static std::unique_ptr<Shader> shader;
	BezierCurve();
	void RenderMenu() override;
	void Render() override;
};


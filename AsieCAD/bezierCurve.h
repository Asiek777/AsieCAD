#pragma once
#include "sceneObject.h"
#include "point.h"

struct CurvePoint
{
	std::weak_ptr<SceneObject> point;
	bool isSelected;
};

class BezierCurve :
	public SceneObject
{
	std::vector<CurvePoint> points;
	Position selectedCenter;
	bool RenderBroken = false;
	std::unique_ptr<MeshBuffer> brokenMesh;
	std::unique_ptr<MeshBuffer> curveMesh;
	int SelectedCount();
	void clearExpired();
	void CalcCubic(int from, std::vector<glm::vec3> &nodes, std::vector<glm::vec3> &curvePoint);
	void CalcQuadratic(int from, std::vector<glm::vec3>& nodes, std::vector<glm::vec3>& curvePoint);
	int CalcStepCount(int from, int degree, std::vector<glm::vec3>& nodes);
public:
	static glm::mat4 viewProjection;
	static std::unique_ptr<Shader> shader;
	BezierCurve();
	bool AddPoint(std::shared_ptr<SceneObject>& point);
	bool IsCurve() override { return true; }
	void RenderMenu() override;
	void Render() override;
	glm::vec3 GetCenter() override;
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange) override {}
};


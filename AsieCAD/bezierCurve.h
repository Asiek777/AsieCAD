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
	int SelectedCount();
	void clearExpired();
	Position selectedCenter;
public:
	BezierCurve() : SceneObject("Curve") {}
	bool AddPoint(std::shared_ptr<SceneObject>& point);
	bool IsCurve() override { return true; }
	void RenderMenu() override;
	void Render() override;
	glm::vec3 GetCenter() override;
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange) override {}
};


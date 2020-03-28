#pragma once
#include "sceneObject.h"

struct CurvePoint
{
	std::weak_ptr<SceneObject> point;
	bool isSelected;
};


class Curve :
	public SceneObject
{
protected:
	bool RenderBroken = false;
	std::vector<CurvePoint> points;
	Position selectedCenter;
	int SelectedCount();
	void clearExpired();
	void RenderSelectedPoints();
public:
	Curve(const char* _name) : SceneObject(_name) {}
	bool IsCurve() override { return true; }
	glm::vec3 GetCenter() override;
	bool AddPoint(std::shared_ptr<SceneObject>& point);
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange) override {}
	void RenderMenu() override;
};


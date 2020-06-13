#pragma once
#include "point.h"
#include "sceneObject.h"

struct ControlPoint
{
	std::weak_ptr<SceneObject> point;
	bool isSelected;
};

class PointObject :
	public SceneObject
{
protected:
	bool ableMultiSelect = true;
	bool isEditable = true;
	std::vector<ControlPoint> points;
	Position selectedCenter;
	char hasChanged = 0;
	int SelectedCount();
	void clearExpired();
	void RenderSelectedPoints();
	char HasChanged() override;
	static std::vector<std::shared_ptr<Point>> CommonPoints(std::shared_ptr<PointObject> p1,
		std::shared_ptr<PointObject> p2);
public:
	PointObject(const char* _name) : SceneObject(_name) {}
	glm::vec3 GetCenter() override;
	void calcSelectioncenter();
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange) override {}
	void RenderMenu() override;
	bool IsPointObject() override { return true; }
	void SetPointRefToPoint(std::shared_ptr<SceneObject>& merged, 
		std::shared_ptr<SceneObject>& deleted);
};


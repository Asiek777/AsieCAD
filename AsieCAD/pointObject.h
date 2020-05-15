#pragma once
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
public:
	PointObject(const char* _name) : SceneObject(_name) {}
	glm::vec3 GetCenter() override;
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange) override {}
	void RenderMenu() override;
};


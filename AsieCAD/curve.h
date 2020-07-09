#pragma once
#include "pointObject.h"


class Curve :
	public PointObject
{
protected:
	bool drawBroken = false;
	

public:
	Curve(const char* _name) : PointObject(_name) {}
	bool IsCurve() override { return true; }
	bool AddPoint(std::shared_ptr<SceneObject> point);
	void RenderMenu() override;
	void Serialize(std::string type, tinyxml2::XMLElement* scene);
};


#pragma once
#include "curve.h"


class BSpline :
	public Curve
{
	static int Number;
	bool isBezier = false;
	std::vector<glm::vec3> bezierCoords;
	std::vector<std::shared_ptr<SceneObject>> bezierPoints;
	std::vector<ControlPoint> pointsCopy;
	void UpdateBezierContinuity();
	void CalcBezierCoordinates(int pointCount, std::vector<glm::vec3>& deBors, 
		std::vector<glm::vec3>& midPoints);
public:
	static glm::mat4 viewProjection;
	BSpline();
	BSpline(tinyxml2::XMLElement* data);
	void Render() override;
	void RenderMenu() override;
	void Serialize(tinyxml2::XMLElement* scene) override;
};


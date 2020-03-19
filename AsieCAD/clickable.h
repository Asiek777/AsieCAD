#pragma once
#include "sceneObject.h"

class Clicable : public SceneObject
{
protected:
	glm::vec3 location;
public:
	Clicable(const char* _name) : SceneObject(_name){}
	bool IsClicable() override
	{
		return true;
	}
	float DistanceToLine(glm::vec3& point, glm::vec3& dir)
	{
		glm::vec3 M = point - location;
		glm::vec3 dist = glm::vec3(M.y * dir.z - M.z * dir.y,
			M.z * dir.x - M.x * dir.z, M.x * dir.y - M.y * dir.x);
		return glm::length(dist);
	}
};
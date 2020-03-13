#pragma once
#include "sceneObject.h"
class Point :
	public SceneObject
{
	static std::unique_ptr<Shader> shader;
	glm::vec3 Location;
	void Render(Shader& shader) override;
	void RenderMenu() override;
};


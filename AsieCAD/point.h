#pragma once
#include "sceneObject.h"
#include "meshBuffer.h"

class Point :
	public SceneObject
{
	glm::vec3 location;
	static std::unique_ptr<MeshBuffer> mesh;
public:
	static std::unique_ptr<Shader> shader;
	Point(glm::vec3 _location = glm::vec3(0));
	void Render() override;
	void RenderMenu() override;
};


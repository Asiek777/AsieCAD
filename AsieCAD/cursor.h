#pragma once
#include "sceneObject.h"
#include "meshBuffer.h"

class Cursor :
	public SceneObject
{
	glm::vec3 location;
	std::unique_ptr<MeshBuffer> mesh;
	glm::mat4 modelMatrix = glm::mat4(1);
public:
	static std::unique_ptr<Shader> shader;
	Cursor();
	void Render() override;
	void RenderMenu() override;
};


#pragma once
#include "meshBuffer.h"
#include "clickable.h"

class Point :
	public Clicable
{
	static std::unique_ptr<MeshBuffer> mesh;
public:
	static std::unique_ptr<Shader> shader;
	Point(glm::vec3 _location = glm::vec3(0));
	void Render() override;
	void RenderMenu() override;
};


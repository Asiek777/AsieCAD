#pragma once
#include "meshBuffer.h"
#include "clickable.h"

class Point :
	public Clicable
{
	static std::unique_ptr<MeshBuffer> mesh;
public:
	static std::unique_ptr<Shader> shader;
	Point();
	void Render() override;
	void RenderMenu() override;
	bool IsPoint() override { return true; }
	static void DrawPoint(glm::vec3 position, glm::vec3 color);
};


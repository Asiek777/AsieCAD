#pragma once
#include "meshBuffer.h"
#include "clickable.h"

class Point :
	public Clicable
{
	static int Number;
	static std::unique_ptr<MeshBuffer> mesh;
	char hasChanged;
public:
	static std::unique_ptr<Shader> shader;
	Point(glm::vec3 location);
	Point(float x, float y, float z) : Point(glm::vec3(x,y,z)) {}
	void Render() override;
	void RenderMenu() override;
	bool IsPoint() override { return true; }
	bool HasChanged() override { return hasChanged; }
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange = glm::vec3(1), 
		glm::vec3 rotChange = glm::vec3(0)) override;
	static void DrawPoint(glm::vec3 position, glm::vec3 color);
};


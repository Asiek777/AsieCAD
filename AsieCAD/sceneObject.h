#pragma once
#include <string>
#include "glm/glm.hpp"
#include "Imgui/imgui.h"
#include <memory>
#include <vector>
#include "shader_s.h"

class SceneObject 
{
protected:
	std::string name;
	char text[64];
public:
	SceneObject() {};
	SceneObject(const char* _name);
	virtual ~SceneObject() = default;
	void RenderFullMenu();
	virtual void RenderMenu() = 0;
	virtual void Render(Shader& shader) = 0;
	static std::vector<std::unique_ptr<SceneObject>> SceneObjects;
	static void ItemListMenu();
	static void AddItemMenu();
};

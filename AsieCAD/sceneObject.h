#pragma once
#include <string>
#include "glm/glm.hpp"
#include "Imgui/imgui.h"
#include <memory>
#include <vector>
#include "shader_s.h"

class SceneObject 
{
	static int selected;
	static int selectedCount;
protected:
	std::string name;
	char text[64];
	bool isSelected = false;
	virtual void RenderMenu() = 0;
public:
	static glm::vec3 GetRotationCenter();
	SceneObject() {};
	SceneObject(const char* _name);
	virtual ~SceneObject() = default;
	virtual bool IsClicable();
	void RenderFullMenu();
	static void Select(int i);
	static void ChangeSelection(int i);
	static void RenderProperties();
	virtual void Render() = 0;
	static std::vector<std::unique_ptr<SceneObject>> SceneObjects;
	static void ItemListMenu();
	static void AddItemMenu();
};

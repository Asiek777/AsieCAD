#pragma once
#include <string>
#include "glm/glm.hpp"
#include "Imgui/imgui.h"
#include <memory>
#include <vector>
#include "shader_s.h"
#include "position.h"


class SceneObject 
{
	static int selected;
	//static SceneObject* selected;
	static int selectedCount;
	static bool rotateAroundCursor;
	//static std::unique_ptr<SceneObject>& GetSelected();
	static Position selectedCenter;
protected:
	std::string name;
	char text[64];
	bool isSelected = false;
	virtual void RenderMenu() = 0;
public:
	static std::vector<std::unique_ptr<SceneObject>> SceneObjects;
	SceneObject() {};
	SceneObject(const char* _name);
	virtual ~SceneObject() = default;
	virtual bool IsClicable();
	virtual void SetSelection(bool _isSelected);
	virtual glm::vec3 GetCenter() = 0;
	virtual void Render() = 0;
	virtual void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange) = 0;
	void RenderFullMenu();
	
	static void RenderScene();
	static void Select(int i);
	static void ChangeSelection(int i);
	static void RenderProperties();
	static void ItemListMenu();
	static void AddItemMenu();
	static glm::vec3 GetRotationCenter();
	static glm::vec3 GetCursorCenter();
};

#pragma once
#include <string>
#include "glm/glm.hpp"
#include "Imgui/imgui.h"
#include <memory>
#include <vector>
#include "shader_s.h"
#include "position.h"
#include "colors.h"


class SceneObject 
{
	static int selected;
	static int selectedCount;
	static bool rotateAroundCursor;
	static Position selectedCenter;
protected:
	char text[64];
	bool isSelected = false;
	virtual void RenderMenu() = 0;
	static glm::mat4 viewProjection;
public:
	std::string name;
	static std::vector<std::shared_ptr<SceneObject>> SceneObjects;
	
	SceneObject() {};
	SceneObject(const char* _name);
	virtual ~SceneObject() = default;
	
	virtual bool IsClicable() { return false; }
	virtual bool IsPoint() { return false; }
	virtual bool IsCurve() { return false; }
	virtual bool IsPointObject() { return false; }
	virtual bool IsDeletable() { return true; }
	virtual char HasChanged() { return 0; }
	
	virtual glm::vec3 GetCenter() = 0;
	virtual void Render() = 0;
	virtual void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange = glm::vec3(1),
		glm::vec3 rotChange = glm::vec3(0)) = 0;
	void RenderFullMenu();
	
	static void SetViewProjectionMatrix(glm::mat4 matrix) { viewProjection = matrix; }
	static void RenderScene();	
	static void DrawMenu();
	
	static void AddPointsToCruve();
	static void AddCurveFromPoints();
	static void RenderProperties();
	static void ItemListMenu();
	static void AddItemMenu();
	
	static void Select(int i);
	static void ChangeSelection(int i);
	
	static glm::vec3 GetRotationCenter();
	static glm::vec3 GetCursorCenter();
};

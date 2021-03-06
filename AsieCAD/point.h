#pragma once
#include "meshBuffer.h"
#include "clickable.h"

class Point :
	public Clicable
{
	static int Number;
	static std::unique_ptr<MeshBuffer> points;
	char hasChanged;
	char undeletableRefs = 0;
	Point(const char* _name);
	static void InitShader();
public:
	static std::unique_ptr<Shader> shader;
	Point(glm::vec3 location);
	Point(float x, float y, float z) : Point(glm::vec3(x,y,z)) {}
	void Render() override;
	void RenderMenu() override;
	void Serialize(tinyxml2::XMLElement* scene) override;
	bool IsPoint() override { return true; }
	bool IsDeletable() override { return !undeletableRefs; }
	char HasChanged() override { return hasChanged; }
	void SetDeletability(bool value) { undeletableRefs += value ? -1 : 1; }
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange = glm::vec3(1), 
		glm::vec3 rotChange = glm::vec3(0)) override;
	static void RenderPoints();
	static void DrawPoint(glm::vec3 position, glm::vec3 color);
	static void DrawManyPoints(std::vector<glm::vec3> coords, glm::vec3 color);
	static void MergePoints();
	static void MergePointsMenu();
	static std::shared_ptr<Point> FakePoint(glm::vec3 location, std::string name);
};


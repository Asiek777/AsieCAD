#pragma once

#include "meshBuffer.h"
#include "sceneObject.h"
#include <fstream>

class Milling : public SceneObject
{
	std::unique_ptr<MeshBuffer> mesh, pathBuffer;
	std::unique_ptr<Shader> shader, pathShader;
	glm::vec3 position = glm::vec3(0.0f);
	int step = 0;
	float t = 0;
	bool isFlat;

	float radius;
	int indexCount;
	std::vector<glm::vec3> path;

	static float speed;
	static bool showPath;
	static void ReadPaths(std::string path);

public:
	Milling(float radius, bool _isFlat);
	~Milling();
	void Render();
	void Update(float dt);
	glm::vec3 GetCenter() override { return glm::vec3(0); }
	void RenderMenu() override {}
	void Serialize(tinyxml2::XMLElement* scene) override {};

	static void MillingFileMenu();
};


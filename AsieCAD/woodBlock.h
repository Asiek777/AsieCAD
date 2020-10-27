#pragma once
#include <memory>
#include <vector>

#include "meshBuffer.h"
#include "shader_s.h"
#include "sceneObject.h"

class WoodBlock : public SceneObject
{
	std::unique_ptr<MeshBuffer> mesh;
	std::unique_ptr<Shader> shader;
	unsigned highMapTex, woodTex;
	std::vector<float> highMap;
	glm::vec3 size = glm::vec3(15,5,15);
	int texWidth, texHeight, indexCount[2];
	
	static int NewTexSize[2];
	static float maxDeep;
	void PrepareBuffers();
public:
	WoodBlock();
	~WoodBlock();
	void Render();
	void UpdateHighMapTexture();
	::std::string UpdateWood(glm::vec3 pos, float radius, bool isFlat, bool UpdateTex);
	glm::vec3 GetCenter() override { return glm::vec3(0); }
	void RenderMenu() override{}	
	void Serialize(tinyxml2::XMLElement* scene) override {};
	bool IsDeletable() override { return false; }

	static void ShowMenu();
};


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
	unsigned highMapTex;
	std::vector<float> highMap;
	glm::vec3 size = glm::vec3(15,5,15);
	int texWidth, texHeight, indexCount;
public:
	WoodBlock(int height, int width);
	~WoodBlock();
	void Render();
	void UpdateWood(glm::vec3 pos, float radius, bool isFlat);
	glm::vec3 GetCenter() override { return glm::vec3(0); }
	void RenderMenu() override{}
	
	void Serialize(tinyxml2::XMLElement* scene) override {};
};


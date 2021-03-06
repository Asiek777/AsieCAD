#pragma once
#include "sceneObject.h"
#include "meshBuffer.h"
#include "clickable.h"
#include "surface.h"

class Cursor :
	public Clicable, public Surface
{
	std::unique_ptr<MeshBuffer> mesh;
	glm::mat4 modelMatrix = glm::mat4(1);
	bool isRotationCenter;
public:
	glm::vec3 GetRotationCenter();
	static std::unique_ptr<Shader> shader;
	Cursor();
	void Render() override;
	void RenderMenu() override;
	void Serialize(tinyxml2::XMLElement* scene) override{}
	bool IsDeletable() override { return false; }
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange) override;

	glm::vec3 GetPointAt(float u, float v) override { return GetCenter(); }
	TngSpace GetTangentAt(float u, float v) override;
};


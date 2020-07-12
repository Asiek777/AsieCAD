#pragma once
#include <memory>
#include "meshBuffer.h"
#include "shader_s.h"
#include "sceneObject.h"
#include "position.h"
#include "surface.h"
# define M_PI  3.14159265358979323846 

class Torus : public SceneObject, public Surface
{
	static int Number;
	unsigned int VBO, VAO;
	int smallCount, bigCount;
	float smallRadius, bigRadius;
	std::unique_ptr<MeshBuffer> mesh;
	Position position;
	void prepareBuffers();
public:
	static std::unique_ptr<Shader> shader;
	Torus(int _smallCircle, int _bigCircle, float _smallRadius, 
		float _bigRadius);
	Torus(tinyxml2::XMLElement* data);
	virtual ~Torus() {};
	void Render() override;
	void RenderMenu() override;
	glm::vec3 GetCenter() override;
	bool IsSurface() override { return true; }
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange) override;
	void Serialize(tinyxml2::XMLElement* scene) override;

	glm::vec3 GetPointAt(float u, float v) override;
	TngSpace GetTangentAt(float u, float v) override;
	bool RollU() override { return true; }
	bool RollV() override { return true; }
};


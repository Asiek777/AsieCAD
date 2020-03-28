#pragma once
#include <memory>
#include "meshBuffer.h"
#include "shader_s.h"
#include "sceneObject.h"
#include "position.h"
# define M_PI  3.14159265358979323846 

class Torus : public SceneObject
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
	virtual ~Torus() {};
	void Render() override;
	void RenderMenu() override;
	glm::vec3 GetCenter() override;
	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange) override;
};


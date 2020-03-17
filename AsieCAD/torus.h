#pragma once
#include <memory>
#include "meshBuffer.h"
#include "shader_s.h"
#include "Imgui/imgui.h"
#include "sceneObject.h"
#include "Position.h"
# define M_PI  3.14159265358979323846 

class Torus : public SceneObject
{
	unsigned int VBO, VAO;
	int smallCount, bigCount;
	float smallRadius, bigRadius;
	std::unique_ptr<MeshBuffer> mesh;
	Position position;
	bool hasChanged = false;
	void prepareBuffers();
public:
	static std::unique_ptr<Shader> shader;
	Torus(int _smallCircle, int _bigCircle, float _smallRadius, 
		float _bigRadius, const char* name );
	virtual ~Torus() {};
	void Render();
	void RenderMenu();
};


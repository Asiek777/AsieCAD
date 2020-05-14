#pragma once
#include "shader_s.h"
#include "sceneObject.h"
#include "camera.h"

class Framebuffers
{
	bool drawStereo;
	bool useRightFilter;
	float eyeDistance;	
	glm::vec3 leftFilter = glm::vec3(0, 1, 1);
	glm::vec3 rightFilter = glm::vec3(1, 0, 0);
	
	unsigned int framebuffer[2], quadVAO, quadVBO, textureColorbuffer[2];
	int screenWidth, screenHeight;
	float nearZ = 1.0f, farZ = 100.0f;
	float projPlaneDist = 50;
	float quadVertices[24] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	std::unique_ptr<Shader> frameShader;
	glm::mat4 frustMatrix(float l, float r, float b, float t, float n, float f);
	glm::mat4 frustrumMatrix(float cameraZoom, float eyeOffset);
public:
	Framebuffers(int _screenWidth, int _screenHeight);
	~Framebuffers();
	glm::mat4 SelectFrustrum(float cameraZoom, float up, float down, float left, float right);
	void RenderScene(Camera& camera, glm::mat4 &viewProjection);
	void DrawMenu();
};


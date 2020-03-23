#pragma once
#include <glad/glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

#include "shader_s.h"
#include "camera.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "torus.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class App
{
	const char* glsl_version = "#version 330 core";
	static App* instance;

	int screenWidth = 1800, screenHeight = 950;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	Camera camera = Camera(glm::vec3(0.f, 0.f, 10.0f));
	float lastX = screenWidth / 2.f, lastY = screenHeight / 2.f;
	bool isCameraRotating = false, isCameraMoving = false;
	glm::mat4 viewProjection;

	void SelectItem();
	void processInput(GLFWwindow *window);
	void setCameraRotate(GLFWwindow* window, bool move);
	void ScreenPosToWorldRay(int mouseX, int mouseY, glm::vec3& out_origin, 
		glm::vec3& out_direction);
	
public:
	App();
	void CreateDefaultScene();
	void setMatrices();
	void setMatricesoShaders();
	static App* GetInstance() { return instance; };
	int Run();

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};


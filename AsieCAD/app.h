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
#include "framebuffers.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class App
{
	const char* glsl_version = "#version 330 core";
	static App* instance;
	GLFWwindow* window;
	std::unique_ptr<Framebuffers> framebuffers;
	
	int screenWidth = 1800, screenHeight = 950;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	Camera camera = Camera(glm::vec3(0.f, 0.f, 10.0f));
	float lastX = screenWidth / 2.f, lastY = screenHeight / 2.f;
	float firstX, firstY;
	bool isCameraRotating = false, isCameraMoving = false, isSelecting = false;
	glm::mat4 viewProjection;

	void SelectItem(bool isPressed);
	void processInput(GLFWwindow *window);
	void setCameraRotate(GLFWwindow* window, bool move);
	void screenPosToWorldRay(int mouseX, int mouseY, glm::vec3& out_origin, 
		glm::vec3& out_direction);
	
public:
	App();
	int Init();
	void DrawMenu();
	void CreateDefaultScene();
	static App* GetInstance() { return instance; };
	int Run();

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};


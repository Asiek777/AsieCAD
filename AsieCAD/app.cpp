#include "app.h"
#include "point.h"
#include "cursor.h"
#include "BezierC0.h"
#include "milling.h"
#include "tinyxml2/tinyxml2.h"
#include "tinyxml2/tinyfiledialogs.h"
#include "toolXML.h"
#include "woodBlock.h"

App* App::instance;

App::App()
{
	instance = this;
}

int App::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(screenWidth, screenHeight, "AsieCAD", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
		
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, ::framebuffer_size_callback);
	glfwSetScrollCallback(window, ::scroll_callback);
	glfwSetCursorPosCallback(window, ::mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
		
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glPointSize(5);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsClassic();

	SceneObject::SceneObjects.emplace_back(std::make_shared<Cursor>());
	CreateDefaultScene();
	framebuffers = std::make_unique<Framebuffers>(screenWidth, screenHeight);
	
	return 0;
}

int App::Run()
{
	auto frameShader = std::make_unique<Shader>("shaders/frame.vert", "shaders/frame.frag");
	
	while (!glfwWindowShouldClose(window)) {

		processInput(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		framebuffers->RenderScene(camera, viewProjection);
		DrawMenu();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void App::DrawMenu()
{
	{
		static float f = 0.0f;

		ImGui::Begin("Settings Window");

		ImGui::SliderFloat("View Angle", &camera.Zoom, 1.0f, 45.0f);

		framebuffers->DrawMenu();
		Milling::ShowMenu();
		if (Milling::millingMode) {
			auto milling = std::static_pointer_cast<Milling>(SceneObject::SceneObjects[2]);
			milling->Update(1.f / ImGui::GetIO().Framerate);
		}
		SceneObject::DrawMenu();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		            1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		if (ImGui::Button("Quit"))
			glfwSetWindowShouldClose(window, true);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void App::CreateDefaultScene()
{
}

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, screenWidth, screenHeight);
}

void App::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	if (isCameraRotating)
		camera.ProcessMouseRotation(xoffset, yoffset, SceneObject::GetRotationCenter());
	if (isCameraMoving)
		camera.ProcessMouseMove(xoffset, yoffset);
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(!ImGui::GetIO().WantCaptureMouse)
		camera.ProcessMouseScroll(yoffset);
}

void App::processInput(GLFWwindow *window) {
	if (!ImGui::GetIO().WantCaptureMouse) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
			setCameraRotate(window, true);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE)
			setCameraRotate(window, false);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			isCameraMoving = true;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
			isCameraMoving = false;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			SelectItem(1);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
			SelectItem(0);
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (!ImGui::GetIO().WantTextInput) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void App::SelectItem(bool isPressed)
{
	ImVec2 mousePos = ImGui::GetMousePos();
	if(ImGui::GetIO().KeyShift) {
		if (!isSelecting && isPressed) {
			firstX = mousePos.x;
			firstY = mousePos.y;
			isSelecting = true;
		}
		else if(isSelecting && !isPressed) {
			float left = ((float)firstX / (float)screenWidth - 0.5f) * 2.0f;
			float up = ((float)(screenHeight - firstY) / (float)screenHeight - 0.5f) * 2.0f;
			float right = ((float)mousePos.x / (float)screenWidth - 0.5f) * 2.0f;
			float down = ((float)(screenHeight - mousePos.y) / (float)screenHeight - 0.5f) * 2.0f;
			glm::mat4 frustrum = framebuffers->SelectFrustrum(camera.Zoom, up, down, left, right)
				* camera.GetViewMatrix();
			SceneObject::Select(-1);
			for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
				if (SceneObject::SceneObjects[i]->IsClicable()) {
					glm::vec4 newPos = glm::vec4(SceneObject::SceneObjects[i]->GetCenter(),1);
					newPos = frustrum * newPos;
					newPos /= newPos.w;
					if (newPos.x <= 1 && newPos.x >= -1 && newPos.y >= -1 && newPos.y <= 1)
						SceneObject::ChangeSelection(i);
					
				}
			isSelecting = false;
		}		
	}
	else if (isPressed) {
		bool ctrlClick = ImGui::GetIO().KeyCtrl;
		glm::vec3 point, dir;
		screenPosToWorldRay(mousePos.x, screenHeight - mousePos.y, point, dir);
		dir = glm::normalize(dir);
		float minDist = 0.3f;
		int minI = -1;
		for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
			if (SceneObject::SceneObjects[i]->IsClicable()) {
				Clicable* clicked = static_cast<Clicable*>(SceneObject::SceneObjects[i].get());
				float dist = clicked->DistanceToLine(point, dir);
				if (dist < minDist) {
					minDist = dist;
					minI = i;
				}
			}
		if (ctrlClick) {
			if (minI >= 0)
				SceneObject::ChangeSelection(minI);
		}
		else {
			if (minI >= 0)
				SceneObject::Select(minI);
			else
				SceneObject::Select(-1);
		}
		isSelecting = false;
	}
}

void App::setCameraRotate(GLFWwindow* window, bool move)
{
	if (isCameraRotating != move)
	{
		isCameraRotating = move;
		if(move)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void App::screenPosToWorldRay(int mouseX, int mouseY, glm::vec3& out_origin, 
	glm::vec3& out_direction)
{
	glm::vec4 lRayStart_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, 
		-1.0,	 1.0f);
	glm::vec4 lRayEnd_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
		0.0,	1.0f);

	glm::mat4 M = glm::inverse(viewProjection);
	glm::vec4 lRayStart_world = M * lRayStart_NDC;
	lRayStart_world /= lRayStart_world.w;
	glm::vec4 lRayEnd_world = M * lRayEnd_NDC;
	lRayEnd_world /= lRayEnd_world.w;

	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);

	out_origin = glm::vec3(lRayStart_world);
	out_direction = glm::normalize(lRayDir_world);
}

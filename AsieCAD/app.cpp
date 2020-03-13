#include "app.h"

App* App::instance;

App::App()
{
	instance = this;
}

int App::Run()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "AsieCAD", NULL, NULL);
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glm::mat4 projection;
	glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsClassic();

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	SceneObject::SceneObjects.emplace_back(std::make_unique<Torus>(50, 50, 1, 4, "torus 1"));
	SceneObject::SceneObjects.emplace_back(std::make_unique<Torus>(50, 50, 1, 6, "torus 2"));
	Shader torusShader("shaders/torus.vert", "shaders/torus.frag");
	torusShader.use();

	while (!glfwWindowShouldClose(window)) {

		processInput(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		torusShader.use();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glm::mat4 view = camera.GetViewMatrix();
		torusShader.setMat4("view", view);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / screenHeight, 0.1f, 100.0f);
		torusShader.setMat4("projection", projection);
		glm::mat4 model(1.f);
		torusShader.setMat4("model", model);
		
		for (int i = 0; i < SceneObject::SceneObjects.size(); i++) {
			SceneObject::SceneObjects[i]->Render(torusShader);
		}

		{
			static float f = 0.0f;

			ImGui::Begin("Settings Window");

			ImGui::SliderFloat("View Angle", &camera.Zoom, 1.0f, 45.0f);

			SceneObject::AddItemMenu();
			SceneObject::ItemListMenu();

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			if (ImGui::Button("Quit"))
				glfwSetWindowShouldClose(window, true);
			ImGui::End();
			ImGui::ShowDemoWindow();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, screenWidth, screenHeight);
}

void App::mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	//if (firstMouse)
	//{
	//	lastX = xpos;
	//	lastY = ypos;
	//	firstMouse = false;
	//}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	if (isCameraRotating)
		camera.ProcessMouseRotation(xoffset, yoffset);
	if (isCameraMoving)
		camera.ProcessMouseMove(xoffset, yoffset);
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
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



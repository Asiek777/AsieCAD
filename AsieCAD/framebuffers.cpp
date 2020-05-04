#include "framebuffers.h"

glm::mat4 Framebuffers::frustMatrix(float l, float r, float b, float t, float n, float f)
{
	glm::mat4 mat = glm::mat4(
		2.f * n / (r - l), 0.f, (r + l) / (r - l), 0.f,
		0.f, 2.f * n / (t - b), (t + b) / (t - b), 0.f,
		0.f, 0.f, (f + n) / (f - n), -2.f * f * n / (f - n),
		0.f, 0.f, 1.f, 0.f
	);
	return mat;
}

Framebuffers::Framebuffers(int _screenWidth, int _screenHeight)
{
	screenHeight = _screenHeight;
	screenWidth = _screenWidth;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenFramebuffers(2, framebuffer);
	glGenTextures(2, textureColorbuffer);
	for (int i = 0; i < 2; ++i) {

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[i]);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _screenWidth, _screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer[i], 0);
	}
	frameShader = std::make_unique<Shader>("shaders/frame.vert", "shaders/frame.frag");
}

Framebuffers::~Framebuffers()
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
}

glm::mat4 Framebuffers::frustrumMatrix(float cameraZoom, float eyeOffset)
{
	float aspect = (float)screenWidth / screenHeight;
	float offset = -eyeOffset / projPlaneDist;
	float n = nearZ;
	float t = n * std::tanf(glm::radians(cameraZoom)/2);
	float b = -t;
	float r = t * aspect + offset;
	float l = -t * aspect + offset;
	return glm::frustum(l, r, b, t, n, farZ);
}

void Framebuffers::RenderScene(Camera &camera)
{
	glEnable(GL_DEPTH_TEST);
	if(drawStereo)
		for (int i = 0; i < 2; ++i) {

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[i]);
			glClearColor(0.f, 0.f, 0.f, 0.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			float eyeOffset = i ? eyeDistance / 2 : -eyeDistance / 2;
			glm::mat4 frust = frustrumMatrix(camera.Zoom, eyeOffset);
			glm::mat4 viewProjection = frust *
				camera.GetViewMatrix(eyeOffset);
			SceneObject::SetViewProjectionMatrix(viewProjection);

			SceneObject::RenderScene();

		}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[0]);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 viewProjection = frustrumMatrix(camera.Zoom, 0) *
			camera.GetViewMatrix(0);
		//glm::mat4 viewProjection = glm::perspective(glm::radians(camera.Zoom),
		//	(float)screenWidth / screenHeight, nearZ, farZ)
		//	* camera.GetViewMatrix(0);
		SceneObject::SetViewProjectionMatrix(viewProjection);

		SceneObject::RenderScene();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer[1]);
	glBindVertexArray(quadVAO);
	frameShader->use();
	frameShader->setVec3("leftFilter", drawStereo ? leftFilter : glm::vec3(1));
	frameShader->setVec3("rightFilter", drawStereo ? rightFilter : glm::vec3(0));	
	frameShader->setInt("frame0", 0);
	frameShader->setInt("frame1", 1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Framebuffers::DrawMenu()
{
	if (ImGui::CollapsingHeader("Stereoscopic options")) {
		ImGui::Checkbox("Enable Stereoscopic 3D", &drawStereo);
		ImGui::DragFloat("Eye distance", &eyeDistance, 0.001f, 0, 1);
		ImGui::DragFloat("Projection plane Distance", &projPlaneDist, 
			1.f, nearZ, farZ);
		
		if (ImGui::Checkbox("Adjust filters indepentently", &useRightFilter))
			rightFilter = glm::vec3(1) - leftFilter;
		if (ImGui::ColorEdit3("Left Filter", &leftFilter.x))
			rightFilter = glm::vec3(1) - leftFilter;
		if (useRightFilter)
			ImGui::ColorEdit3("Right Filter", &rightFilter.x);
		else			
			ImGui::ColorEdit3("Right Filter", &rightFilter.x,
				ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs);
	}
}

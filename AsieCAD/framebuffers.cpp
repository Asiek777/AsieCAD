#include "framebuffers.h"

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
		glGenRenderbuffers(1, rbo + i);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo[i]);
	}
	frameShader = std::make_unique<Shader>("shaders/frame.vert", "shaders/frame.frag");
}

Framebuffers::~Framebuffers()
{
	glDeleteRenderbuffers(2, rbo);
	glDeleteTextures(2, textureColorbuffer);
	glDeleteFramebuffers(2, framebuffer);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
}

glm::mat4 Framebuffers::SelectFrustrum(float cameraZoom, float up, float down, 
	float left, float right)
{
	float aspect = (float)screenWidth / screenHeight;
	float t = nearZ * std::tanf(glm::radians(cameraZoom) / 2);
	float r = t * aspect;
	return glm::frustum(r * left, r * right, t * down, t * up, nearZ, farZ);
}


glm::mat4 Framebuffers::frustrumMatrix(float cameraZoom, float eyeOffset)
{
	float aspect = (float)screenWidth / screenHeight;
	float offset = -eyeOffset / projPlaneDist;
	float t = nearZ * std::tanf(glm::radians(cameraZoom)/2);
	float b = -t;
	float r = t * aspect + offset;
	float l = -t * aspect + offset;
	return glm::frustum(l, r, b, t, nearZ, farZ);
}

void Framebuffers::RenderScene(Camera &camera, glm::mat4& viewProjection)
{
	glEnable(GL_DEPTH_TEST);
	glm::vec3 clear = COLORS::BACKGROUND;
	if(drawStereo)
		for (int i = 0; i < 2; ++i) {

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[i]);
			
			glClearColor(clear.x, clear.y, clear.z, 0.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			float eyeOffset = i ? eyeDistance / 2 : -eyeDistance / 2;
			glm::mat4 frust = frustrumMatrix(camera.Zoom, eyeOffset);
			viewProjection = frust *
				camera.GetViewMatrix(eyeOffset);
			SceneObject::SetViewProjectionMatrix(viewProjection);

			SceneObject::RenderScene();

		}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[0]);
		glClearColor(clear.x, clear.y, clear.z, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		viewProjection = frustrumMatrix(camera.Zoom, 0) *
			camera.GetViewMatrix(0);
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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
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

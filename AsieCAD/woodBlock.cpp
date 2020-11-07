#include "woodBlock.h"
#include "app.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int WoodBlock::NewTexSize[] = { 1000, 1000 };
float WoodBlock::maxDeep = 1.0f;
glm::vec3 WoodBlock::scale = glm::vec3(15, 5, 15);


WoodBlock::WoodBlock() :
	SceneObject("WoodBlock"), texWidth(NewTexSize[0]), texHeight(NewTexSize[1])
{
	std::cout << texWidth << " " << texHeight << "\n";
	shader = std::make_unique<Shader>("shaders/wood.vert", "shaders/phong.frag", "shaders/wood.geom");
	PrepareBuffers();

	highMap = std::vector<float>(texHeight *texWidth);
	for (int i = 0; i < texHeight; i++)
		for (int j = 0; j < texWidth; j++) {
			highMap[i * texWidth + j] = scale.y;
		}
	glGenTextures(1, &highMapTex);
	glBindTexture(GL_TEXTURE_2D, highMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texHeight, 0, GL_RED, GL_FLOAT, highMap.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &woodTex);
	glBindTexture(GL_TEXTURE_2D, woodTex);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("textures/wood.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void WoodBlock::PrepareBuffers()
{
	std::vector<float> vertices;
	std::vector<unsigned> indices;
	float hStep = 1.f / texHeight;
	float wStep = 1.f / texWidth;
	for (int i = 0; i < texHeight + 2; i++)
		for (int j = 0; j < texWidth + 2; j++) {
			float x = glm::clamp(hStep * i - hStep * 0.5f, 0.f, 1.f);
			float z = glm::clamp(wStep * j - wStep * 0.5f, 0.f, 1.f);
			vertices.emplace_back(x);
			vertices.emplace_back(1.0f);
			vertices.emplace_back(z);
		}
	for (int i = 0; i < texHeight + 1; i++)
		for (int j = 0; j < texWidth + 1; j++) {
			indices.emplace_back(i * (texWidth + 2) + j);
			indices.emplace_back((i + 1) * (texWidth + 2) + j);
			indices.emplace_back(i * (texWidth + 2) + j + 1);
			indices.emplace_back(i * (texWidth + 2) + j + 1);
			indices.emplace_back((i + 1) * (texWidth + 2) + j);
			indices.emplace_back((i + 1) * (texWidth + 2) + j + 1);
		}
	int offset = vertices.size() / 3;
	indexCount[0] = indices.size();
	for (int i = 0; i < texHeight + 2; i++) {
			float x = glm::clamp(hStep * i - hStep * 0.5f, 0.f, 1.f);
			float z = glm::clamp(wStep * 0 - wStep * 0.5f, 0.f, 1.f);
			vertices.emplace_back(x);
			vertices.emplace_back(1.0f);
			vertices.emplace_back(z);
			vertices.emplace_back(x);
			vertices.emplace_back(0.0f);
			vertices.emplace_back(z);
		}
	for (int j = 1; j < texWidth + 2; j++) {
		float x = glm::clamp(hStep * (texHeight + 1) - hStep * 0.5f, 0.f, 1.f);
		float z = glm::clamp(wStep * j - wStep * 0.5f, 0.f, 1.f);
		vertices.emplace_back(x);
		vertices.emplace_back(1.0f);
		vertices.emplace_back(z);
		vertices.emplace_back(x);
		vertices.emplace_back(0.0f);
		vertices.emplace_back(z);
	}
	for (int i = texHeight; i >= 0; i--) {
		float x = glm::clamp(hStep * i - hStep * 0.5f, 0.f, 1.f);
		float z = glm::clamp(wStep * (texWidth + 1) - wStep * 0.5f, 0.f, 1.f);
		vertices.emplace_back(x);
		vertices.emplace_back(1.0f);
		vertices.emplace_back(z);
		vertices.emplace_back(x);
		vertices.emplace_back(0.0f);
		vertices.emplace_back(z);
	}
	for (int j = texWidth; j >= 0; j--) {
		float x = glm::clamp(hStep * 0 - hStep * 0.5f, 0.f, 1.f);
		float z = glm::clamp(wStep * j - wStep * 0.5f, 0.f, 1.f);
		vertices.emplace_back(x);
		vertices.emplace_back(1.0f);
		vertices.emplace_back(z);
		vertices.emplace_back(x);
		vertices.emplace_back(0.0f);
		vertices.emplace_back(z);
	}
	for (int i = offset; i < vertices.size() / 3; i++)
		indices.emplace_back(i);
	indices.emplace_back(vertices.size() / 3 - 2);
	indices.emplace_back(vertices.size() / 3 - 2);
	vertices.emplace_back(0.0f);
	vertices.emplace_back(0.0f);
	vertices.emplace_back(0.0f);

	vertices.emplace_back(0.0f);
	vertices.emplace_back(0.0f);
	vertices.emplace_back(1.0f);

	vertices.emplace_back(1.0f);
	vertices.emplace_back(0.0f);
	vertices.emplace_back(0.0f);

	vertices.emplace_back(1.0f);
	vertices.emplace_back(0.0f);
	vertices.emplace_back(1.0f);
	for (int i = 4; i > 0; i--)
		indices.emplace_back(vertices.size() / 3 - i);
	indexCount[1] = indices.size() - indexCount[0];
		
	mesh = std::make_unique<MeshBuffer>(vertices, 1, indices);
}

WoodBlock::~WoodBlock()
{
	glDeleteTextures(1, &highMapTex);
	glDeleteTextures(1, &woodTex);
}

void WoodBlock::Render()
{
	shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, highMapTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTextureEXT(GL_TEXTURE_2D, woodTex);
	glBindVertexArray(mesh->GetVAO());
	shader->setVec3("viewPos", App::GetCameraPos());
	shader->setMat4("viewProjection", viewProjection);
	shader->setInt("tex", 1);
	shader->setInt("useTexture", true);
	glm::vec3 scalling = glm::vec3(scale.x, 1, scale.z);
	glm::mat4 model = model = glm::scale(glm::mat4(1), scalling);
	model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -0.5f));
	shader->setMat4("model", model);
	glDrawElements(GL_TRIANGLES, indexCount[0], GL_UNSIGNED_INT, 0);

	glDrawElements(GL_TRIANGLE_STRIP, indexCount[1], GL_UNSIGNED_INT, 
		(void*)(indexCount[0] * sizeof(GLuint)));
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	
}


::std::string WoodBlock::UpdateWood(glm::vec3 pos, float radius, bool isFlat, bool UpdateTex)
{
	glm::vec2 texCenter = glm::vec2(pos.z / scale.z, pos.x / scale.x) + 0.5f;
	float rH = radius / scale.z * texHeight;
	float rW = radius / scale.x * texWidth;
	int center[2] = { std::round(texCenter.x * texHeight),
		std::round(texCenter.y * texWidth) };
	for (int i = -rH - 1; i <= rH + 1; i++)
		for (int j = -rW - 1; j <= rW + 1; j++)
			if (center[0] + i >= 0 && center[0] + i < texHeight &&
				center[1] + j >= 0 && center[1] + j < texWidth) {
				glm::vec2 coords = glm::vec2(center[0] + i, center[1] + j);
				coords.x /= texHeight;
				coords.y /= texWidth;
				coords.x = (coords.x - 0.5f) * scale.z;
				coords.y = (coords.y - 0.5f) * scale.x;
				glm::vec2 worldPos(pos.z, pos.x);
				float dist = glm::distance(coords, worldPos);
				if (dist < radius) {
					float high = pos.y;
					if(!isFlat)
						high += (radius - sqrtf(radius * radius - dist * dist));
					if(high < maxDeep) {
						UpdateHighMapTexture();
						return "Milling went to deep";
					}
					if (highMap[(center[0] + i) * texWidth + (center[1] + j)] > high) {
						if (isFlat && dist < 0.8 * radius && 
							highMap[(center[0] + i) * texWidth + (center[1] + j)] > high + 0.0001)
							return "Milling with wrong cutter's part";
						highMap[(center[0] + i) * texWidth + (center[1] + j)] = high;
					}
				}
			}
	if (UpdateTex) {
		UpdateHighMapTexture();
	}
	return std::string("");
}

void WoodBlock::UpdateHighMapTexture()
{
	glBindTexture(GL_TEXTURE_2D, highMapTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RED, GL_FLOAT, highMap.data());
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texHeight, 0, 
	//             GL_RED, GL_FLOAT, highMap.data());
	glBindTexture(GL_TEXTURE_2D, 0);
}

void WoodBlock::ShowMenu()
{
	ImGui::DragInt2("HighMap scale", NewTexSize);
	ImGui::DragFloat3("Wood Size", &scale.x, 0.05f, 0);
	ImGui::DragFloat("Maximal deepness", &maxDeep, 0.01f, 0, 5);
	if (ImGui::Button("Reset block")) {
		SceneObjects[1] = std::make_shared<WoodBlock>();
	}
}

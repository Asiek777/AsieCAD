#include "woodBlock.h"

#include "app.h"


WoodBlock::WoodBlock(int _height, int _width) :
	SceneObject("WoodBlock"), texWidth(_width), texHeight(_height)
{
	shader = std::make_unique<Shader>("shaders/wood.vert", "shaders/phong.frag", "shaders/wood.geom");
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
			vertices.emplace_back(x);
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
	indexCount = indices.size();
	mesh = std::make_unique<MeshBuffer>(vertices, 1, indices, "TODO");

	highMap = std::vector<float>(texHeight *texWidth);
	for (int i = 0; i < texHeight; i++)
		for (int j = 0; j < texWidth; j++) {
			highMap[i * texWidth + j] = size.y;
		}
	glGenTextures(1, &highMapTex);
	glBindTexture(GL_TEXTURE_2D, highMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texHeight, 0, GL_RED, GL_FLOAT, highMap.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	
}

WoodBlock::~WoodBlock()
{
	glDeleteTextures(1, &highMapTex);
}

void WoodBlock::Render()
{
	shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, highMapTex);
	glBindVertexArray(mesh->GetVAO());
	shader->setVec3("color", glm::vec3(1.0f));
	shader->setVec3("viewPos", App::GetCameraPos());
	shader->setMat4("viewProjection", viewProjection);
	glm::mat4 model = model = glm::scale(glm::mat4(1), glm::vec3(15, 1, 15));
	model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -0.5f));
	shader->setMat4("model", model);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	
}

void WoodBlock::UpdateWood(glm::vec3 pos, float radius, bool isFlat)
{
	float r = radius / size.x;
	glm::vec2 texCenter = glm::vec2(pos.z, pos.x) / size.x + 0.5f;
	float rH = r * texHeight;
	float rW = r * texWidth;
	int center[2] = { std::round(texCenter.x * texHeight),
		std::round(texCenter.y * texWidth) };
	for (int i = -rH - 1; i <= rH + 1; i++)
		for (int j = -rW - 1; j <= rW + 1; j++)
			if (center[0] + i >= 0 && center[0] + i < texHeight &&
				center[1] + j >= 0 && center[1] + j < texWidth) {
				glm::vec2 coords = glm::vec2(center[0] + i, center[1] + j);
				coords.x /= texHeight;
				coords.y /= texWidth;
				float dist = glm::distance(coords, texCenter);
				if (dist < r) {
					float high = pos.y;
					if(!isFlat)
						high += (r - sqrtf(r * r - dist * dist)) * size.x;					
					if (highMap[(center[0] + i) * texWidth + (center[1] + j)] > high)
						highMap[(center[0] + i) * texWidth + (center[1] + j)] = high;
				}
			}		
	glBindTexture(GL_TEXTURE_2D, highMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texHeight, 0, GL_RED, GL_FLOAT, highMap.data());
	glBindTexture(GL_TEXTURE_2D, 0);
}

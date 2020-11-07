#include "pathGenerator.h"

#include "sceneObject.h"
#include "surface.h"
#include "stb_image.h"

PathGenerator::PathGenerator()
{
	highMap = std::vector<std::vector<float>>(MAP_SIZE);
	for (int i = 0; i < MAP_SIZE; i++)
		highMap[i] = std::vector<float>(MAP_SIZE);
}

void PathGenerator::PrepareFatPaths()
{
	for (int i = 0; i < MAP_SIZE; i++)
		for (int j = 0; j < MAP_SIZE; j++)
			highMap[i][j] = 0;
	for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
		if (SceneObject::SceneObjects[i]->IsPoint()) {
			glm::vec3 pos = SceneObject::SceneObjects[i]->GetCenter();
			max.x = std::max(pos.x, max.x);
			max.y = std::max(pos.y, max.y);
			min.x = std::min(pos.x, min.x);
			min.y = std::min(pos.y, min.y);
		}
	for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
		if (SceneObject::SceneObjects[i]->IsSurface()) {
			auto surface = Surface::SceneObjectToSurface(
				SceneObject::SceneObjects[i]);
			int size = 300;
			for (int j = 0; j <= size; j++)
				for (int k = 0; k <= size; k++) {
					float u = j / (float)size, v = k / (float)size;
					glm::vec3 pos = surface->GetPointAt(u, v);
					//SceneObject::SceneObjects.emplace_back(std::make_shared<Point>(pos));
					int x = std::round((pos.x - min.x) / (max.x - min.x) * (MAP_SIZE - 1));
					int y = std::round((pos.y - min.y) / (max.y - min.y) * (MAP_SIZE - 1));
					highMap[x][y] = std::max(highMap[x][y], pos.z);
				}
		}
	float x, y;
	file = std::ofstream("test.k16");
	for (int i = 0; i < MAP_SIZE; i++)
		for (int j = i % 2 ? 0 : MAP_SIZE - 1; i % 2 ? j < MAP_SIZE : j >= 0; i % 2 ? j++ : j--) {
			float z = GetHighAt(i, j);
			if (z > 0) {
				std::string line;
				x = i / (float)(MAP_SIZE - 1) * 15.f;
				y = j / (float)(MAP_SIZE - 1) * 15.f;
				line = "N" + std::to_string(step) + "G01X" + std::to_string(x) +
					"Y" + std::to_string(y) + "Z" + std::to_string(z);
				file << line << std::endl;
				step++;
			}
		}
}

float PathGenerator::GetHighAt(int x, int y)
{
	float max = 0;
	for (int i = -5; i <= 5; i++)
		for (int j = -5; j <= 5; j++)
			if (x + i >= 0 && x + i < MAP_SIZE && y + j >= 0 && y + j < MAP_SIZE)
				max = std::max(highMap[x + i][y + j], max);
	return max;		
}

void PathGenerator::ShowMenu()
{
	if (ImGui::Button("CalculatePaths"))
		PrepareFatPaths();
}

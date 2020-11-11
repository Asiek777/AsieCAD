#include "pathGenerator.h"

#include "sceneObject.h"
#include "surface.h"
#include "stb_image.h"

void PathGenerator::PreparePaths()
{
	Initialize();
	PrepareFatPaths();
}

void PathGenerator::Initialize()
{
	highMap = std::vector<std::vector<float>>(MAP_SIZE);
	for (int i = 0; i < MAP_SIZE; i++)
		highMap[i] = std::vector<float>(MAP_SIZE);
	for (int i = 0; i < MAP_SIZE; i++)
		for (int j = 0; j < MAP_SIZE; j++)
			highMap[i][j] = 0;
	for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
		if (SceneObject::SceneObjects[i]->IsPoint()) {
			glm::vec3 pos = SceneObject::SceneObjects[i]->GetCenter();
			_max.x = std::max(pos.x, _max.x);
			_max.y = std::max(pos.y, _max.y);
			_min.x = std::min(pos.x, _min.x);
			_min.y = std::min(pos.y, _min.y);
		}
	float baseScale = 0.9f;
	scale = std::max(_max.x - _min.x, _max.y - _min.y) / baseScale;
	offset = (_max + _min - scale) / 2.f;
}

void PathGenerator::PrepareFatPaths()
{
	Initialize();
	
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
					int x = std::round((pos.x - offset.x) / scale * (MAP_SIZE - 1));
					int y = std::round((pos.y - offset.y) / scale * (MAP_SIZE - 1));
					highMap[x][y] = std::max(highMap[x][y], pos.z);
				}
		}
	float x, y;
	for (int i = - 50; i < MAP_SIZE + 50; i += 41)
		for (int j = i % 2 ? - 50 : MAP_SIZE + 50 - 1; i % 2 ? j < MAP_SIZE + 50 :
			j >= - 50; i % 2 ? j += 1 : j -= 1) {
			float z = GetHighAt(i, j);
			
			x = i / (float)(MAP_SIZE - 1);
			y = j / (float)(MAP_SIZE - 1);
			path.emplace_back(glm::vec3(x, y, z));
		}
	SavePathToFile("paths/test.k16");
}

float PathGenerator::GetHighAt(int x, int y)
{
	float radius = (MAP_SIZE - 1) / 20.f;
	float max = 0.2;
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
			if (x + i >= 0 && x + i < MAP_SIZE && y + j >= 0 && 
				y + j < MAP_SIZE && i * i + j * j < radius * radius){
				float dist = i * i + j * j;
				float high = highMap[x + i][y + j] + 0.2 +
					(radius - std::max(sqrtf(radius * radius - dist), 0.f)) / radius * 0.1f;
				max = std::max(high, max);
			}
	return max;
}

void PathGenerator::SavePathToFile(std::string filename)
{
	step = 3;
	file = std::ofstream(filename);
	float maxZ = 7;
	for (int j = 0; j < 10; j++)
		for (int i = 0; i < path.size() - 2; i++) {
			glm::vec3 a = glm::normalize(path[i + 1] - path[i]);
			glm::vec3 b = glm::normalize(path[i + 2] - path[i]);
			float cos = glm::dot(a, b);
			if (cos > 0.99999)
				path.erase(path.begin() + i + 1);
		}
	AddToFile(0.5, 0.5, maxZ);
	AddToFile(path[0] + glm::vec3(0, 0, maxZ));
	for (int i = 0; i < path.size(); i++)
		AddToFile(path[i]);	
	AddToFile(path.back() + glm::vec3(0, 0, maxZ));
	AddToFile(0.5, 0.5, maxZ);
}

void PathGenerator::AddToFile(float x, float y, float z)
{
	x = (x - 0.5f) * 150;
	y = (y - 0.5f) * 150;
	z = z / scale * 150 + 15;
	std::string line = "N" + std::to_string(step) + "G01X" + std::to_string(x) +
		"Y" + std::to_string(y) + "Z" + std::to_string(z);
	file << line << std::endl;
	step++;
}

void PathGenerator::ShowMenu()
{
	if (ImGui::Button("Calculate paths"))
		PreparePaths();
}

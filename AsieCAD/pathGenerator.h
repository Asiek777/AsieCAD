#pragma once
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

class PathGenerator
{
	const int MAP_SIZE= 1000;
	int step = 3;
	//float min, max;
	float scale;
	glm::vec2 offset;
	glm::vec2 _min = glm::vec2(100), _max = glm::vec2(-100);
	std::vector<std::vector<float>> highMap;
	std::vector<glm::vec3> path;
	std::ofstream file;

	void PrepareFatPaths();
	float GetHighAt(int x, int y);
	void SavePathToFile(std::string filename);
	void AddToFile(float x, float y, float z);
	void AddToFile(glm::vec3 pos) { return AddToFile(pos.x, pos.y, pos.z); }
	void PreparePaths();
	
public:
	void Initialize();
	void ShowMenu();
};


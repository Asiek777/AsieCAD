#pragma once
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

class PathGenerator
{
	const int MAP_SIZE= 1000;
	int step = 3;
	glm::vec2 min = glm::vec2(100, 100), max = glm::vec2(-100, -100);
	std::vector<std::vector<float>> highMap;
	std::ofstream file;

	float GetHighAt(int x, int y);
	void PrepareFatPaths();
public:
	PathGenerator();
	void ShowMenu();
};


#pragma once
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

#include "surface.h"

class PathGenerator
{
	const int MAP_SIZE= 1000;
	int step;
	//float min, max;
	float scale;
	glm::vec2 offset;
	glm::vec2 _min = glm::vec2(100), _max = glm::vec2(-100);
	std::vector<std::vector<float>> highMap;
	std::vector<glm::vec3> path;
	std::ofstream file;

	void Initialize();
	void PreparePaths();
	void PrepareFatPaths();
	int SurfaceSize(std::shared_ptr<Surface>& surface);
	float GetHighAt(int x, int y);
	void PrepareExactPaths();
	void ReducePath(std::vector<glm::vec3>& reducedPath);
	void SavePathToFile(std::string filename, float minZ = 0, float maxZ = 7);
	void AddToFile(float x, float y, float z);
	void AddToFile(glm::vec3 pos) { return AddToFile(pos.x, pos.y, pos.z); }
	
public:
	void ShowMenu();
};


#pragma once
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <algorithm>

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
	void PrepareFlatPaths();
	void ReducePath(std::vector<glm::vec3>& reducedPath);
	void SavePathToFile(std::string filename, float minZ = 0, float maxZ = 7);
	void AddToFile(float x, float y, float z);
	void AddToFile(glm::vec3 pos) { return AddToFile(pos.x, pos.y, pos.z); }
	
public:
	void ShowMenu();
};

struct Izolines {
	std::vector<std::vector<float>> lines;
	int izolineCount;
	Izolines(int _izolineCount) :izolineCount(_izolineCount), lines(_izolineCount)
	{}
	void AddIzolines(std::vector<std::vector<float>> newLines)
	{
		for (int i = 0; i < izolineCount; i++) {
			lines[i].insert(lines[i].end(), newLines[i].begin(), newLines[i].end());
			std::sort(lines[i].begin(), lines[i].end());
			for (int j = 0; j < lines[i].size() - 1;) {
				if (lines[i].size() == 0)
					break;
				if (lines[i][j] == lines[i][j + 1])
					lines[i].erase(lines[i].begin() + j, lines[i].begin() + j + 2);
				else
					j++;
			}
		}
	}
};
#include "pathGenerator.h"



#include <iosfwd>
#include <iosfwd>
#include <vector>
#include <vector>
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>



#include "bezierPatch.h"
#include "OffsetSurface.h"
#include "sceneObject.h"
#include "surface.h"
#include "stb_image.h"

void PathGenerator::PreparePaths()
{
	Initialize();
	PrepareFatPaths();
	PrepareExactPaths();
	PrepareCircuitPaths();
	PrepareFlatPaths();
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
	for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
		if (SceneObject::SceneObjects[i]->IsSurface()) {
			auto surface = Surface::SceneObjectToSurface(
				SceneObject::SceneObjects[i]);
			int size = SurfaceSize(surface) * 6;
			std::cout << size << std::endl;
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
	for (int i = MAP_SIZE * -.07f; i < MAP_SIZE * 1.07; i += 75) {
		for (int j = i % 2 ? MAP_SIZE * -.07f : MAP_SIZE * 1.07 - 1; i % 2 ? j < MAP_SIZE * 1.07 :
			j >= MAP_SIZE * -.07f; i % 2 ? j += 1 : j -= 1) {
			float z = GetHighAt(i, j);

			x = i / (float)(MAP_SIZE - 1);
			y = j / (float)(MAP_SIZE - 1);
			path.emplace_back(glm::vec3(x, y, z));
		}
	}
	SavePathToFile("paths/test.k16", 2.5f);
}

int PathGenerator::SurfaceSize(std::shared_ptr<Surface>& surface)
{
	glm::vec3 pos1 = surface->GetPointAt(0, 0);
	glm::vec3 pos2 = surface->GetPointAt(0.25f, 0.25f);
	glm::vec3 pos3 = surface->GetPointAt(0.5f, 0.5f);
	return std::max((int)((glm::distance(pos1, pos2) + glm::distance(pos2, pos3)) * 5), 50);
}

float PathGenerator::GetHighAt(int x, int y)
{
	float radius = (MAP_SIZE - 1) / 15.f;
	float max = 0.2;
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
			if (x + i >= 0 && x + i < MAP_SIZE && y + j >= 0 &&
				y + j < MAP_SIZE && i * i + j * j < radius * radius) {
				float dist = i * i + j * j;
				float high = highMap[x + i][y + j] +0.2 -
					(radius - sqrtf(radius * radius - dist)) / radius * 0.8f;
				max = std::max(high, max);
			}
	return max;
}

void PathGenerator::PrepareExactPaths()
{
	path.clear();
	float radius = 0.4f;
	std::vector<std::shared_ptr<Surface>> surfaces;
	std::vector<std::shared_ptr<OffsetSurface>> offsetSurfaces;
	std::vector<Izolines> izolines;
	for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
		if (SceneObject::SceneObjects[i]->IsSurface()) 
			surfaces.emplace_back(Surface::SceneObjectToSurface(
				SceneObject::SceneObjects[i]));
	
	for (int i = 0; i < surfaces.size(); i++) {
		offsetSurfaces.emplace_back(std::make_shared<OffsetSurface>(surfaces[i], radius));
		izolines.emplace_back(SurfaceSize(surfaces[i]) * 4);
	}
	Surface::beginFromCursor = true;
	const int interSize = 6;
	struct inter{ int index[2]; glm::vec3 cursorPos; } inters[interSize] = {
		{{0,1}, glm::vec3(12, -0.7, 2.0)},
		{{0,1}, glm::vec3(12, 0.7, 2.0)},
		{{0,2}, glm::vec3(8.6, 2, 1.6)},
		{{0,4}, glm::vec3(3.24, 3.22, -1.3)},
		{{0,4}, glm::vec3(7.46, 3.44, -2.5)},
		{{0,5}, glm::vec3(5.4, -3.74, -0.9)},
	};
	
	for (int i = 0; i < interSize; i++) {
		int index = inters[i].index[1];
		SceneObject::SceneObjects[0]->UpdatePosition(inters[i].cursorPos);
		auto end = SceneObject::SceneObjects.size();
		Surface::FindIntersection(offsetSurfaces[0], offsetSurfaces[index]);
		
		auto intersectionCurve = std::dynamic_pointer_cast<IntersectionCurve>(
			SceneObject::SceneObjects.back());
		if (SceneObject::SceneObjects.size() > end) {
			end = SceneObject::SceneObjects.size();
			auto interpolationCurve = intersectionCurve->MakeInterpolatedCurve();

			for (auto j = end; j < SceneObject::SceneObjects.size(); j++) {
				glm::vec3 pos = SceneObject::SceneObjects[j]->GetCenter();
				if (pos.z >= radius)
					path.emplace_back(glm::vec3((pos.x - offset.x) / scale,
						(pos.y - offset.y) / scale, pos.z - radius));
			}
			path.emplace_back(glm::vec3(-1));
			for (int j = 0; j < 2; j++) {
				int lineCount = SurfaceSize(surfaces[inters[i].index[j]]) * 4;
				izolines[inters[i].index[j]].AddIzolines(intersectionCurve->CalcTrimming(
					lineCount + 1, false, !j));
			}
			
			SceneObject::SceneObjects.erase(SceneObject::SceneObjects.begin()+end, 
				SceneObject::SceneObjects.end());
		}
	}	
	SceneObject::SceneObjects[0]->UpdatePosition( glm::vec3(0.f));

	for (int k = 0; k < surfaces.size(); k++) {
		int stepCount = SurfaceSize(surfaces[k]) * 4;
		float step = 1.f / stepCount;
		
		for (int i = 0; i < stepCount; i++){
			int lineIt = 0, lastLineIt = 0;
			std::vector<float>& line = izolines[k].lines[i];
			for (int j = 0; j < stepCount;j++) {
				while (lineIt < line.size() && j * step > line[lineIt])
					lineIt++;
				if(lineIt > lastLineIt)
					path.emplace_back(glm::vec3(-1));
				lastLineIt = lineIt;

				if (lineIt % 2 == 0 ^ k >= 2) {
					auto tangent = offsetSurfaces[k]->GetTangentAt(j * step, i * step);
					glm::vec3 pos = tangent.pos;
					//if (k == 2)
					//	SceneObject::SceneObjects.emplace_back(std::make_shared<Point>(tangent.pos));
					if (pos.z > radius && tangent.normal.z > 0)
						path.emplace_back(glm::vec3((pos.x - offset.x) / scale,
							(pos.y - offset.y) / scale, pos.z - radius));
				}
			}
			if (path.back() != glm::vec3(-1))
				path.emplace_back(glm::vec3(-1));
		}
	}
	SavePathToFile("paths/test.k08", 0, 3);
}

void PathGenerator::PrepareCircuitPaths()
{
	path.clear();
	float radius = 0.5f;
	const int circuitCount = 10;
	struct { int index; bool constU; float param; bool isReversed; } circuits[circuitCount] = {
		{0, 1, 0, 1},
		{0, 0, 1.f / 6, 0},
		{0, 0, 4.f / 6, 1},
		{1, 0, 0.1, 0},
		{1, 0, 0.6, 1},
		{2, 0, 0.5, 0},
		{4, 0, 0.2, 0},
		{5, 0, 1.f / 6, 0},
		{5, 1, 1, 0},
		{5, 0, 4.f / 6, 1},

	};
	std::vector<glm::vec2> offsets[circuitCount];
	
	std::vector<std::shared_ptr<Surface>> surfaces;
	for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
		if (SceneObject::SceneObjects[i]->IsSurface())
			surfaces.emplace_back(Surface::SceneObjectToSurface(
				SceneObject::SceneObjects[i]));

	for (int i = 0; i < circuitCount; i++) {
		auto surface = surfaces[circuits[i].index];
		int len = SurfaceSize(surface) * 4;
		if (!circuits[i].constU) {
			float v = circuits[i].param;
			for (int j = 0; j <= len; j++) {
				TngSpace tangent = surface->GetTangentAt(j / (float)len, v);
				glm::vec2 pos = glm::vec2(tangent.pos.x, tangent.pos.y);
				glm::vec2 normal = glm::vec2(tangent.normal.x, tangent.normal.y);
				normal = glm::normalize(normal) * radius;
				if (!std::isnan(normal.x))
					offsets[i].emplace_back(pos + normal);
			}
		}
		else {
			float u = circuits[i].param;
			for (int j = 1; j < len; j++) {
				TngSpace tangent = surface->GetTangentAt(u, j / (float)len / 2 + 1 / 6.f);
				glm::vec2 pos = glm::vec2(tangent.pos.x, tangent.pos.y);
				glm::vec2 normal = glm::vec2(tangent.diffV.y, -tangent.diffV.x);
				if (i > 3)
					normal = -normal;
				normal = glm::normalize(normal) * radius;
				if (!std::isnan(normal.x))
					offsets[i].emplace_back(pos + normal);
			}
		}
		if (circuits[i].isReversed)
			std::reverse(offsets[i].begin(), offsets[i].end());
	}

	path.emplace_back(glm::vec3((glm::vec2(-4, 0) - offset) / scale, 1));
	for (int j = 0; j < offsets[0].size() - 1; j++)
		path.emplace_back(glm::vec3((offsets[0][j] - offset) / scale, 1));
	path.emplace_back(glm::vec3(-1));
	
	std::vector<glm::vec2> ringRoad{ glm::vec2(-4,0), glm::vec2(-1,0) };
	const int partsLen = 16;
	int parts[partsLen] = { 0,1,5,1,6,1,3,4,4,3,2,7,8,9,2,0 };
	int start = 0;
	for (int i = 0; i < partsLen - 1; i++) {
		auto& current = offsets[parts[i]];
		auto& next = offsets[parts[i + 1]];
		for (int j = start; j < current.size()-1; j++) {
			ringRoad.emplace_back(current[j]);
			bool isIntersecting = false;
			for (int k = 0; k < next.size() - 1; ++k) {
				glm::vec2 q = intersection(current[j], current[j + 1], next[k], next[k + 1]);
				if (isBeetween(current[j], current[j + 1], q) &&
					isBeetween(next[k], next[k + 1], q)) {
					ringRoad.emplace_back(q);
					start = k + 1;
					isIntersecting = true;
					break;
				}
			}
			if (isIntersecting)
				break;
			start = 0;
			if(j == current.size() - 2) {
				ringRoad.emplace_back(current[j]);
				std::vector<glm::vec2> detour = CalcDetour(current[j], current[j + 1], next[0], radius);
				ringRoad.insert(ringRoad.end(), detour.begin(), detour.end());
			}
		}
	}
	for (int i = 0; i < ringRoad.size(); i++)
		path.emplace_back(glm::vec3((ringRoad[i] - offset) / scale, 0));
	SavePathToFile("paths/test.f10", 0, 2.5);
	
}

std::vector<glm::vec2> PathGenerator::CalcDetour(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float radius)
{
	std::vector<glm::vec2> result;
	p1 = p1 - p2;
	p1 = glm::vec2(-p1.y, p1.x);
	p1 = glm::normalize(p1) * radius;
	p1 = p2 + p1;
	float a1 = std::atan2f(p2.y - p1.y, p2.x - p1.x);
	float a2 = std::atan2f(p3.y - p1.y, p3.x - p1.x);
	if (a2 > a1)
		a2 -= 2 * glm::pi<float>();
	for (float a = a1; a > a2; a -= 0.02f)
		result.emplace_back(p1 + radius * glm::vec2(std::cosf(a), std::sinf(a)));
	return result;	
}

glm::vec2 PathGenerator::intersection(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4)
{
	float div = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
	float a = p1.x * p2.y - p1.y * p2.x;
	float b = p3.x * p4.y - p3.y * p4.x;
	float x = (a * (p3.x - p4.x) - b * (p1.x - p2.x)) / div;
	float y = (a * (p3.y - p4.y) - b * (p1.y - p2.y)) / div;
	return glm::vec2(x, y);
}

bool PathGenerator::isBeetween(glm::vec2 p1, glm::vec2 p2, glm::vec2 q)
{
	float eps = 0.000001;
	if (std::isnan(q.x))
		return false;
	if (std::max(p1.x, p2.x) - q.x < -eps)
		return false;
	if (std::min(p1.x, p2.x) - q.x > eps)
		return false;
	if (std::max(p1.y, p2.y) - q.y < -eps)
		return false;
	if (std::min(p1.y, p2.y) - q.y > eps)
		return false;
	return true;
}

void PathGenerator::PrepareFlatPaths()
{
	path.clear();
	float x, y;
	for (int i = MAP_SIZE * -.07f; i < MAP_SIZE * 1.07; i += 37) {
		std::vector<glm::vec3> oneLine;
		for (int j =  MAP_SIZE * 1.07 - 1; 	j >= MAP_SIZE * .5f; j -= 1) {
			if (!IsColliding(i, j)) {
				x = i / (float)(MAP_SIZE - 1);
				y = j / (float)(MAP_SIZE - 1);
				oneLine.emplace_back(glm::vec3(x, y, 0));
			}
			else
				break;
		}
		if (i % 2)
			std::reverse(oneLine.begin(), oneLine.end());
		path.insert(path.end(), oneLine.begin(), oneLine.end());
	}
	path.emplace_back(glm::vec3(0.96f, 0.54f, 0));
	path.emplace_back(glm::vec3(0.96f, 0.48f, 0));
	path.emplace_back(glm::vec3(-1));
	for (int i = MAP_SIZE * -.07f; i < MAP_SIZE * 1.07; i += 37) {
		std::vector<glm::vec3> oneLine;
		for (int j = MAP_SIZE * -.07; j <= MAP_SIZE * .5f; j += 1) {
			if (!IsColliding(i, j)) {
				x = i / (float)(MAP_SIZE - 1);
				y = j / (float)(MAP_SIZE - 1);
				oneLine.emplace_back(glm::vec3(x, y, 0));
			}
			else
				break;
		}
		if (i % 2)
			std::reverse(oneLine.begin(), oneLine.end());
		path.insert(path.end(), oneLine.begin(), oneLine.end());
	}
	SavePathToFile("paths/test2.f10", 0);
}

bool PathGenerator::IsColliding(int x, int y)
{
	float radius = (MAP_SIZE - 1) / 20.f;
	for (int i = -radius*0.7; i <= radius*0.7; i++)
		for (int j = -radius; j <= radius; j++)
			if (x + i >= 0 && x + i < MAP_SIZE && y + j >= 0 &&
				y + j < MAP_SIZE && i * i + j * j < radius * radius) {
				float high = highMap[x + i][y + j];
				if (high > 0)
					return true;
			}
	return false;
}

void PathGenerator::ReducePath(std::vector<glm::vec3>& reducedPath)
{
	for (int j = 0; j < 12; j++)
		for (int i = 0; i < reducedPath.size() - 2; i++) {
			glm::vec3 a = glm::normalize(reducedPath[i + 1] - reducedPath[i]);
			glm::vec3 b = glm::normalize(reducedPath[i + 2] - reducedPath[i]);
			float cos = glm::dot(a, b);
			if (cos > 0.999999)
				reducedPath.erase(reducedPath.begin() + i + 1);
		}
}

void PathGenerator::SavePathToFile(std::string filename, float minZ, float maxZ)
{
	step = 3;
	file = std::ofstream(filename);
	ReducePath(path);
	if (minZ > 0) {
		std::vector<glm::vec3> higherPath(path);
		for (int i = 0; i < higherPath.size(); i++)
			higherPath[i].z = std::max(higherPath[i].z, minZ);
		ReducePath(higherPath);
		AddToFile(0.5, 0.5, maxZ);
		AddToFile(higherPath[0].x, higherPath[0].y, maxZ);
		for (int i = 0; i < higherPath.size(); i++)
			if (higherPath[i].z >= 0)
				AddToFile(higherPath[i]);
			else {
				if (higherPath[i - 1].z >= 0)
					AddToFile(higherPath[i - 1].x, higherPath[i - 1].y, maxZ);
				if (higherPath[i + 1].z >= 0)
					AddToFile(higherPath[i + 1].x, higherPath[i + 1].y, maxZ);
			}
		
		AddToFile(higherPath.back().x, higherPath.back().y, maxZ);
		
	}
	else
		AddToFile(0.5, 0.5, maxZ);
	AddToFile(path[0].x, path[0].y, maxZ);
	for (int i = 0; i < path.size(); i++)
		if (path[i].z >= 0)
			AddToFile(path[i]);
		else if (i + 1 < path.size() && i > 0) {
			if (path[i - 1].z >= 0)
				AddToFile(path[i - 1].x, path[i - 1].y, maxZ);
			if (path[i + 1].z >= 0)
				AddToFile(path[i + 1].x, path[i + 1].y, maxZ);
		}
	AddToFile(path.back().x, path.back().y, maxZ);
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

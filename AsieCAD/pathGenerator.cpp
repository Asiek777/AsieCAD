#include "pathGenerator.h"



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
	Initialize();
	
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
			j >= -50; i % 2 ? j += 1 : j -= 1) {
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
	return (int)((glm::distance(pos1, pos2) + glm::distance(pos2, pos3)) * 5);
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
				float high = highMap[x + i][y + j]; +0.2 -
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
	std::vector<std::shared_ptr<OffsetSurface>> inboxSurfaces;
	std::vector<Izolines> izolines;
	for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
		if (SceneObject::SceneObjects[i]->IsSurface()) 
			surfaces.emplace_back(Surface::SceneObjectToSurface(
				SceneObject::SceneObjects[i]));
	
	for (int i = 0; i < surfaces.size(); i++) {
		inboxSurfaces.emplace_back(std::make_shared<OffsetSurface>(surfaces[i], radius));
		izolines.emplace_back(SurfaceSize(surfaces[i]) * 4);
	}
	Surface::beginFromCursor = false;
	const int interSize = 5;
	struct inter{ int index[2]; glm::vec3 cursorPos; } inters[interSize] = {
		{{0,1}, glm::vec3(12, -0.7, 2.0)},
		{{0,1}, glm::vec3(12, 0.7, 2.0)},
		{{0,2}, glm::vec3(8.6, 2, 1.6)},
		{{0,4}, glm::vec3(3.6, 3.0, 1.1)},
		//{{0,4}, glm::vec3(8.2, 3.0, 1.1)},
		//{{0,4}, glm::vec3(8.78, 3.0, 0.82)},
		//{{0,4}, glm::vec3(9.0, 3.0, 0.34)},
		{{0,5}, glm::vec3(5.4, -3.74, 0.9)},
		//{{0,5}, glm::vec3(3.3, -3.6, 0.5)},
		//{{0,5}, glm::vec3(7.34, -3.94, 0.5)}
	};
	
	for (int i = 0; i < interSize; i++) {
		int index = inters[i].index[1];
		SceneObject::SceneObjects[0]->UpdatePosition(inters[i].cursorPos);
		auto end = SceneObject::SceneObjects.size();
		Surface::FindIntersection(inboxSurfaces[0], inboxSurfaces[index]);
		
		auto intersectionCurve = std::dynamic_pointer_cast<IntersectionCurve>(
			SceneObject::SceneObjects.back());
		if (SceneObject::SceneObjects.size() > end) {
			end = SceneObject::SceneObjects.size();
			auto interpolationCurve = intersectionCurve->MakeInterpolatedCurve();

			for (auto j = end; j < SceneObject::SceneObjects.size(); j++) {
				glm::vec3 pos = SceneObject::SceneObjects[j]->GetCenter();
				if (pos.z > radius)
					path.emplace_back(glm::vec3((pos.x - offset.x) / scale,
						(pos.y - offset.y) / scale, pos.z - 0.4f));
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
					auto tangent = surfaces[k]->GetTangentAt(j * step, i * step);
					glm::vec3 pos = tangent.normal * 0.4f + tangent.pos;
					if (pos.z > 0.4f && tangent.normal.z > 0)
						path.emplace_back(glm::vec3((pos.x - offset.x) / scale,
							(pos.y - offset.y) / scale, pos.z - 0.4f));
				}
			}
			if (path.back() != glm::vec3(-1))
				path.emplace_back(glm::vec3(-1));
		}
	}
	SavePathToFile("paths/test.k08", 0, 3);
}

void PathGenerator::PrepareFlatPaths()
{
	std::vector<std::shared_ptr<Point>> flatPoints;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			flatPoints.emplace_back(std::make_shared<Point>(
				((_min.x - 1) * (3 - i) + (_max.x + 1) * i) / 3,
				((_min.y - 1) * (3 - j) + (_max.y + 1) * j) / 3, 0));
	auto floor = std::make_shared<BezierPatch>(flatPoints, false);
	//SceneObject::SceneObjects.emplace_back(floor);
	//SceneObject::SceneObjects.insert(SceneObject::SceneObjects.end(), 
	//	flatPoints.begin(), flatPoints.end());
	const int circuitSize = 10;
	struct { int index; bool constU; float param; } circuits[circuitSize] = {
		{0, 0, 0.16685},
		{0, 0, 0.66685},
		{0, 1, 0},
		{1, 0, 0.6},
		{1, 0, 0.1},
		{2, 0, 0.5},
		{4, 0, 0.2},
		{5, 0, 0.16685},
		{5, 0, 0.66685},
		{5, 1, 1},
		
	};
	std::vector<std::shared_ptr<Surface>> surfaces;
	for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
		if (SceneObject::SceneObjects[i]->IsSurface())
			surfaces.emplace_back(Surface::SceneObjectToSurface(
				SceneObject::SceneObjects[i]));

	for(int i=0;i<circuitSize;i++) {
		
	}
	
		
	
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

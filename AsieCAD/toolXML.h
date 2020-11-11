#pragma once
#include "bSpline.h"
#include "sceneObject.h"
#include "tinyxml2/tinyxml2.h"
#include "point.h"
class ToolXML
{
	static void LoadPoints(tinyxml2::XMLElement* scene);
	static void LoadPoint(tinyxml2::XMLElement* elem);
	static void LoadNotPoints(tinyxml2::XMLElement* scene);
	static void LoadBezierC0(tinyxml2::XMLElement* data);
	static void LoadBezierC2(tinyxml2::XMLElement* data);
	static void LoadBezierInter(tinyxml2::XMLElement* data);
	static void FillCurve(tinyxml2::XMLElement* points, std::shared_ptr<Curve> curve);
	static void LoadTorus(tinyxml2::XMLElement* data);
	static std::vector<std::shared_ptr<Point>> FillSurface(
		tinyxml2::XMLElement* data, int& rows, int& cols);
	static void LoadPatchC0(tinyxml2::XMLElement* data);
	static void LoadPatchC2(tinyxml2::XMLElement* data);
	static std::shared_ptr<Point> FindPoint(tinyxml2::XMLElement* elem);
public:
	static void LoadScene(std::string path);
	static glm::vec3 ReadVector(tinyxml2::XMLElement* Pos);
	static bool SaveScene(std::string path);
	static void SaveVec3(glm::vec3 vec, std::string Name, tinyxml2::XMLElement* elem);
	static void ClearScene();
	static void LoadSaveMenu();
};


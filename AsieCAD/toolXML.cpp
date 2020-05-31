#include "toolXML.h"


#include "BezierC0.h"
#include "bezierPatch.h"
#include "bSpline.h"
#include "cubicInterpolated.h"
#include "splinePatch.h"
#include "torus.h"


void ToolXML::LoadScene(std::string path)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS)
		return;
	SceneObject::SceneObjects.erase(SceneObject::SceneObjects.begin() + 1,
		SceneObject::SceneObjects.end());
	tinyxml2::XMLElement* scene = doc.FirstChildElement("Scene");
	LoadPoints(scene);
	LoadNotPoints(scene);
}

void ToolXML::LoadPoints(tinyxml2::XMLElement* scene)
{
	for(auto elem = scene->FirstChildElement("Point"); elem != nullptr; 
	    elem = elem->NextSiblingElement("Point")) {
		LoadPoint(elem);
	}
}

void ToolXML::LoadPoint(tinyxml2::XMLElement* elem)
{
	std::string name = elem->Attribute("Name");
	glm::vec3 pos = ReadVector(elem->FirstChildElement("Position"));
	auto point = std::make_shared<Point>(pos);
	point->name = name;
	SceneObject::SceneObjects.emplace_back(point);
}

glm::vec3 ToolXML::ReadVector(tinyxml2::XMLElement* Pos)
{
	glm::vec3 pos;
	pos.x = Pos->FloatAttribute("X");
	pos.y = Pos->FloatAttribute("Y");
	pos.z = Pos->FloatAttribute("Z");
	return pos;
}

void ToolXML::LoadNotPoints(tinyxml2::XMLElement* scene)
{
	for (auto elem = scene->FirstChildElement(); elem != nullptr;
		elem = elem->NextSiblingElement()) {
		std::string type = elem->Name();
		if(type == "Point") {
		}
		else if(type == "BezierC0") {
			LoadBezierC0(elem);
		}
		else if (type == "BezierC2") {
			LoadBezierC2(elem);
		}
		else if (type == "BezierInter")
		{
			LoadBezierInter(elem);
		}
		else if (type == "Torus")
		{
			LoadTorus(elem);
		}
		else if (type == "PatchC0")
		{
			LoadPatchC0(elem);
		}
		else if (type == "PatchC2")
		{
			LoadPatchC2(elem);
		}
	}
}
void ToolXML::LoadBezierC0(tinyxml2::XMLElement* data)
{
	std::string name = data->Attribute("Name");
	auto curve = std::make_shared<BezierC0>(data);
	FillCurve(data->FirstChildElement(), curve);
	curve->name = name;
	SceneObject::SceneObjects.emplace_back(curve);

}
void ToolXML::LoadBezierC2(tinyxml2::XMLElement* data)
{
	std::string name = data->Attribute("Name");
	auto curve = std::make_shared<BSpline>(data);
	FillCurve(data->FirstChildElement(), curve);
	curve->name = name;
	SceneObject::SceneObjects.emplace_back(curve);
	
}
void ToolXML::LoadBezierInter(tinyxml2::XMLElement* data)
{
	std::string name = data->Attribute("Name");
	auto curve = std::make_shared<CubicInterpolated>(data);
	FillCurve(data->FirstChildElement(), curve);
	curve->name = name;
	SceneObject::SceneObjects.emplace_back(curve);

}
void ToolXML::LoadTorus(tinyxml2::XMLElement* data)
{
	std::string name = data->Attribute("Name");
	auto torus = std::make_shared<Torus>(data);
	torus->name = name;
	SceneObject::SceneObjects.emplace_back(torus);
}

void ToolXML::LoadPatchC0(tinyxml2::XMLElement* data)
{
	std::string name = data->Attribute("Name");
	int rows = 0, cols = 0;
	auto points = FillSurface(data, rows, cols);
	bool isCylinder = data->Attribute("WrapDirection") != std::string("None");
	auto patch = std::make_shared<BezierPatch>(points, data, isCylinder, rows, cols);
	patch->name = name;
	SceneObject::SceneObjects.emplace_back(patch);
}

void ToolXML::LoadPatchC2(tinyxml2::XMLElement* data)
{
	std::string name = data->Attribute("Name");
	int rows = 0, cols = 0;
	auto points = FillSurface(data, rows, cols);
	bool isCylinder = data->Attribute("WrapDirection") != std::string("None");
	auto patch = std::make_shared<SplinePatch>(points, data, isCylinder, rows, cols);
	patch->name = name;
	SceneObject::SceneObjects.emplace_back(patch);
}

std::vector<std::shared_ptr<Point>> ToolXML::FillSurface(tinyxml2::XMLElement* data,
                                                         int& rows, int& cols)
{
	bool isRowDirect = data->Attribute("WrapDirection") == std::string("Row");
	auto pointRefs = data->FirstChildElement();
	for (auto elem = pointRefs->FirstChildElement(); elem != nullptr;
	     elem = elem->NextSiblingElement()) {
		int Row = elem->IntAttribute("Row");
		int Column = elem->IntAttribute("Column");
		if (Row >= rows)
			rows = Row + 1;
		if (Column >= cols)
			cols = Column + 1;
	}
	auto points = std::vector<std::shared_ptr<Point>> (rows*cols);
	for (auto elem = pointRefs->FirstChildElement(); elem != nullptr;
	     elem = elem->NextSiblingElement()) {
		int Row = elem->IntAttribute("Row");
		int Column = elem->IntAttribute("Column");
		if (isRowDirect)
			points[Column * rows + Row] = FindPoint(elem);
		else
			points[Row * cols + Column] = FindPoint(elem);
	}
	//if (isRowDirect)
	//	std::swap(rows, cols);
	return points;
}

void ToolXML::FillCurve(tinyxml2::XMLElement* points, std::shared_ptr<Curve> curve)
{
	for (auto elem = points->FirstChildElement(); elem != nullptr;
		elem = elem->NextSiblingElement()) {
		std::shared_ptr<SceneObject> ptr = FindPoint(elem);
		curve->AddPoint(ptr);
	}
}

std::shared_ptr<Point> ToolXML::FindPoint(tinyxml2::XMLElement* elem)
{
	std::string ref = elem->Attribute("Name");
	for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
		if (SceneObject::SceneObjects[i]->name == ref && 
			SceneObject::SceneObjects[i]->IsPoint())
			return std::static_pointer_cast<Point>(SceneObject::SceneObjects[i]);
}

#include "toolXML.h"


#include "BezierC0.h"
#include "bezierPatch.h"
#include "bSpline.h"
#include "cubicInterpolated.h"
#include "splinePatch.h"
#include "torus.h"
#include "tinyxml2/tinyfiledialogs.h"


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
		auto point = FindPoint(elem);
		point->SetDeletability(0);
		if (isRowDirect)
			points[Column * rows + Row] = point;
		else
			points[Row * cols + Column] = point;
		
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

bool ToolXML::SaveScene(std::string path)
{
	tinyxml2::XMLDocument doc;

	auto declaration = doc.NewDeclaration();
	auto scene = doc.NewElement("Scene");
	scene->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	scene->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	scene->SetAttribute("xmlns", "http://mini.pw.edu.pl/mg1");
	for(int i=0;i<SceneObject::SceneObjects.size();i++) {
		SceneObject::SceneObjects[i]->Serialize(scene);
	}
	doc.LinkEndChild(scene);
	return doc.SaveFile(path.c_str()) == tinyxml2::XML_SUCCESS;

}
void ToolXML::SaveVec3(glm::vec3 vec, std::string Name, tinyxml2::XMLElement* elem)
{
	auto vector = elem->InsertNewChildElement(Name.c_str());
	vector->SetAttribute("X", vec.x);
	vector->SetAttribute("Y", vec.y);
	vector->SetAttribute("Z", vec.z);
}
void ToolXML::LoadSaveMenu()
{
	char const* lTheSaveFileName, *lTheOpenFileName;
	char const* lFilterPatterns[] = { "*.xml" };

	if (ImGui::Button("Open file")) {
		lTheOpenFileName = tinyfd_openFileDialog(
			"Choose an XML file to read",
			"",
			1,
			lFilterPatterns,
			NULL,
			0);
		if(lTheOpenFileName) {
			LoadScene(lTheOpenFileName);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save file as...")) {
		lTheSaveFileName = tinyfd_saveFileDialog(
			"Save your scene as",
			"rybcia.xml",
			1,
			lFilterPatterns,
			NULL);
		if(lTheSaveFileName) {
			SaveScene(lTheSaveFileName);
		}
	}
}
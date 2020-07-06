#include "sceneObject.h"
#include "torus.h"
#include "point.h"
#include <algorithm>
#include "BezierC0.h"
#include "bezierPatch.h"
#include "bSpline.h"
#include "cubicInterpolated.h"
#include "PointSurface.h"
#include "toolXML.h"


std::vector<std::shared_ptr<SceneObject>> SceneObject::SceneObjects;
int SceneObject::selected = -1;
int SceneObject::selectedCount = 0;
bool SceneObject::rotateAroundCursor = false;
bool SceneObject::hidePoints = false;
Position SceneObject::selectedCenter;
glm::mat4 SceneObject::viewProjection;

glm::vec3 SceneObject::GetRotationCenter()
{
	if (rotateAroundCursor || selectedCount < 1)
		return GetCursorCenter();
	else if (selectedCount == 1)
		return SceneObjects[selected]->GetCenter();
	else if (selectedCount > 1)
		return selectedCenter.location;
}
glm::vec3 SceneObject::GetCursorCenter()
{
	return SceneObjects[0]->GetCenter();
}
SceneObject::SceneObject(const char* _name)
{
	strcpy_s(text, 64, _name);
	name = text;
}
void SceneObject::RenderFullMenu()
{
	if (ImGui::InputText(IsPointObject()?"Object name ":"Object name", text, 64))
		name = text;
	RenderMenu();
}
void SceneObject::RenderScene()
{
	if (!hidePoints)
		Point::RenderPoints();
	for (int i = 0; i < SceneObjects.size(); i++)
		if (!SceneObjects[i]->IsPoint())
			SceneObjects[i]->Render();
	if (selectedCount > 1)
		Point::DrawPoint(selectedCenter.location, COLORS::CENTER);
	if (selectedCount == 1 && !SceneObjects[selected]->IsPoint())
		Point::DrawPoint(SceneObjects[selected]->GetCenter(), COLORS::CENTER);
}
void SceneObject::ItemListMenu()
{
	static bool showPoints = true;
	if (ImGui::CollapsingHeader("Object list")) {
		ImGui::Checkbox("Show Points", &showPoints);
		ImGui::ListBoxHeader("");
		for (int i = 0; i < SceneObjects.size(); i++)
			if (showPoints || !SceneObjects[i]->IsPoint())
				if (ImGui::Selectable(
					(std::to_string(i) + ": " + SceneObjects[i]->name).c_str(),
					SceneObjects[i]->isSelected)) {

					if (ImGui::GetIO().KeyCtrl)
						ChangeSelection(i);
					else
						Select(i);
				}
		ImGui::ListBoxFooter();
	}
}
void SceneObject::Select(int i)
{
	for (int j = 0; j < SceneObjects.size(); j++)
		SceneObjects[j]->isSelected = false;
	selected = i;
	if (selected >= 0) {
		SceneObjects[i]->isSelected = true;
		selectedCount = 1;
	}
	else
		selectedCount = 0;
}
void SceneObject::ChangeSelection(int i)
{
	SceneObjects[i]->isSelected = !SceneObjects[i]->isSelected;
	selectedCount += SceneObjects[i]->isSelected ? 1 : -1;
	if (selectedCount == 1)
		for (selected = 0; !SceneObjects[selected]->isSelected; selected++);
	else if (selectedCount > 1) {
		glm::vec3 centerLoc = glm::vec3(0);
		for (int i = 0; i < SceneObjects.size(); i++)
			if (SceneObjects[i]->isSelected)
				centerLoc += SceneObjects[i]->GetCenter();
		selectedCenter = Position(centerLoc / (float)selectedCount);
	}
	
}
void SceneObject::DrawMenu()
{
	ImGui::Checkbox("Rotate Camera Around Cursor", &rotateAroundCursor);
	ImGui::Checkbox("Hide Points", &hidePoints);
	ToolXML::LoadSaveMenu();
	AddItemMenu();
	ItemListMenu();
	COLORS::DrawMenu();
	RenderProperties();
}
void SceneObject::AddPointsToCruve()
{
	int curveInx;
	for (curveInx = 0; !SceneObjects[curveInx]->isSelected || !SceneObjects[curveInx]->IsCurve(); curveInx++);
	BezierC0* curve = static_cast<BezierC0*>(SceneObjects[curveInx].
		get());
	for (int i = 0; i < SceneObjects.size(); i++)
		if (SceneObjects[i]->isSelected && i != curveInx)
			curve->AddPoint(SceneObjects[i]);
}
void SceneObject::AddCurveFromPoints()
{
	auto curve = std::make_shared<BezierC0>();
	for (int i = 0; i < SceneObjects.size(); i++)
		if (SceneObjects[i]->isSelected)
			curve->AddPoint(SceneObjects[i]);
	SceneObjects.emplace_back(std::move(curve));
}
void SceneObject::RenderProperties()
{
	ImGui::Begin("Properties");	
	if (selectedCount == 1) {
		Surface::SurfaceInteresectionMenu();

		if (SceneObjects[selected]->IsDeletable() && ImGui::Button("Delete object")) {
			SceneObjects.erase(SceneObjects.begin() + selected);
			selected = -1;
			selectedCount = 0;
		}
		else
			SceneObjects[selected]->RenderFullMenu();
	}
	else if (selectedCount > 1) {
		Surface::SurfaceInteresectionMenu();
		Point::MergePointsMenu();
		BezierPatch::FillSurfaceMenu();
		std::string text = "Selected " + std::to_string(selectedCount) + " items";
		
		ImGui::Text(text.c_str());
		if (std::any_of(SceneObjects.begin(), SceneObjects.end(),
			[](auto const& object) { return object->isSelected && object->IsDeletable(); })
			&& ImGui::Button("Delete items")) {
			
			auto end = std::remove_if(SceneObjects.begin() + 1, SceneObjects.end(),
				[](auto const& object) { return object->isSelected && object->IsDeletable(); });
			SceneObjects.erase(end, SceneObjects.end());
			selectedCount = SceneObjects[0]->isSelected;
			selected = selectedCount ? 0 : -1;
		}
		int pointCount = 0;
		int curveCount = 0;
		for (int i = 0; i < SceneObjects.size(); i++)
			if (SceneObjects[i]->isSelected) {
				pointCount += SceneObjects[i]->IsPoint();
				curveCount += SceneObjects[i]->IsCurve();
			}
		if(pointCount == selectedCount)
			if(ImGui::Button("Create Curve"))
				AddCurveFromPoints();
		if (curveCount == 1 && pointCount == selectedCount - 1)
			if (ImGui::Button("Add point to curve"))
				AddPointsToCruve();
				
		if (selectedCenter.RenderMenu(GetCursorCenter()))
			for (int i = 0; i < SceneObjects.size(); i++)
				if (SceneObjects[i]->isSelected)
					SceneObjects[i]->UpdatePosition(selectedCenter.newPos(SceneObjects[i]->GetCenter()),
						selectedCenter.scaleChange, selectedCenter.rotationChange);
	}

	ImGui::End();
}
void SceneObject::AddItemMenu()
{
	if (ImGui::CollapsingHeader("Add items")) {
		if (ImGui::Button("Add Torus"))
			SceneObjects.emplace_back(std::make_shared<Torus>(50, 50, 1, 2));
		ImGui::SameLine();
		if (ImGui::Button("Add point")) {
			SceneObjects.emplace_back(std::make_shared<Point>(GetCursorCenter()));
			if (selected >= 0 && SceneObjects[selected]->IsCurve())
				(static_cast<Curve*>(SceneObjects[selected].get()))->
				AddPoint(SceneObjects[SceneObjects.size() - 1]);
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Bezier's C0 Curve"))
			SceneObjects.emplace_back(std::make_shared<BezierC0>());
		if (ImGui::Button("Add B-spline"))
			SceneObjects.emplace_back(std::make_shared<BSpline>());
		ImGui::SameLine();
		if (ImGui::Button("Add Cubic interpolaction curve"))
			SceneObjects.emplace_back(std::make_shared<CubicInterpolated>());
		PointSurface::RenderCreationMenu();
	}
}

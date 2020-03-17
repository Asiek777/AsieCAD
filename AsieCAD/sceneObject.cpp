#include "sceneObject.h"
#include "torus.h"
#include "point.h"


std::vector<std::unique_ptr<SceneObject>> SceneObject::SceneObjects;

SceneObject::SceneObject(const char* _name)
{
    strcpy_s(text, 64, _name);
    name = text;
}
void SceneObject::RenderFullMenu()
{
    if (ImGui::InputText("Object name", text, 64))
        name = text;
    RenderMenu();
}
void SceneObject::ItemListMenu()
{
	static int selected = -1, selectedCount = 0;
	if (ImGui::CollapsingHeader("Object list")) {
		ImGui::BeginChild("Item list");
		ImGui::ListBoxHeader("");
		for (int i = 0; i < SceneObjects.size(); i++)
			if (ImGui::Selectable((std::to_string(i) + ": " + SceneObjects[i]->name).c_str(),
				SceneObjects[i]->isSelected)) {
				
				if(ImGui::GetIO().KeyCtrl) {
					SceneObjects[i]->isSelected = !SceneObjects[i]->isSelected;
					selectedCount += SceneObjects[i]->isSelected ? 1 : -1;
					if (selectedCount == 1)
						for (selected = 0; !SceneObjects[selected]->isSelected; selected++);
				}
				else {
					for (int j = 0; j < SceneObjects.size(); j++)
						SceneObjects[j]->isSelected = false;
					selected = i;
					SceneObjects[i]->isSelected = true;
					selectedCount = 1;
				}
			}
		ImGui::ListBoxFooter();
		ImGui::EndChild();
	}

	ImGui::Begin("Properties");
	if (selectedCount == 1) {

		if (!SceneObjects[0]->isSelected && ImGui::Button("Delete object")) {
			SceneObjects.erase(SceneObjects.begin() + selected);
			selected = -1;
		}
		else
			SceneObjects[selected]->RenderFullMenu();
	}
	ImGui::End();
}
void SceneObject::AddItemMenu()
{
	if(ImGui::Button("Add Torus"))
        SceneObjects.emplace_back(std::make_unique<Torus>(50, 50, 1, 2, "new torus"));
	if(ImGui::Button("Add point"))
		SceneObjects.emplace_back(std::make_unique<Point>());
}

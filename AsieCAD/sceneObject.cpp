#include "sceneObject.h"
#include "torus.h"


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
	static int selected = -1;
	if (ImGui::CollapsingHeader("Object list")) {
		ImGui::BeginChild("Item list", ImVec2(0, 200), true);
		for (int i = 0; i < SceneObjects.size(); i++)
		{
			if (ImGui::Selectable((std::to_string(i)+ ": " +SceneObjects[i]->name).c_str(), 
				selected == i))
				selected = i;
		}
		ImGui::EndChild();
	}

    ImGui::Begin("Properties");
    if (selected >=0 && selected < SceneObjects.size()) {

	    if (ImGui::Button("Delete object")) {
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
	if(ImGui::Button("Add Torus")) {
        SceneObjects.emplace_back(std::make_unique<Torus>(50, 50, 1, 2, "new torus"));
	}
}

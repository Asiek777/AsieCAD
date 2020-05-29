#pragma once
#include <glm/detail/type_vec3.hpp>
#include "Imgui/imgui.h"

namespace COLORS
{
	inline glm::vec3 BASE = glm::vec3(1.0f, 1.0f, 1.0f);
	inline glm::vec3 HIGHLIGHT = glm::vec3(1.0f, 0.0f, 1.0f);
	inline glm::vec3 BROKEN = glm::vec3(0.3f, 0.3f, 1.0f);
	inline glm::vec3 CURVE_POINT = glm::vec3(0.0f, 1.0f, 0.0f);
	inline glm::vec3 CENTER = glm::vec3(1.0f, 1.0f, 0.0f);
	inline glm::vec3 BACKGROUND = glm::vec3(0.0f, 0.0f, 0.0f);
	
	static void DrawMenu()
	{
		if(ImGui::CollapsingHeader("Color edition")) {
			ImGui::ColorEdit3("Base", &BASE.x);			
			ImGui::ColorEdit3("Highlight", &HIGHLIGHT.x);
			ImGui::ColorEdit3("Selection center", &CENTER.x);
			ImGui::ColorEdit3("Curve's broken", &BROKEN.x);
			ImGui::ColorEdit3("Selected curve", &CURVE_POINT.x);
			ImGui::ColorEdit3("Background", &BACKGROUND.x);
		}
	}	
}

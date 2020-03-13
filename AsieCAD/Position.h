#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include  "Imgui/imgui.h"

struct Position
{
	glm::vec3 Location;
	glm::vec3 Rotation;
	glm::vec3 Scale;
	glm::mat4 modelMatrix;
	Position()
	{
		Location = glm::vec3(0);
		Rotation = glm::vec3(0);
		Scale = glm::vec3(1);
		updateMatrix();
	}
	
	void updateMatrix()
	{
		modelMatrix = glm::translate(glm::mat4(1.0f), Location);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(Rotation.x),
			glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(Rotation.y),
			glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(Rotation.z),
			glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, Scale);
	}
	glm::mat4 GetModelMatrix() const { return  modelMatrix; }

	void RenderMenu()
	{
		bool hasChanged = false;
		hasChanged |= ImGui::DragFloat3("Location", &Location.x, 0.02f);
		hasChanged |= ImGui::DragFloat3("Rotation", &Rotation.x, 0.5f);
		hasChanged |= ImGui::DragFloat3("Scale", &Scale.x, 0.02f);
		if (hasChanged)
			updateMatrix();
	};
};

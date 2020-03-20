#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Imgui/imgui.h"


struct Position
{
	glm::vec3 location;
	glm::vec3 rotation, lastRotation;
	glm::vec3 scale, lastScale;
	bool isCursorCenter = false;
	glm::mat4 modelMatrix;
	glm::vec3 cursor;
	Position(glm::vec3 _location = glm::vec3(0))
	{
		location = _location;
		rotation = glm::vec3(0);
		scale = glm::vec3(1);
		lastScale = scale;
		lastRotation = rotation;
	}
	void Update()
	{
		if (isCursorCenter) {			
			glm::vec3 toCenter = location - cursor;
			glm::vec3 rotate = rotation - lastRotation;
			glm::vec3 scal = scale / lastScale;
			glm::mat4 transform = glm::mat4(1.0f); //translate(glm::mat4(1.0f), cursor);
			transform = glm::rotate(transform, glm::radians(rotate.x),
				glm::vec3(1, 0, 0));
			transform = glm::rotate(transform, glm::radians(rotate.y),
				glm::vec3(0, 1, 0));
			transform = glm::rotate(transform, glm::radians(rotate.z),
				glm::vec3(0, 0, 1));
			transform = glm::scale(transform, scal);
			glm::vec3 move = glm::vec3(transform * glm::vec4(toCenter, 0));
			location = cursor + move;

		}
		lastRotation = rotation;
		lastScale = scale;

		UpdateMatrix();
	}
	

	void UpdateMatrix(){
		modelMatrix = glm::translate(glm::mat4(1.0f), location);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x),
			glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y),
			glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z),
			glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, scale);
	}
	glm::mat4 GetModelMatrix() const { return  modelMatrix; }

	void RenderMenu(glm::vec3 _cursor) {
		cursor = _cursor;
		bool hasChanged = false;
		ImGui::Checkbox("Cursor as transform center", &isCursorCenter);
		hasChanged |= ImGui::DragFloat3("location", &location.x, 0.02f);
		hasChanged |= ImGui::DragFloat3("rotation", &rotation.x, 0.5f);
		hasChanged |= ImGui::DragFloat3("scale", &scale.x, 0.015f);
		if (hasChanged)
			Update();
	}

};


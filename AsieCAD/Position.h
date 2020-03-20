#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Imgui/imgui.h"


struct Position
{
	glm::vec3 location, lastLocation, locationChange;
	glm::vec3 rotation, lastRotation, rotationChange;
	glm::vec3 scale, lastScale, scaleChange;
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
		lastLocation = location;
	}

	glm::vec3 newPos(glm::vec3 pos)
	{
		pos += locationChange;
		glm::vec3 center = isCursorCenter ? cursor : location;
		glm::vec3 toCenter = pos - center;
		glm::mat4 transform = glm::mat4(1.0f); //translate(glm::mat4(1.0f), cursor);
		transform = glm::rotate(transform, glm::radians(rotationChange.x),
			glm::vec3(1, 0, 0));
		transform = glm::rotate(transform, glm::radians(rotationChange.y),
			glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, glm::radians(rotationChange.z),
			glm::vec3(0, 0, 1));
		transform = glm::scale(transform, scaleChange);
		glm::vec3 move = glm::vec3(transform * glm::vec4(toCenter, 0));
		return center + move;
	}
	void Update()
	{
		rotationChange = rotation - lastRotation;
		scaleChange = scale / lastScale;
		locationChange = location - lastLocation;
		if (isCursorCenter) {			
			glm::vec3 toCenter = location - cursor;
			glm::mat4 transform = glm::mat4(1.0f); //translate(glm::mat4(1.0f), cursor);
			transform = glm::rotate(transform, glm::radians(rotationChange.x),
				glm::vec3(1, 0, 0));
			transform = glm::rotate(transform, glm::radians(rotationChange.y),
				glm::vec3(0, 1, 0));
			transform = glm::rotate(transform, glm::radians(rotationChange.z),
				glm::vec3(0, 0, 1));
			transform = glm::scale(transform, scaleChange);
			glm::vec3 move = glm::vec3(transform * glm::vec4(toCenter, 0));
			location = cursor + move;

		}
		lastRotation = rotation;
		lastScale = scale;
		lastLocation = location;

		UpdateMatrix();
	}

	void UpdatePosition(glm::vec3 pos, glm::vec3 scaleChange, glm::vec3 rotChange)
	{
		location = pos;
		scale *= scaleChange;
		rotation += rotChange;
		lastRotation = rotation;
		lastScale = scale;
		lastLocation = location;
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

	bool RenderMenu(glm::vec3 _cursor) {
		cursor = _cursor;
		bool hasChanged = false;
		ImGui::Checkbox("Cursor as transform center", &isCursorCenter);
		hasChanged |= ImGui::DragFloat3("location", &location.x, 0.02f);
		hasChanged |= ImGui::DragFloat3("rotation", &rotation.x, 0.5f);
		hasChanged |= ImGui::DragFloat3("scale", &scale.x, 0.015f);
		if (hasChanged) {
			Update();
			return true;
		}
		return false;
	}

};


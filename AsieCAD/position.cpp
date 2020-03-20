//#include "position.h"
//#include "sceneObject.h"
//#include  "Imgui/imgui.h"
//
//
//void Position::Update()
//{
//	if (isCursorCenter) {
//		glm::vec3 cursor = SceneObject::GetCursorCenter();
//		glm::vec3 toCenter = location - cursor;
//		glm::vec3 rotate = rotation - lastRotation;
//		glm::vec3 scal = scale / lastScale;
//		glm::mat4 transform = glm::mat4(1.0f); //translate(glm::mat4(1.0f), cursor);
//		transform = glm::rotate(transform, glm::radians(rotate.x),
//			glm::vec3(1, 0, 0));
//		transform = glm::rotate(transform, glm::radians(rotate.y),
//			glm::vec3(0, 1, 0));
//		transform = glm::rotate(transform, glm::radians(rotate.z),
//			glm::vec3(0, 0, 1));
//		transform = glm::scale(transform, scal);
//		glm::vec3 move = glm::vec3(transform * glm::vec4(toCenter, 0));
//		location = cursor + move;
//
//	}
//	lastRotation = rotation;
//	lastScale = scale;
//
//	UpdateMatrix();
//}
//void Position::UpdateMatrix()
//{
//	modelMatrix = glm::translate(glm::mat4(1.0f), location);
//	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x),
//		glm::vec3(1, 0, 0));
//	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y),
//		glm::vec3(0, 1, 0));
//	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z),
//		glm::vec3(0, 0, 1));
//	modelMatrix = glm::scale(modelMatrix, scale);
//}
//void Position::RenderMenu()
//{
//	bool hasChanged = false;
//	ImGui::Checkbox("Cursor as transform center", &isCursorCenter);
//	hasChanged |= ImGui::DragFloat3("location", &location.x, 0.02f);
//	hasChanged |= ImGui::DragFloat3("rotation", &rotation.x, 0.5f);
//	hasChanged |= ImGui::DragFloat3("scale", &scale.x, 0.015f);
//	if (hasChanged)
//		Update();
//}

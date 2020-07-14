#include "pointSurface.h"

#include "bezierPatch.h"
#include "splinePatch.h"


int PointSurface::PatchCount[2] = { 1,1 };
float PointSurface::PatchSize[2] = { 3,3 };
float PointSurface::CylinderLength = 1;
float PointSurface::CylinderRadius = 3;
std::unique_ptr<Shader> PointSurface::meshShader;

PointSurface::PointSurface(std::vector<std::shared_ptr<Point>> _points, bool _isCylinder, const char* _name) :
	PointObject(_name), curveCount{ 4,4 }
{
	if (!meshShader)
		meshShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	patchCount[0] = PatchCount[0];
	patchCount[1] = PatchCount[1];
	curveIndexes[1] = std::make_unique<MeshBuffer>(std::vector<float>{ 1, 1, 1 });
	curveIndexes[0] = std::make_unique<MeshBuffer>(std::vector<float>{ 1, 1, 1 });
	isEditable = false;
	UpdateCurvesBuffers();
	for (int i = 0; i < _points.size(); i++)
		points.emplace_back(
			ControlPoint{ std::weak_ptr<SceneObject>(_points[i]), false });
	isCylinder = _isCylinder;
}

PointSurface::~PointSurface()
{
	for (int i = 0; i < points.size(); i++)
		if(!points[i].point.expired())
			static_cast<Point*> (points[i].point.lock().get())->SetDeletability(true);
}

void PointSurface::RenderMesh(int pointCount[2])
{
	std::vector<glm::vec3> knots(points.size());
	std::vector<unsigned> indices;
	for (int i = 0; i < points.size(); i++)
		knots[i] = points[i].point.lock()->GetCenter();
	for (int i = 0; i < pointCount[0]; i++)
		for (int j = 0; j < pointCount[1]; j++) {
			if (i < pointCount[0] - 1) {
				indices.emplace_back(i * pointCount[1] + j);
				indices.emplace_back((i + 1) * pointCount[1] + j);
			}
			if (isCylinder || j < pointCount[1] - 1) {
				indices.emplace_back(i * pointCount[1] + j);
				indices.emplace_back(i * pointCount[1] + (j + 1) % pointCount[1]);
			}
		};
	auto mesh = std::make_unique<MeshBuffer>(MeshBuffer::Vec3ToFloats(knots), 1, indices);
	meshShader->use();
	meshShader->setMat4("model", glm::mat4(1.0f));
	meshShader->setMat4("viewProjection", viewProjection);
	meshShader->setVec3("color", COLORS::BROKEN);
	glBindVertexArray(mesh->GetVAO());
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void PointSurface::RenderMenu()
{
	ImGui::Checkbox("Show mesh", &showMesh);
	if (ImGui::DragInt2("Curves count", curveCount, 0.3, 2, 64))
		UpdateCurvesBuffers();
	PointObject::RenderMenu();
	TestSurfaceMenu();
}

void PointSurface::Serialize(int pointCount[2], tinyxml2::XMLElement* scene, std::string type)
{
	auto ptr = scene->InsertNewChildElement(type.c_str());
	ptr->SetAttribute("Name", name.c_str());
	ptr->SetAttribute("ShowControlPolygon", showMesh);
	ptr->SetAttribute("RowSlices", curveCount[0] - 1);
	ptr->SetAttribute("ColumnSlices", curveCount[1] - 1);
	ptr->SetAttribute("WrapDirection", isCylinder?"Column":"None");
	auto pointRefs = ptr->InsertNewChildElement("Points");
	for (int i = 0; i < points.size(); i++) {
		auto pRef = pointRefs->InsertNewChildElement("PointRef");
		pRef->SetAttribute("Name", points[i].point.lock()->name.c_str());
		pRef->SetAttribute("Row", i / pointCount[1]);
		pRef->SetAttribute("Column", i % pointCount[1]);
	}
	
}

void PointSurface::UpdateCurvesBuffers()
{
	for (int dim = 0; dim < 2; dim++) {
		std::vector<glm::vec3> curves(curveCount[dim] * patchCount[dim]);
		if (!isTrimmed) {
			for (int i = 0; i < patchCount[dim]; i++) 
				for (int j = 0; j < curveCount[dim]; j++) {
					curves[i * curveCount[dim] + j] = glm::vec3(
						(float)j / (curveCount[dim] - 1), -1, -1);
				}
		}
		else {
			auto curve = trimCurve.lock();
			bool alongU = dynamic_cast<BezierPatch*>(this) ? !dim : dim;
			auto izolines = curve->CalcTrimming((curveCount[dim] - 1) * patchCount[dim] + 1,
				alongU, isFirst);
			for (int i = 0; i < patchCount[dim]; i++)
				for (int j = 0; j < curveCount[dim]; j++) {
					glm::vec2 coordsRange = izolines[i * (curveCount[dim] - 1) + j];
					curves[i * curveCount[dim] + j] = glm::vec3(
						(float)j / (curveCount[dim] - 1), coordsRange.s, coordsRange.t);
				}
		}
		curveIndexes[dim]->UpdateBuffer(MeshBuffer::Vec3ToFloats(curves));
	}
}

void PointSurface::SetTrimCurve(std::shared_ptr<IntersectionCurve> curve, bool _isFirst)
{
	isTrimmed = true;
	Surface::SetTrimCurve(curve, _isFirst);
	UpdateCurvesBuffers();
}

void PointSurface::RenderCreationMenu()
{
	if (ImGui::CollapsingHeader("Bezier patch creation")) {
		ImGui::DragInt2("Patches count", PatchCount, 0.3, 1, 50);
		ImGui::DragFloat2("Patches patchCount", PatchSize, 0.02, 0);
		ImGui::DragFloat("Cylinder length", &CylinderLength, 0.02, 0);
		ImGui::DragFloat("Cylinder radius", &CylinderRadius, 0.001, 0);

		if (ImGui::Button("Add patch")) {
			int size[2] = { PatchCount[0] * 3 + 1, PatchCount[1] * 3 + 1 };
			auto points = PrepareFlatVertices(size);
			SceneObjects.emplace_back(std::make_shared<BezierPatch>(points, false));
		}
		ImGui::SameLine();
		if (ImGui::Button("Add cylinder patch")) {
			int size[2] = { PatchCount[0] * 3 + 1, PatchCount[1] * 3 };
			auto points = PrepareRoundVertices(size);
			SceneObjects.emplace_back(std::make_shared<BezierPatch>(points, true));
		}
		if (ImGui::Button("Add spline patch")) {
			int size[2] = { PatchCount[0] + 3, PatchCount[1] + 3 };
			auto points = PrepareFlatVertices(size);
			SceneObjects.emplace_back(std::make_shared<SplinePatch>(points, false));
		}
		ImGui::SameLine();
		if (ImGui::Button("Add cylinder spline patch")) {
			int size[2] = { PatchCount[0] + 3, PatchCount[1] };
			auto points = PrepareRoundVertices(size);
			SceneObjects.emplace_back(std::make_shared<SplinePatch>(points, true));
		}
	}
}

std::vector<std::shared_ptr<Point>> PointSurface::PrepareFlatVertices(int size[2])
{
	std::vector<std::shared_ptr<Point>>	points(size[0] * size[1]);
	glm::vec3 begin = GetCursorCenter();
	glm::vec2 PointDistance(PatchSize[0] / (size[0] - 1), PatchSize[1] / (size[1] - 1));
	for (int i = 0; i < size[0]; i++)
		for (int j = 0; j < size[1]; j++)
			points[i * size[1] + j] = std::make_shared<Point>(begin +
				glm::vec3(PointDistance.x * i, 0, PointDistance.y * j));
	for (int i = 0; i < points.size(); i++) {
		points[i]->SetDeletability(false);
		SceneObjects.push_back(points[i]);
	}
	return  points;
}

std::vector<std::shared_ptr<Point>> PointSurface::PrepareRoundVertices(int size[2])
{
	std::vector<std::shared_ptr<Point>>	points(size[0] * size[1]);
	glm::vec3 begin = GetCursorCenter();
	for (int i = 0; i < size[0]; i++)
		for (int j = 0; j < size[1]; j++) {
			float angle = j * glm::two_pi<float>() / size[1];
			glm::vec3 offset = glm::vec3(i * CylinderLength / size[0],
				std::sinf(angle) * CylinderRadius, std::cosf(angle) * CylinderRadius);
			points[i * size[1] + j] = std::make_shared<Point>(begin + offset);
		}
	for (int i = 0; i < points.size(); i++) {
		points[i]->SetDeletability(false);
		SceneObjects.push_back(points[i]);
	}
	return  points;
}
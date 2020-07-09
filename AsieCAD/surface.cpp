#include "surface.h"
#include "colors.h"
#include "PointSurface.h"
#include "torus.h"
#include "cubicInterpolated.h"


void Surface::TestSurfaceMenu()
{
	static glm::vec2 coords = glm::vec2(0.5f, 0.5f);
	ImGui::DragFloat2("Point coords", &coords.x, 0.002, 0, 1);
	TngSpace space = GetTangentAt(coords.x, coords.y);
	Point::DrawPoint(space.pos, COLORS::CURVE_POINT);
	Point::DrawPoint(space.pos + 0.3f * space.normal, COLORS::CURVE_POINT);
	Point::DrawPoint(space.pos + space.diffU, COLORS::CURVE_POINT);
	Point::DrawPoint(space.pos + space.diffV, COLORS::CURVE_POINT);
}

void Surface::SurfaceInteresectionMenu()
{
	if(SceneObject::selectedCount == 2) {
		std::shared_ptr<Surface> surface[2];
		for (int i = 0; i < SceneObject::SceneObjects.size(); i++)
			if (SceneObject::SceneObjects[i]->isSelected) {
				if (!SceneObject::SceneObjects[i]->IsSurface())
					return;
				if (!surface[0])
					surface[0] = SceneObjectToSurface(SceneObject::SceneObjects[i]);
				else {
					surface[1] = SceneObjectToSurface(SceneObject::SceneObjects[i]);
					break;
				}				
			}
		if (ImGui::Button("Calculate intersection")) {
			FindIntersection(surface[0], surface[1]);
		}
	}
}

void Surface::FindIntersection(std::shared_ptr<Surface> s1, std::shared_ptr<Surface> s2)
{
	glm::vec4 pos = CalcFirstPoint(s1, s2);
	auto curve = std::make_shared<CubicInterpolated>();
	SceneObject::SceneObjects.emplace_back(curve);
	
	for (int j = 0; j < 25; ++j) {
		glm::vec4 newPos = pos;
		float d = 0.1f;
		for (int i = 0; i < 20; ++i) {
			TngSpace space1 = s1->GetTangentAt(newPos.s, newPos.t);
			TngSpace space2 = s2->GetTangentAt(newPos.p, newPos.q);
			glm::vec3 t = glm::cross(space1.normal, space2.normal);
			glm::vec4 jac[4];
			jac[0] = glm::vec4(space1.diffU, glm::dot(space1.diffU, t));
			jac[1] = glm::vec4(space1.diffV, glm::dot(space1.diffV, t));
			jac[2] = glm::vec4(-space2.diffU, 0.f);
			jac[3] = glm::vec4(-space2.diffV, 0.f);
			glm::mat4 jacobian(jac[0], jac[1], jac[2], jac[3]);
			glm::vec4 f = glm::vec4(space1.pos - space2.pos,
			                        glm::dot(space1.pos - s1->GetPointAt(pos.s, pos.t), t) - d);
			newPos = newPos - glm::inverse(jacobian) * f;
		}
		pos = newPos;
		auto point = std::make_shared<Point>(s1->GetPointAt(newPos.s, newPos.t));
		curve->AddPoint(point);
		SceneObject::SceneObjects.emplace_back(point);
	}
}

glm::vec4 Surface::CalcFirstPoint(std::shared_ptr<Surface> s1, std::shared_ptr<Surface> s2)
{
	float divide = 5, minDist = 100000000;
	glm::vec4 pos(0.5f);
	for (int i = 0; i <= divide; i++)
		for (int j = 0; j <= divide; j++)
			for (int k = 0; k <= divide; k++)
				for (int l = 0; l <= divide; l++) {
					glm::vec4 newPos(i / divide, j / divide, k / divide, l / divide);
					float dist = calcFunction(s1, s2, newPos);
					if (dist < minDist) {
						pos = newPos;
						minDist = dist;
					}
				}
	TngSpace space1 = s1->GetTangentAt(pos.s, pos.t);
	TngSpace space2 = s2->GetTangentAt(pos.p, pos.q);
	//auto point = std::make_shared<Point>(space1.pos);
	//SceneObject::SceneObjects.emplace_back(point);
	glm::vec4 gradient = minusGradient(pos, space1, space2);
	float alfa = FunctionMin(pos, gradient, s1, s2);
	for (int i = 0; i < 100; i++) {
		space1 = s1->GetTangentAt(pos.s, pos.t);
		space2 = s2->GetTangentAt(pos.p, pos.q);
		glm::vec4 gradient = minusGradient(pos, space1, space2);
		float alfa = FunctionMin(pos, gradient, s1, s2);
		std::cout << alfa << std::endl;
		pos += alfa * gradient;
		if (alfa < 0.0000001f)
			break;
	}
	auto point = std::make_shared<Point>(s1->GetPointAt(pos.s, pos.t));
	SceneObject::SceneObjects.emplace_back(point);
	point = std::make_shared<Point>(s2->GetPointAt(pos.p, pos.q));
	SceneObject::SceneObjects.emplace_back(point);
	return pos;
}

float Surface::calcFunction(std::shared_ptr<Surface>& s1, std::shared_ptr<Surface>& s2, 
                            glm::vec4 pos)
{
	auto p1 = s1->GetPointAt(pos.s, pos.t);
	auto p2 = s2->GetPointAt(pos.p, pos.q);
	return glm::dot(p1 - p2, p1 - p2);
}

glm::vec4 Surface::minusGradient(glm::vec4 pos, TngSpace &space1, TngSpace &space2)
{
	glm::vec3 diff[4];
	diff[0] = 2.f * space1.diffU * (space1.pos - space2.pos);
	diff[1] = 2.f * space1.diffV * (space1.pos - space2.pos);
	diff[2] = -2.f * space2.diffU * (space1.pos - space2.pos);
	diff[3] = -2.f * space2.diffV * (space1.pos - space2.pos);
	glm::vec4 result;
	for (int i = 0; i < 4; i++)
		result[i] = diff[i].x + diff[i].y + diff[i].z;
	return -glm::normalize(result);
}

std::shared_ptr<Surface> Surface::SceneObjectToSurface(std::shared_ptr<SceneObject> object)
{
	auto p1 = std::dynamic_pointer_cast<PointSurface>(object);
	auto p2 = std::dynamic_pointer_cast<Torus>(object);
	if (p1)
		return p1;
	else
		return p2;
}

float Surface::FunctionMin(glm::vec4 x, glm::vec4 p, std::shared_ptr<Surface>& s1,
	std::shared_ptr<Surface>& s2)
{
	static const float phi = 0.5f * (1 + sqrtf(5));
	float min = 0, max = 0.2, right, left;
	bool lastCalcedRight = true;
	right = calcFunction(s1, s2, x + p * ((max - min) / phi + min));
	for (int i = 0; i < 40; i++) {
		if (lastCalcedRight)
			left = calcFunction(s1, s2, x + p * (max - (max - min) / phi));
		else
			right = calcFunction(s1, s2, x + p * ((max - min) / phi + min));
		if (left < right) {
			right = left;
			lastCalcedRight = true;
			max = (max - min) / phi + min;
		}
		else {
			left = right;
			lastCalcedRight = false;
			min = max - (max - min) / phi;
		}
	}
	return (max + min) / 2.f;
}


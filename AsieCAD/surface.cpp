#include "surface.h"

#include <algorithm>



#include "colors.h"
#include "cursor.h"
#include "PointSurface.h"
#include "torus.h"

float Surface::stepLength = 0.1f;
bool Surface::beginFromCursor = false;

void Surface::TestSurfaceMenu()
{
	//static glm::vec2 coords = glm::vec2(0.5f, 0.5f);
	//ImGui::DragFloat2("Point coords", &coords.x, 0.002, 0, 1);
	//TngSpace space = GetTangentAt(coords.x, coords.y);
	//Point::DrawPoint(space.pos, COLORS::CURVE_POINT);
	//Point::DrawPoint(space.pos + 0.3f * space.normal, COLORS::CURVE_POINT);
	//Point::DrawPoint(space.pos + space.diffU, COLORS::CURVE_POINT);
	//Point::DrawPoint(space.pos + space.diffV, COLORS::CURVE_POINT);
}

void Surface::SurfaceInteresectionMenu()
{
	if (SceneObject::selectedCount == 1) {
		if (SceneObject::SceneObjects[SceneObject::selected]->IsSurface()) {
			auto surface = SceneObjectToSurface(
				SceneObject::SceneObjects[SceneObject::selected]);
			if (ImGui::CollapsingHeader("Find Intersection")) {
				ImGui::Checkbox("Begin from cursor", &beginFromCursor);
				ImGui::DragFloat("Step length", &stepLength, 0.001, 0.001f, 1.f);

				if (ImGui::Button("Calculate intersection")) {
					FindIntersection(surface, surface);
				}
			}
		}
	}
	else if (SceneObject::selectedCount == 2) {
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
		if (ImGui::CollapsingHeader("Find Intersection")) {
			ImGui::Checkbox("Begin from cursor", &beginFromCursor);
			ImGui::DragFloat("Step length", &stepLength, 0.001, 0.001f, 1.f);
			
			if (ImGui::Button("Calculate intersection")) {
				FindIntersection(surface[0], surface[1]);
			}
		}
	}
}

void Surface::FindIntersection(std::shared_ptr<Surface> s1, std::shared_ptr<Surface> s2)
{
	glm::vec4 pos = CalcFirstPoint(s1, s2);
	if (pos.x == -1 || isnan(pos.x)) {
		IntersectionCurve::intersectionNotFound = true;
		return;
	}
	for (int j = 0; j < 4; j++)
		if (pos[j] > 1 || pos[j] < 0)
			pos[j] = pos[j] - std::floorf(pos[j]);
	std::vector <IntersectionPoint> points;
	points.emplace_back(IntersectionPoint{ pos, s1->GetPointAt(pos.s, pos.t) });
	bool isClosed = FindAnotherPoints(pos, points, false, s1, s2);
	auto curve = std::make_shared<IntersectionCurve>(points, isClosed, s1, s2);
	if (isClosed) {
		s1->SetTrimCurve(curve, true);
		s2->SetTrimCurve(curve, false);
	}
	IntersectionCurve::newest = curve;
	
	SceneObject::SceneObjects.emplace_back(curve);
}

glm::vec4 Surface::CalcFirstPoint(std::shared_ptr<Surface> s1, std::shared_ptr<Surface> s2)
{
	float divide = 5;
	if (s1 == s2)
		return FirstPointFromOneSurface(s1, divide);
	if (beginFromCursor) {
		return FirstPointFromCursor(s1, s2, divide);
	}
	else {
		return FirstPointFromTwoSurfaces(s1, s2, divide);
	}
}

glm::vec4 Surface::FirstPointFromCursor(std::shared_ptr<Surface> s1, 
	std::shared_ptr<Surface> s2, float divide)
{
	float minDist[] = { 10000000, 10000000 };
	glm::vec4 pos(0.5f), newPos;
	auto cursor = std::dynamic_pointer_cast<Cursor>(SceneObject::SceneObjects[0]);
	glm::vec4 startPos[2];
	for (int i = 0; i <= divide; i++)
		for (int j = 0; j <= divide; j++) {
			newPos = glm::vec4 (i / divide, j / divide, 0, 0);
			float dist[2] = { calcFunction(s1, cursor, newPos),
				calcFunction(s2, cursor, newPos) };
			if(dist[0]<minDist[0]) {
				minDist[0] = dist[0];
				pos.s = newPos.s;
				pos.t = newPos.t;
			}
			if(dist[1] < minDist[1]) {
				minDist[1] = dist[1];
				pos.p = newPos.s;
				pos.q = newPos.t;
			}
		}
	glm::vec4 s1Pos = GradientMinimalization(pos, s1, cursor);
	glm::vec4 s2Pos = GradientMinimalization(pos, cursor, s2);
	pos = glm::vec4(s1Pos.s, s1Pos.t, s2Pos.p, s2Pos.q);
	pos = GradientMinimalization(pos, s1, s2);
	float dist = glm::length(s1->GetPointAt(pos.s, pos.t) -
		s2->GetPointAt(pos.p, pos.q));
	if (dist < 0.01f)
		return pos;
	return glm::vec4(-1);
}

glm::vec4 Surface::FirstPointFromTwoSurfaces(std::shared_ptr<Surface> s1, 
	std::shared_ptr<Surface> s2, float divide)
{
	std::vector<std::pair<float, int>> bestPos;
	std::vector<glm::vec4> startPos;
	int index = 0;

	for (int i = 0; i <= divide; i++)
		for (int j = 0; j <= divide; j++)
			for (int k = 0; k <= divide; k++)
				for (int l = 0; l <= divide; l++) {
					glm::vec4 newPos(i / divide, j / divide, k / divide, l / divide);
					float dist = calcFunction(s1, s2, newPos);
					startPos.emplace_back(newPos);
					bestPos.emplace_back(std::make_pair(dist, index++));
				}
	std::sort(bestPos.begin(), bestPos.end());
	for (int i = 0; i < bestPos.size(); i++) {
		glm::vec4 pos = GradientMinimalization(startPos[bestPos[i].second], s1, s2);
		float dist = glm::length(s1->GetPointAt(pos.s, pos.t) - 
			s2->GetPointAt(pos.p, pos.q));
		if (dist < 0.01f)
			return pos;
		std::cout << "checked " << i << " of " << bestPos.size() << " possible begginnings\n";
	}
	return glm::vec4(-1);
}

glm::vec4 Surface::FirstPointFromOneSurface(std::shared_ptr<Surface> s1, float divide)
{
	std::vector<std::pair<float, int>> bestPos;
	std::vector<glm::vec4> startPos;
	int index = 0;

	for (int i = 0; i <= divide; i++)
		for (int j = 0; j <= divide; j++)
			for (int k = 0; k <= divide; k++)
				for (int l = 0; l <= divide; l++)
					if (i != k || j != l) {
						glm::vec4 newPos(i / divide, j / divide, k / divide, l / divide);
						float dist = calcFunction(s1, s1, newPos);
						startPos.emplace_back(newPos);
						bestPos.emplace_back(std::make_pair(dist, index++));
					}
	std::sort(bestPos.begin(), bestPos.end());
	for (int i = 0; i < bestPos.size(); i++) {
		glm::vec4 pos = GradientMinimalization(startPos[bestPos[i].second], s1, s1);
		float dist = glm::length(s1->GetPointAt(pos.s, pos.t) -
			s1->GetPointAt(pos.p, pos.q));
		//if (dist < 0.00001f)
		//	continue;
		float coordDist = glm::length(glm::vec2(pos.s - pos.p, pos.t - pos.q));
		if(std::max(std::abs(pos.s - pos.p), std::abs(pos.t - pos.q)) > 0.999f)
			continue;
		if (dist < 0.01f && coordDist > 0.01f)
			return pos;
		std::cout << "checked " << i << " of " << bestPos.size() << " possible begginnings\n";
	}
	return glm::vec4(-1);
}

bool Surface::FindAnotherPoints(glm::vec4 pos, std::vector<IntersectionPoint>& points,
	bool isReverse, std::shared_ptr<Surface> s1, std::shared_ptr<Surface> s2)
{
	while (true) {
		glm::vec4 newPos = pos;
		glm::vec4 f;
		for (int i = 0; i < 20; ++i) {
			TngSpace space1 = s1->GetTangentAt(newPos.s, newPos.t);
			TngSpace space2 = s2->GetTangentAt(newPos.p, newPos.q);
			glm::vec3 t = glm::cross(space1.normal, space2.normal);
			if (isReverse)
				t *= -1.f;
			glm::vec4 jac[4];
			jac[0] = glm::vec4(space1.diffU, glm::dot(space1.diffU, t));
			jac[1] = glm::vec4(space1.diffV, glm::dot(space1.diffV, t));
			jac[2] = glm::vec4(-space2.diffU, 0.f);
			jac[3] = glm::vec4(-space2.diffV, 0.f);
			glm::mat4 jacobian(jac[0], jac[1], jac[2], jac[3]);
			f = glm::vec4(space1.pos - space2.pos,
				glm::dot(space1.pos - s1->GetPointAt(pos.s, pos.t), t) - stepLength);
			newPos = newPos - glm::inverse(jacobian) * f;
			if(isnan(newPos.x)) {
				f = glm::vec4(1);
				break;
			}

		}
		pos = newPos;
		if (glm::length(f) > 0.1 ||
			(!s1->RollU() && (pos.s < 0 || pos.s > 1)) ||
			(!s1->RollV() && (pos.t < 0 || pos.t > 1)) ||
			(!s2->RollU() && (pos.p < 0 || pos.p > 1)) ||
			(!s2->RollV() && (pos.q < 0 || pos.q > 1))) {
			if (!isReverse) {
				glm::vec4 coords = points[0].coords;
				std::reverse(points.begin(), points.end());
				FindAnotherPoints(coords, points, true, s1, s2);
			}
			return false;
		}
		glm::vec3 location = s1->GetPointAt(pos.s, pos.t);
		IntersectionPoint inter{ pos, location };
		points.emplace_back(inter);
		if (glm::length(location - points[0].location) < stepLength)
			return true;
	}
}

glm::vec4 Surface::GradientMinimalization(glm::vec4 pos, std::shared_ptr<Surface> s1, 
	std::shared_ptr<Surface> s2)
{
	TngSpace space1 = s1->GetTangentAt(pos.s, pos.t);
	TngSpace space2 = s2->GetTangentAt(pos.p, pos.q);
	if (space1.pos == space2.pos)
		return pos;
	glm::vec4 gradient = minusGradient(pos, space1, space2);
	float alfa = FunctionMin(pos, gradient, s1, s2);
	for (int i = 0; i < 100; i++) {
		space1 = s1->GetTangentAt(pos.s, pos.t);
		space2 = s2->GetTangentAt(pos.p, pos.q);
		glm::vec4 gradient = minusGradient(pos, space1, space2);
		if (isnan(gradient.x))	
			return pos;
		float alfa = FunctionMin(pos, gradient, s1, s2);
		pos += alfa * gradient;
		if (alfa < 0.0000001f)
			break;
	}
	return pos;
}

float Surface::calcFunction(std::shared_ptr<Surface> s1, std::shared_ptr<Surface> s2, 
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


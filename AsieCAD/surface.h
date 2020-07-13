#pragma once
#include "intersectionCurve.h"
#include "point.h"
#include "glm/glm.hpp"

struct TngSpace
{
	TngSpace()
	{
		pos = diffV = diffU = normal = glm::vec3(0);
	}
	glm::vec3 pos, diffU, diffV, normal;
};

struct IntersectionPoint;
class IntersectionCurve;

class Surface
{
	static float stepLength;
	static bool beginFromCursor;
	
	static glm::vec4 minusGradient(glm::vec4 pos, TngSpace& space1, TngSpace& space2);
	static float calcFunction(std::shared_ptr<Surface> s1,
		std::shared_ptr<Surface> s2, glm::vec4 pos);
	
	static void FindIntersection(std::shared_ptr<Surface> s1, std::shared_ptr<Surface> s2);
	static glm::vec4 CalcFirstPoint(std::shared_ptr<Surface> s1, std::shared_ptr<Surface> s2);
	
	static glm::vec4 FirstPointFromCursor(std::shared_ptr<Surface> s1,
		std::shared_ptr<Surface> s2, float divide);
	static glm::vec4 FirstPointFromTwoSurfaces(std::shared_ptr<Surface> s1,
		std::shared_ptr<Surface> s2, float divide);
	static glm::vec4 FirstPointFromOneSurface(std::shared_ptr<Surface> s1, float divide);
	static glm::vec4 GradientMinimalization(glm::vec4 pos, 
		std::shared_ptr<Surface> s1, std::shared_ptr<Surface> s2);
	
	static std::shared_ptr<Surface> SceneObjectToSurface(std::shared_ptr<SceneObject> object);
	static float FunctionMin(glm::vec4 x, glm::vec4 p, 
		std::shared_ptr<Surface>& s1, std::shared_ptr<Surface>& s2);

	
	static bool FindAnotherPoints(glm::vec4 pos, std::vector<IntersectionPoint>& points,
	                              bool isReverse, std::shared_ptr<Surface> s1,
	                              std::shared_ptr<Surface> s2);
	
protected:
	std::weak_ptr<IntersectionCurve> trimCurve;
	bool isFirst;
	void TestSurfaceMenu();
	
public:
	virtual ~Surface() {}
	virtual glm::vec3 GetPointAt(float u, float v) = 0;
	virtual TngSpace GetTangentAt(float u, float v) = 0;
	virtual bool RollU() { return false; }
	virtual bool RollV() { return false; }
	virtual void SetTrimCurve(std::shared_ptr<IntersectionCurve> curve, bool _isFirst)
		{	trimCurve = curve; isFirst = _isFirst;	}
	static void SurfaceInteresectionMenu();

	
};
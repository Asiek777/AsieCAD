#pragma once
#include "sceneObject.h"
#include "surface.h"
#include "meshBuffer.h"

struct IntersectionPoint
{
	glm::vec4 coords;
	glm::vec3 location;
};


class IntersectionCurve :
    public SceneObject
{
	std::vector<IntersectionPoint> points;
	std::shared_ptr<Surface> s1, s2;
	
	std::unique_ptr<MeshBuffer> mesh;
	static bool intersectionNotFound;
	static int Number;
	static std::unique_ptr<Shader> shader;
	static std::weak_ptr<IntersectionCurve> newest;
	friend class Surface;
	void IzolineIntersection(float line, std::vector<float>& intersections, 
		glm::vec2 p1, glm::vec2 p2);
	
public:
	IntersectionCurve(std::vector<IntersectionPoint> _points, bool isClosed,
		std::shared_ptr<Surface> _s1, std::shared_ptr<Surface> _s2);
	void Render() override;
	void RenderMenu() override;
	glm::vec3 GetCenter() override;
	void Serialize(tinyxml2::XMLElement* scene) override {}
	std::vector<glm::vec2> CalcTrimming(int lineCount, bool alongU, bool isFirst);
	static void RenderPlot();
};

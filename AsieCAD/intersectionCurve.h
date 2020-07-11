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
	
	static int Number;
	static std::unique_ptr<Shader> shader;
	std::unique_ptr<MeshBuffer> mesh;
	
	
public:
	IntersectionCurve(std::vector<IntersectionPoint> _points,
		std::shared_ptr<Surface> _s1, std::shared_ptr<Surface> _s2);
	void Render() override;
	void RenderMenu() override {}
	glm::vec3 GetCenter() override;
	void Serialize(tinyxml2::XMLElement* scene) override {}
};


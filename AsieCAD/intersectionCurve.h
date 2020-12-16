#pragma once
#include "cubicInterpolated.h"
#include "sceneObject.h"
#include "surface.h"
#include "meshBuffer.h"

struct IntersectionPoint
{
	glm::vec4 coords;
	glm::vec3 location;
};

enum Openness;

class IntersectionCurve :
    public SceneObject
{
	std::vector<IntersectionPoint> points;
	std::shared_ptr<Surface> s1, s2;	
	std::unique_ptr<MeshBuffer> mesh;
	std::vector<std::vector<float>> grid[4];
	unsigned int tex[2];
	
	static bool intersectionNotFound;
	static int Number;
	static std::unique_ptr<Shader> shader;
	static std::weak_ptr<IntersectionCurve> newest;
	friend class Surface;
	void GenerateTextures(int texNumber);
	void IzolineIntersection(float line, std::vector<float>& intersections, 
		glm::vec2 p1, glm::vec2 p2, bool isReversed);
	static void RenderPlotGrid(std::shared_ptr<IntersectionCurve> curve, int offset);
	void CalcLineInteresctions(float line, std::vector<glm::vec2>& coords, 
	                           std::vector<float>& intersections, bool isReversed);
	
public:
	IntersectionCurve(std::vector<IntersectionPoint> _points, Openness isClosed,
		std::shared_ptr<Surface> _s1, std::shared_ptr<Surface> _s2);
	virtual ~IntersectionCurve();
	void Render() override;
	std::shared_ptr<CubicInterpolated> MakeInterpolatedCurve();
	void RenderMenu() override;
	glm::vec3 GetCenter() override;
	void Serialize(tinyxml2::XMLElement* scene) override {}
	std::vector<std::vector<float>> CalcTrimming(int lineCount, bool alongU, 
		bool isFirst, bool isReversed = false);
	unsigned int GetTexture(int isFirst) { return tex[isFirst]; }
	static void RenderPlot();
};


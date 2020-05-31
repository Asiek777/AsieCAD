#pragma once
#include "sceneObject.h"
#include "point.h"
#include "curve.h"

class BezierC0 :
	public Curve
{
	static int Number;
protected:
	std::unique_ptr<MeshBuffer> curveMesh;
	std::vector<unsigned int> indices;
	static std::vector<unsigned int> CalcIndices(int nodeCount);
public:
	//static glm::mat4 viewProjection;
	static std::unique_ptr<Shader> bezierShader;
	static std::unique_ptr<Shader> brokenShader;
	BezierC0();
	BezierC0(const char* _name);
	BezierC0(tinyxml2::XMLElement* data);
	void RenderCurve();
	void RenderBroken();
	void Render() override;
	void Serialize(tinyxml2::XMLElement* scene) override;
	static	void DrawBezierCurve(std::vector<glm::vec3>& nodes, glm::vec3 color = COLORS::BASE);
	static	void DrawBroken(std::vector<glm::vec3>& nodes, glm::vec3 color = COLORS::BROKEN);
};


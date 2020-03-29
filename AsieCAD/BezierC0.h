#pragma once
#include "sceneObject.h"
#include "point.h"
#include "curve.h"

class BezierC0 :
	public Curve
{
protected:
	static int Number;
	//std::unique_ptr<MeshBuffer> brokenMesh;
	std::unique_ptr<MeshBuffer> curveMesh;
	std::vector<unsigned int> indices;
public:
	static glm::mat4 viewProjection;
	static std::unique_ptr<Shader> bezierShader;
	static std::unique_ptr<Shader> brokenShader;
	BezierC0();
	BezierC0(const char* _name);
	void RenderCurve();
	void UpdateIndices();
	void RenderBroken();
	void Render() override;
};


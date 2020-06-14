#pragma once
#include "sceneObject.h"
#include "bezierPatch.h"

struct GregoryCorner{
	glm::vec3 p, e[2], f[2];	
};

class GregoryPatch : public SceneObject
{
	static int Number;
	static std::unique_ptr<Shader> patchShader;
	static std::unique_ptr<Shader> meshShader;
	
	std::weak_ptr<BezierPatch> patches[3];
	Border border[3];
	int index;
	bool showMesh = false;
	int curveCount[2] = {4, 4};
	GregoryCorner points[4];
	
	void UpdatePoints();
	std::vector<glm::vec3> CalcBezierHalf(std::vector<glm::vec3> curve);
	void RenderMesh();
public:
	GregoryPatch(std::shared_ptr<BezierPatch> _patches[3], Border _border[3], int index);
	void Render() override;
	void RenderMenu() override;
	void Serialize(tinyxml2::XMLElement* scene) override {}
	glm::vec3 GetCenter() override { return glm::vec3(0); }
};


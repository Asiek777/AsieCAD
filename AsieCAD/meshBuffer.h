#pragma once
#include <vector>
#include <xstring>
#include <glad/glad.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

class MeshBuffer
{
	static MeshBuffer* cubeBuffer;
	static MeshBuffer* slopeBuffer;


	unsigned int VBO, VAO, EBO, size;
public:
	unsigned int inline GetVAO() { return VAO; }
	MeshBuffer();
	MeshBuffer(std::vector<float> vertices, int vertSize);
	MeshBuffer(std::vector<float> vertices, bool hasEBO = false, 
	           std::vector<unsigned>  indices = {});
	MeshBuffer(std::vector<float> vertices, bool hasEBO, std::vector<unsigned> indices,
	           std::string s1);
	~MeshBuffer();

	void UpdateBuffer(std::vector<float> vertices);
	static std::vector<float> Vec3ToFloats(std::vector<glm::vec3>& vecs);
};


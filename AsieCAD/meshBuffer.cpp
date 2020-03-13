#include "meshBuffer.h"

MeshBuffer::MeshBuffer()
{
}


MeshBuffer::MeshBuffer(std::vector<float> vertices, bool hasEBO, 
	std::vector<unsigned> indices) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);	

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	if (hasEBO) {
		
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned),
			indices.data(), GL_STATIC_DRAW);
	}
	glBindVertexArray(0);

}


MeshBuffer::~MeshBuffer()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

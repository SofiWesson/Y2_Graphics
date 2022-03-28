#pragma once
#include "glm/glm.hpp"

class Mesh
{
public:
	Mesh() : m_triCount(0), m_vao(0), m_vbo(0), m_ibo(0) {}
	virtual ~Mesh();

	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 normal;
		glm::vec2 texCoord;
	};

	void InitialiseQuad();
	void InitialiseFullscreenQuad();
	void Initialise(unsigned int a_vertexCount, const Vertex* a_vertices, unsigned int a_indexCount = 0, unsigned int* a_indices = nullptr);
	
	virtual void Draw();

protected:
	unsigned int m_triCount
				, m_vao		// Vertex array object
				, m_vbo		// Vertex buffer object
				, m_ibo;	// Index buffer object

};
#include <gl_core_4_4.h>
#include "Mesh.h"

Mesh::~Mesh()
{
	// If the array does not equal zero, then delete the data

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

void Mesh::InitialiseQuad()
{
	// Check if the mesh is not initialised already

	assert(m_vao == 0);

	// Generate buffers
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);

	// Bind the vertex array, this is our mesh wrapper
	glBindVertexArray(m_vao);

	// Bind the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// define 6 vertices for our two triangles
	Vertex vertices[6];
	vertices[0].position = { -0.5,	0,	 0.5,	1 };
	vertices[1].position = {  0.5,	0,	 0.5,	1 };
	vertices[2].position = { -0.5,	0,	-0.5,	1 };

	vertices[3].position = { -0.5,	0,	-0.5,	1 };
	vertices[4].position = {  0.5,	0,	 0.5,	1 };
	vertices[5].position = { -0.5,	0,	-0.5,	1 };

	// Fill the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	// enable the first element as the position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// Now unbind the buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// This quad is made of 2 triangles
	m_triCount = 2;
}

void Mesh::Draw()
{
	glBindVertexArray(m_vao);

	// Are we actually using the indices or are we just using vertices
	if (m_ibo != 0)
		glDrawElements(GL_TRIANGLES, 3 * m_triCount, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, 3 * m_triCount);
}
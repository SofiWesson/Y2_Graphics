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
	vertices[5].position = {  0.5,	0,	-0.5,	1 };

	vertices[0].normal = { 0,	1,	0,	0 };
	vertices[1].normal = { 0,	1,	0,	0 };
	vertices[2].normal = { 0,	1,	0,	0 };
	vertices[3].normal = { 0,	1,	0,	0 };
	vertices[4].normal = { 0,	1,	0,	0 };
	vertices[5].normal = { 0,	1,	0,	0 };

	vertices[0].texCoord = { 0, 1 }; // bottom left
	vertices[1].texCoord = { 1, 1 }; // bottom right
	vertices[2].texCoord = { 0, 0 }; // top	left
	vertices[3].texCoord = { 0, 0 }; // top left
	vertices[4].texCoord = { 1, 1 }; // bottom right
	vertices[5].texCoord = { 1, 0 }; // top right


	// Fill the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	// enable the first element as the position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// Enable the second elecment as the normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)16);

	// Enable the third element as the texture coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)32);

	// Now unbind the buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// This quad is made of 2 triangles
	m_triCount = 2;
}

void Mesh::InitialiseFullscreenQuad()
{
	assert(m_vao == 0);

	// generate buffers
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);

	// then we bind the vertex array into a mesh wrapper
	glBindVertexArray(m_vao);

	// then bind the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// define the quad's vertices
	float vertices[] = {
		-1,  1, // left top
		-1, -1, // left bottom
		 1,  1, // rigtht top

		-1, -1, // left bottom
		 1, -1, // right bottom
		 1,  1  // right top
	};

	// then we fill out vertex buffer
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW);

	// now we enable the first element at the position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0);

	// then unbind the buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// this quad was made with two triangles
	m_triCount = 2;
}

void Mesh::Initialise(unsigned int a_vertexCount, const Vertex* a_vertices, unsigned int a_indexCount, unsigned int* a_indices)
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

	// Fill the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, a_vertexCount * sizeof(Vertex), a_vertices, GL_STATIC_DRAW);

	// Enable the first elecment as the position 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	if (a_indexCount != 0)
	{
		glGenBuffers(1, &m_ibo);

		// Bind the vertex buffers
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

		// Fill the vertex buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_indexCount * sizeof(unsigned int), a_indices, GL_STATIC_DRAW);
		m_triCount = a_indexCount / 3;
	}
	else
		m_triCount = a_vertexCount / 3;

	// Now unbind the buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
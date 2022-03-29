#include "ParticleEmitter.h"

#include <glm/ext.hpp>
#include <gl_core_4_4.h>

ParticleEmitter::ParticleEmitter() :
	m_particles(nullptr),
	m_firstDead(0),
	m_maxParticles(0),
	m_position(0, 0, 0),
	m_vao(0), m_vbo(0), m_ibo(0),
	m_vertexData(nullptr)
{
}

ParticleEmitter::~ParticleEmitter()
{
	delete[] m_particles;
	delete[] m_vertexData;

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

void ParticleEmitter::Initialise(unsigned int a_maxParticles, unsigned int a_emitRate,
	float a_lifetimeMin, float a_lifetimeMax,
	float a_velocityMin, float a_velocityMax,
	float a_startSize, float a_endSize,
	const glm::vec4& a_startColour, const glm::vec4& a_endColour)
{
	// first we want to set up the emitters 
	m_emitTimer = 0;
	m_emitRate = 1.f / a_emitRate;

	// then store all the arguments as our variables
	m_startColour = a_startColour;
	m_endColour = a_endColour;

	m_startSize = a_startSize;
	m_endSize = a_endSize;

	m_velocityMin = a_velocityMin;
	m_velocityMax = a_velocityMax;

	m_lifespanMin = a_lifetimeMin;
	m_lifespanMax = a_lifetimeMax;

	m_maxParticles = a_maxParticles;

	// next create the array of particles
	m_particles = new Particle[m_maxParticles];
	m_firstDead = 0;

	/* next we need to create an array of verticies for our particles,
	there needs to be 4 verticies to make a quad this data will be
	generated as we update the emitter*/
	m_vertexData = new ParticleVertex[m_maxParticles * 4];
	unsigned int* indexData = new unsigned int[m_maxParticles * 6];
	for (unsigned int i = 0; i < m_maxParticles; ++i)
	{
		indexData[i * 6 ]    = i * 4;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;
		indexData[i * 6 + 3] = i * 4;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}

	// finally, create the OpenGL buffers
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(ParticleVertex), m_vertexData, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(unsigned int), indexData, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0); // this is the position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);

	glEnableVertexAttribArray(1); // this is the colour of the particle
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indexData;
}

void ParticleEmitter::Emit()
{
	// check to see if there is an available pparticle for the system to emit
	if (m_firstDead >= m_maxParticles)
		return;

	// otherwise return the first dead particle
	Particle& particle = m_particles[m_firstDead++];

	// reset the particle of the returned particle
	particle.postion = m_position;

	// reset the lifespan of the returned particle, we will be randomising this by default
	particle.lifetime = 0;
	particle.lifespan = (rand() / (float)RAND_MAX) * (m_lifespanMax - m_lifespanMin) + m_lifespanMin;
	
	// reset the starting colour of the particle
	particle.colour = m_startColour;

	// reset the starting scale of the particle
	particle.size = m_startSize;

	/* reset the starting velocity of the particle, this will be
	randomised on each axis and the force provided */
	float velocity = (rand() / (float)RAND_MAX) * (m_velocityMax - m_velocityMin) + m_velocityMin;
	
	particle.velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;

	particle.velocity = glm::normalize(particle.velocity) * velocity;
}

void ParticleEmitter::Update(float a_dt, const glm::mat4& a_cameraTransform)
{
	/* this will moveand update all alive particles, then remove the dying particles 
	it will then emit the particles based on the emitters provided rate
	finally we will update the vertix array and construct the billboarding */
	m_emitTimer += a_dt;

	// spawn particles
	while (m_emitTimer > m_emitRate)
	{
		Emit();
		m_emitTimer -= m_emitRate;
	}

	unsigned int quad = 0;

	// now we need to update all of the particles to make sure they work as billboard quads
	for (unsigned int i = 0; i < m_firstDead; i++)
	{
		Particle* particle = &m_particles[i];
		
		particle->lifetime += a_dt;
		if (particle->lifetime >= particle->lifespan)
		{ // if true, repalce the last alive particle with this one
			*particle = m_particles[m_firstDead - 1];
			m_firstDead--;
		}
		else
		{
			// while false, allow the particle to change, first we move the particle
			particle->postion += particle->velocity * a_dt; // FYI you'll need to adjust this for gravity

			// next scale the particle
			particle->size = glm::mix(m_startSize, m_endSize, particle->lifetime / particle->lifespan);

			// then we can colour the particle
			particle->colour = glm::mix(m_startColour, m_endColour, particle->lifetime / particle->lifespan);

			// finally we will set up our quad using the correct position, colour, and scale
			float halfSize = particle->size * 0.5f;
			m_vertexData[quad * 4 + 0].position = glm::vec4(halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 0].colour = particle->colour;

			m_vertexData[quad * 4 + 1].position = glm::vec4(-halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 1].colour = particle->colour;

			m_vertexData[quad * 4 + 2].position = glm::vec4(-halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 2].colour = particle->colour;

			m_vertexData[quad * 4 + 3].position = glm::vec4(halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 3].colour = particle->colour;

			// set up our billboard's transform
			glm::vec3 zAxis = glm::normalize(glm::vec3(a_cameraTransform[3]) - particle->postion);
			glm::vec3 xAxis = glm::cross(glm::vec3(a_cameraTransform[1]), zAxis);
			glm::vec3 yAxis = glm::cross(zAxis, xAxis);

			glm::mat4 billboard(glm::vec4(xAxis, 0), glm::vec4(yAxis, 0),
								glm::vec4(zAxis, 0), glm::vec4(0, 0, 0, 1));

			m_vertexData[quad * 4].position     = billboard *
			m_vertexData[quad * 4].position     + glm::vec4(particle->postion, 0);

			m_vertexData[quad * 4 + 1].position = billboard *
			m_vertexData[quad * 4 + 1].position + glm::vec4(particle->postion, 0);

			m_vertexData[quad * 4 + 2].position = billboard *
			m_vertexData[quad * 4 + 2].position + glm::vec4(particle->postion, 0);

			m_vertexData[quad * 4 + 3].position = billboard *
			m_vertexData[quad * 4 + 3].position + glm::vec4(particle->postion, 0);

			++quad;
		}
	}
}

void ParticleEmitter::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDead * 4 * sizeof(ParticleVertex), m_vertexData);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_firstDead * 6, GL_UNSIGNED_INT, 0);
}
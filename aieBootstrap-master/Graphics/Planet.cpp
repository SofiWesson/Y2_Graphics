#include "Application.h"
#include "Planet.h"

#include "Gizmos.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

Planet::Planet()
{
}

Planet::Planet(glm::vec3 a_position, glm::vec3 a_rotation, glm::vec4 a_colour, float a_radius, bool a_direction, Planet* a_parent)
{
	m_position = a_position;
	m_rotation = a_rotation;
	m_colour = a_colour;
	m_radius = a_radius;
	m_direction = a_direction;
	m_parent = a_parent;
}

Planet::~Planet()
{

}

bool Planet::startup()
{
	return true;
}

void Planet::shutdown()
{
}

void Planet::Update(float a_dt)
{
	if (m_parent != nullptr)
	{
		float dist = glm::distance(m_position, m_parent->GetPosition());
		float penetration = m_distFromParent + m_radius - dist;
		
		if (penetration < 0)
		{
			
		}
		
		glm::vec3 normal = glm::normalize(m_position - m_parent->GetPosition());
		glm::vec3 impact = (m_velocity - m_parent->m_velocity) * normal;

		m_velocity += -impact;
		m_position += m_velocity * a_dt;
	}
}

void Planet::Draw()
{
	aie::Gizmos::addSphere(m_position, m_radius, 12, 12, m_colour);
}
#include "SolarSystem.h"
#include "Planet.h"

SolarSystem::SolarSystem()
{
	m_sun = new Planet(
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec4(1, .8f, 0, 1),
		1.5f,
		true, 
		nullptr);

	m_mercury = new Planet(
		glm::vec3(2.5f, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec4(.5f, .5f, .5f, 1),
		0.1f,
		true,
		m_sun);

	m_venus = new Planet(
		glm::vec3(3.5f, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec4(.5f, .5f, .4f, 1),
		0.2f,
		true,
		m_sun);

	m_earth = new Planet(
		glm::vec3(4.5f, 0, 0),
		glm::vec3(0, 0, 0), 
		glm::vec4(.1f, .8f, .9f, 1),
		0.25f, 
		true,
		m_sun);

	m_mars = new Planet(
		glm::vec3(5.5f, 0, 0), 
		glm::vec3(0, 0, 0), 
		glm::vec4(.9f, .2f, 0.1f, 1), 
		0.15f, 
		true,
		m_sun);

	m_jupiter = new Planet(
		glm::vec3(7, 0, 0), 
		glm::vec3(0, 0, 0), 
		glm::vec4(.6f, .6f, .5f, 1), 
		0.7f, 
		true,
		m_sun);

	m_saturn = new Planet(
		glm::vec3(9.5f, 0, 0), 
		glm::vec3(0, 0, 0), 
		glm::vec4(.9f, .9f, .6f, 1), 
		0.6f, 
		true,
		m_sun);

	m_uranus = new Planet(
		glm::vec3(11.5f, 0, 0), 
		glm::vec3(0, 0, 0), 
		glm::vec4(.6f, .7f, .8f, 1), 
		0.3f, 
		true,
		m_sun);

	m_neptune = new Planet(
		glm::vec3(12.5f, 0, 0), 
		glm::vec3(0, 0, 0), 
		glm::vec4(0, .1f, .6f, 1), 
		0.3f, 
		true,
		m_sun);

	m_planets.push_back(m_sun);
	m_planets.push_back(m_mercury);
	m_planets.push_back(m_venus);
	m_planets.push_back(m_earth);
	m_planets.push_back(m_mars);
	m_planets.push_back(m_jupiter);
	m_planets.push_back(m_saturn);
	m_planets.push_back(m_uranus);
	m_planets.push_back(m_neptune);
}

SolarSystem::~SolarSystem()
{
}

bool SolarSystem::startup()
{
	return true;
}

void SolarSystem::shutdown()
{
}

void SolarSystem::Update(float a_dt)
{
	for (Planet* planet : m_planets)
	{
		planet->Update(a_dt);
	}
}

void SolarSystem::Draw(glm::mat4 a_matrix)
{
	for (Planet* planet : m_planets)
	{
		planet->Draw();
	}
}

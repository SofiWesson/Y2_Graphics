#pragma once
#include "Application.h"
#include "Planet.h"

#include <list>

class Planet;

class SolarSystem
{
public:
	SolarSystem();
	virtual ~SolarSystem();

	virtual bool startup();
	virtual void shutdown();

	virtual void Update(float a_dt);
	virtual void Draw(glm::mat4 a_matrix);

private:
	std::list<Planet*> m_planets;

	Planet* m_sun = new Planet();
	Planet* m_mercury = new Planet();
	Planet* m_venus = new Planet();
	Planet* m_earth = new Planet();
	Planet* m_mars = new Planet();
	Planet* m_jupiter = new Planet();
	Planet* m_saturn = new Planet();
	Planet* m_uranus = new Planet();
	Planet* m_neptune = new Planet();

};
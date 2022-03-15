#pragma once

#include "Application.h"
#include "SolarSystem.h"
#include "Planet.h"

#include <glm/mat4x4.hpp>

class Application;
class SolarSystem;
class Planet;

class GraphicsApp : public aie::Application {
public:

	GraphicsApp();
	virtual ~GraphicsApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	SolarSystem* m_milkyWay;

	Planet* sun;

};
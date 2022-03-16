#pragma once

#include "Application.h"
#include "SolarSystem.h"
#include "Planet.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJMesh.h"

#include <glm/mat4x4.hpp>
#include <Input.h>

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

	glm::mat4 RotateMesh(glm::mat4 a_matrix, char a_axis, float a_radian);

protected:

	bool LaunchShaders();

	void LoadQuadMesh();
	void LoadBoxMesh();
	void LoadPyramidMesh();

	aie::ShaderProgram m_shader;

	// Generic quad data
	Mesh m_quadMesh;
	glm::mat4 m_quadTransform;

	// Box data
	Mesh m_boxMesh;
	glm::mat4 m_boxTransform;

	// Pyramid data
	Mesh m_pyramidMesh;
	glm::mat4 m_pyramidTransform;

	// Stanford bunny data
	aie::OBJMesh m_bunnyMesh;
	glm::mat4 m_bunnyTransform;

	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	SolarSystem* m_solarSystem = nullptr;

};
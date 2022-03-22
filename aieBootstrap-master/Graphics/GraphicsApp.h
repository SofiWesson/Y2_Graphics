#pragma once

#include "Application.h"
#include "Camera.h"
#include "SolarSystem.h"
#include "Planet.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJMesh.h"

#include <glm/mat4x4.hpp>
#include <Input.h>

class Application;
class Camera;
class FlyCamera;
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
	void LoadGridMesh();

	aie::Texture m_gridTexture;
	aie::Texture m_spearTexture;
	aie::Texture m_potionTexture;

	aie::ShaderProgram m_shader;
	aie::ShaderProgram m_phongShader;
	aie::ShaderProgram m_textureShader;
	aie::ShaderProgram m_normalMapShader;

	glm::mat4 m_modelTransform;

	// Generic quad data
	Mesh m_quadMesh;
	glm::mat4 m_quadTransform;

	// Box data
	Mesh m_boxMesh;
	glm::mat4 m_boxTransform;

	// Pyramid data
	Mesh m_pyramidMesh;
	glm::mat4 m_pyramidTransform;

	// Grid data
	Mesh m_gridMesh;
	glm::mat4 m_gridTransform;

	// Stanford bunny data
	aie::OBJMesh m_bunnyMesh;
	glm::mat4 m_bunnyTransform;

	// Soulspear data
	aie::OBJMesh m_spearMesh;
	glm::mat4 m_spearTransform;

	// Potion data
	aie::OBJMesh m_potionMesh;
	glm::mat4 m_potionTransform;

	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	Camera m_camera;
	FlyCamera m_flyCamera;

	SolarSystem* m_solarSystem = nullptr;

	struct Light
	{
		glm::vec3 direction;
		glm::vec3 colour;
	};

	Light m_light;
	glm::vec3 m_ambientLight;
};
#pragma once

#include "Application.h"
#include "Camera.h"
#include "SolarSystem.h"
#include "Planet.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJMesh.h"
#include "Instance.h"
#include "Scene.h"
#include "RenderTarget.h"
#include "ParticleEmitter.h"

#include <glm/mat4x4.hpp>
#include <Input.h>
#include <vector>

class Application;
class Camera;
class FlyCamera;
class SolarSystem;
class Planet;
class Scene;

enum class PostProcessEffects
{
	DEFAULT = 0,
	BOX_BLUR,
	DISTORT,
	EDGE_DETECTION,
	SEPIA,
	SCANLINES,
	GRAY_SCALE,
	INVERT,
	PIXELIZER,
	POSTERIZATION,
	DISTANCE_FOG,
	DEPTH_OF_FIELD
};

class GraphicsApp : public aie::Application {
public:

	GraphicsApp();
	virtual ~GraphicsApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	glm::mat4 RotateMesh(glm::mat4 a_matrix, char a_axis, float a_radian);

	glm::vec2 m_windowSizeLastFrame = glm::vec2(0.f, 0.f);

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
	aie::ShaderProgram m_postShader;

	aie::RenderTarget m_renderTarget;
	
	// for post-processing
	Mesh m_screenQuad;
	PostProcessEffects m_postProcessingEffect = PostProcessEffects::EDGE_DETECTION;
	glm::mat4 m_modelTransform;

	// list of basic meshes
	std::vector<Mesh> m_basicMeshes;
	std::vector<glm::mat4> m_basicMeshTransforms;

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

	Camera* m_camera = nullptr;
	FlyCamera m_flyCamera;

	SolarSystem* m_solarSystem = nullptr;

	glm::vec3 m_ambientLight;

	Scene* m_scene;

	ParticleEmitter* m_particleEmitter;
	aie::ShaderProgram m_particleShader;
	glm::mat4 m_particleTransform;

	void InitialiseOurParticles();
	void DrawOurParticles(glm::mat4 a_pvm);

	float m_dt = 0.f;
};
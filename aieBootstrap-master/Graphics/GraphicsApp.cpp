#include <imgui.h>

#include "GraphicsApp.h"
#include "Camera.h"
#include "SolarSystem.h"
#include "Planet.h"

#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <list>
#include <vector>
#include <gl_core_4_4.h>
#include <string>
//#include <glm/q_ea_conversion.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

GraphicsApp::GraphicsApp()
{
	
}

GraphicsApp::~GraphicsApp() {

}

bool GraphicsApp::startup() {
	
	glEnable(GL_DEPTH_TEST);

	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// initialise lights
	Light light;
	light.colour = { 1, 1, 1 };
	light.direction = { 1, -1, 1 };
	m_ambientLight = { .5f, .5f, .5f };
	
	// not being used
	_position = { 0, 0, 0 };
	_eulerAngles = { 0, 0, 0 };
	_scale = { 1,1,1 };
	_test = { 0, 0, 0, 0 }; // remove later


	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);

	m_camera = new Camera();
	m_camera->startup();

	m_scene = new Scene(m_camera, glm::vec2(getWindowWidth(), getWindowHeight()), light, m_ambientLight);

	m_scene->AddPointLights(glm::vec3(5, 3, 0), glm::vec3(1, 0, 0), 50);
	m_scene->AddPointLights(glm::vec3(-5, 3, 0), glm::vec3(0, 0, 1), 50);

	// m_solarSystem = new SolarSystem();

	m_basicMeshes.clear();
	m_basicMeshTransforms.clear();

	InitialiseOurParticles();

	return LaunchShaders();
}

void GraphicsApp::shutdown()
{
	Gizmos::destroy();
	delete m_scene;
	delete m_camera;
}

void GraphicsApp::update(float deltaTime)
{
	m_dt = deltaTime;

	// wipe the gizmos clean for this frame
	Gizmos::clear();
	
	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

	// Grab the time since the application started
	float time = getTime();

	// Rotate the light
	/*m_light.direction = glm::normalize(
		glm::vec3(glm::cos(time * 2), glm::sin(time * 2), 0));*/
	m_ambientLight = { .25f, .25f, .25f };

	if (m_solarSystem != nullptr)
		m_solarSystem->Update(deltaTime);

	m_camera->update(deltaTime);

	glm::vec2 windowSize((float)getWindowWidth(), (float)getWindowHeight());

	m_particleEmitter->Update(deltaTime, m_camera->GetTransform(m_camera->GetPosition(), glm::vec3(0), glm::vec3(1)));

	aie::Input* input = aie::Input::getInstance();

	/*if (input->isKeyDown(aie::INPUT_KEY_1) && !input->isKeyUp(aie::INPUT_KEY_1))
		m_camera.setPosition(m_camera.GetPosition() + glm::vec3(.01f, .01f, .01f));
	if (input->isKeyDown(aie::INPUT_KEY_2) && !input->isKeyUp(aie::INPUT_KEY_2))
		m_camera.setRotation(m_camera.GetRotation() + glm::vec3(.01f, .01f, .01f));
	if (input->isKeyDown(aie::INPUT_KEY_3) && !input->isKeyUp(aie::INPUT_KEY_3))
		m_camera.setScale(m_camera.GetScale() + glm::vec3(.01f, .01f, .01f));*/

#pragma region RotateBunny
	// rotaters the bunny transform
	if (input->isKeyDown(aie::INPUT_KEY_UP))
		m_bunnyTransform = RotateMesh(m_bunnyTransform, 'x', 0.1f);
	if (input->isKeyDown(aie::INPUT_KEY_DOWN))
		m_bunnyTransform = RotateMesh(m_bunnyTransform, 'x', -0.1f);
	
	if (input->isKeyDown(aie::INPUT_KEY_LEFT))
		m_bunnyTransform = RotateMesh(m_bunnyTransform, 'y', 0.1f);
	if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
		m_bunnyTransform = RotateMesh(m_bunnyTransform, 'y', -0.1f);
	
	if (input->isKeyDown(aie::INPUT_KEY_COMMA))
		m_bunnyTransform = RotateMesh(m_bunnyTransform, 'z', -0.1f);
	if (input->isKeyDown(aie::INPUT_KEY_PERIOD))
		m_bunnyTransform = RotateMesh(m_bunnyTransform, 'z', 0.1f);

#pragma endregion

	// quit if we press escape
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

#pragma region ImGUI Light Settings
	// ImGui light controls
	ImGui::Begin("Light Settings");
	ImGui::DragFloat3("Global Light Direction", &m_scene->GetGlobalLight().direction[0], 0.1f, -1.0f, 1.0f);
	ImGui::DragFloat3("Global Light Colour", &m_scene->GetGlobalLight().colour[0], 0.1f, 0.0f, 2.0f);
	ImGui::End();

#pragma endregion
#pragma region ImGUI Local Transform

	// Manipulate the transform of all Instanced objects

	ImGui::Begin("Debug");

	int i = 0;

	//std::list<Instance*> instances = m_scene->GetInstances();
	//for (auto iter = instances.begin(); iter != instances.end(); iter++)
	for (auto iter = m_scene->GetInstances().begin(); iter != m_scene->GetInstances().end(); iter++)
	{
		// instance id
		i++;
		std::string id = std::to_string(i);

		Instance* obj = *iter;

		ImGui::BeginGroup();
		ImGui::CollapsingHeader(("Object Transform " + id).c_str());
		
		glm::vec3 pos = obj->GetPosition();
		ImGui::DragFloat3(("Position " + id).c_str(), &pos.x, 0.1f, -100.0f, 100.0f);

		// matrices are ment to use quaternions, glm only lets you pass in radians,
		// and only lets you take out quaternions, this creates issues, use vec3 for rotation instead
		glm::vec3 rot = obj->GetRotation();
		ImGui::DragFloat3(("Rotation " + id).c_str(), &rot.x, 0.1f, -360.0f, 360.0f); // // gimble lock on y
		
		glm::vec3 scale = obj->GetScale();
		ImGui::DragFloat3(("Scale " + id).c_str(), &scale.x, 0.1f, -10.0f, 10.0f);

		// testing custom conversion script // remove later
		//_test = glm::eulerAngles_to_quat(obj->GetRotation());
		//ImGui::DragFloat4(("Quat Test" + id).c_str(), &_test.x, 0.01f, -1.0f, 1.0f);
		//rot = glm::quat_to_eulerAngles(_test);

		// adds changes to the transform of the object
		obj->SetTransform(obj->MakeTransform(pos, glm::vec3(0), scale));
		obj->SetRotation(rot);
		
		ImGui::EndGroup();
	}

	ImGui::End();

#pragma endregion

	m_windowSizeLastFrame = glm::vec2((float)getWindowWidth(), (float)getWindowHeight());
}

void GraphicsApp::draw()
{
	// We need to bind our render target first
	m_renderTarget.bind();

	// wipe the screen to the background colour
	clearScreen();

	// update perspective based on screen size
	glm::mat4 projectionMatrix = m_camera->getProjection((float)getWindowWidth(), (float)getWindowHeight());
	glm::mat4 viewMatrix = m_camera->getView();
	auto pvm = projectionMatrix * viewMatrix * glm::mat4(1);

	if (m_solarSystem != nullptr)
		m_solarSystem->Draw(projectionMatrix * viewMatrix);

	m_scene->Draw();

	// projection view matrix
	pvm = projectionMatrix * viewMatrix * m_particleTransform;

	DrawOurParticles(pvm);

#pragma region Simple Shader on Basic Meshes
	
	m_shader.bind();

	for (int i = 0; i < m_basicMeshTransforms.size(); i++)
	{
		m_modelTransform = m_basicMeshTransforms[i];

		pvm = projectionMatrix * viewMatrix * m_modelTransform;
		m_shader.bindUniform("ProjectionViewModel", pvm);

		m_basicMeshes[i].Draw();
	}

#pragma endregion
#pragma region View Quad

	// Unbind the target to return it to the back buffer
	//m_renderTarget.unbind();
	//clearScreen();

	m_textureShader.bind();

	m_modelTransform = m_quadTransform;

	pvm = projectionMatrix * viewMatrix * m_modelTransform;
	m_textureShader.bindUniform("ProjectionViewModel", pvm);
	m_textureShader.bindUniform("diffuseTexture", 0);

	//m_renderTarget.getTarget(0).bind(0);

	m_gridTexture.bind(0);
	m_quadMesh.Draw();

#pragma endregion
#pragma region Phong Shader

	m_phongShader.bind();
	m_phongShader.bindUniform("LightDirection", m_scene->GetGlobalLight().direction);
	m_phongShader.bindUniform("AmbientColour", m_scene->GetAmbientLight());
	m_phongShader.bindUniform("LightColour", m_scene->GetGlobalLight().colour);
	m_phongShader.bindUniform("CameraPosition", m_camera->GetPosition());

	pvm = projectionMatrix * viewMatrix * m_bunnyTransform;
	m_phongShader.bindUniform("ProjectionViewModel", pvm);
	m_phongShader.bindUniform("ModelMatrix", m_bunnyTransform);

	m_marbleTexture.bind(0);
	m_phongShader.bindUniform("seamlessTexture", 0);

	m_hatchingTexture.bind(1);
	m_phongShader.bindUniform("hatchingTexture", 1);

	m_rampTexture.bind(2);
	m_phongShader.bindUniform("rampTexture", 2);

	m_bunnyMesh.draw();

#pragma endregion

	Gizmos::draw(projectionMatrix * viewMatrix);

#pragma region Post-Processing Shader

	// Unbind the target to return it to the back buffer
	m_renderTarget.unbind();
	clearScreen();

	glm::vec2 windowSize = glm::vec2(getWindowWidth(), getWindowHeight());

	// bind the post-processing shader and texture
	m_postShader.bind();
	m_postShader.bindUniform("colourTarget", 0);
	m_postShader.bindUniform("postProcessTarget", (int)m_postProcessingEffect);
	m_postShader.bindUniform("screenSize", glm::vec2(getWindowWidth(), getWindowHeight()));
	m_postShader.bindUniform("deltaTime", m_dt);

	// opengl stuff for getting depth buffer
	if (m_postProcessingEffect == PostProcessEffects::DISTANCE_FOG)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LESS);
	}

	m_renderTarget.getTarget(0).bind(0);

	m_screenQuad.Draw();

#pragma endregion
}

glm::mat4 GraphicsApp::RotateMesh(glm::mat4 a_matrix, char a_axis, float a_radian)
{
	// old rotation function, not used anymore
	float cos = glm::cos(a_radian);
	float sin = glm::sin(a_radian);

	glm::mat4 tempMat;

	if(std::tolower(a_axis) == 'x')
	{
		tempMat = {
		1,   0,	  0,	0,
		0, cos, -sin,	0,
		0, sin,  cos,	0,
		0,	 0,    0,	1
		};
	}

	if (std::tolower(a_axis) == 'y')
	{
		tempMat = {
		cos,	0,	sin,	0,
		  0,	1,	  0,	0,
		-sin,	0,  cos,	0,
		  0,	0,    0,	1
		};
	}

	if (std::tolower(a_axis) == 'z')
	{
		tempMat = {
		cos, -sin,	0,	0,
		sin,  cos,	0,	0,
		0,	    0,	1,	0,
		0,		0,	0,	1
		};
	}

	return a_matrix * tempMat;
}

bool GraphicsApp::LaunchShaders()
{
	if (m_renderTarget.initialise(1, getWindowWidth(), getWindowHeight()) == false)
	{
		printf("Render Target Error!\n");
		return false;
	}

#pragma region Simple Shader

	m_shader.loadShader(aie::eShaderStage::VERTEX, "./Shaders/simple.vert");
	m_shader.loadShader(aie::eShaderStage::FRAGMENT, "./Shaders/simple.frag");
	if (m_shader.link() == false)
	{
		printf("Simple Shader Error: %s\n", m_shader.getLastError());
		return false;
	}

#pragma endregion
#pragma region Phong Shader

	m_phongShader.loadShader(aie::eShaderStage::VERTEX, "./Shaders/phongExt.vert");
	m_phongShader.loadShader(aie::eShaderStage::FRAGMENT, "./Shaders/phongExt.frag");
	if (m_phongShader.link() == false)
	{
		printf("Phong Shader Error: %s\n", m_phongShader.getLastError());
		return false;
	}

#pragma endregion
#pragma region Texture Shader

	m_textureShader.loadShader(aie::eShaderStage::VERTEX, "./Shaders/texture.vert");
	m_textureShader.loadShader(aie::eShaderStage::FRAGMENT, "./Shaders/texture.frag");
	if (m_textureShader.link() == false)
	{
		printf("Texture Shader Error: %s\n", m_textureShader.getLastError());
		return false;
	}

#pragma endregion
#pragma region Normal Map Shader

	m_normalMapShader.loadShader(aie::eShaderStage::VERTEX, "./Shaders/normalMap.vert");
	m_normalMapShader.loadShader(aie::eShaderStage::FRAGMENT, "./Shaders/normalMap.frag");
	if (m_normalMapShader.link() == false)
	{
		printf("Normal Map Shader Error: %s\n", m_normalMapShader.getLastError());
		return false;
	}

#pragma endregion
#pragma region Post Processing Shader
	m_postShader.loadShader(aie::eShaderStage::VERTEX, "./Shaders/advancedPost.vert");
	m_postShader.loadShader(aie::eShaderStage::FRAGMENT, "./Shaders/advancedPost.frag");
	if (m_postShader.link() == false)
	{
		printf("Post-processing shader has had an Error: %s\n", m_postShader.getLastError());
		return false;
	}
#pragma endregion
#pragma region Particle Shader

	m_particleShader.loadShader(aie::eShaderStage::VERTEX, "./Shaders/particle.vert");
	m_particleShader.loadShader(aie::eShaderStage::FRAGMENT, "./Shaders/particle.frag");
	if (m_particleShader.link() == false)
	{
		printf("Particle Shader has had an Error: %s\n", m_particleShader.getLastError());
		return false;
	}
	m_particleTransform = {
		.5f,  0,	0,	0,
		 0, .5f,	0,	0,
		 0,	 0, .5f,	0,
		 4,	 0,   4,	1
	};

#pragma endregion


#pragma region Textured Quad Mesh

	if (m_gridTexture.load("./textures/numbered_grid.tga") == false)
	{
		printf("Failed to load texture, please check file path!\n");
		return false;
	}
	m_quadMesh.InitialiseQuad();
	m_quadTransform = {
		10,  0,	0,	0,
		 0, 10,	0,	0,
		 0,	 0, 10,	0,
		 0,	 0,  0,	1
	};
	
#pragma endregion
#pragma region Bunny Mesh

	if (m_bunnyMesh.load("./stanford/Bunny.obj") == false)
	{
		printf("Bunny mesh error!\n");
		return false;
	}
	m_bunnyTransform = {
		0.1f,    0,    0,	0,
		   0, 0.1f,	   0,	0,
		   0,	 0, 0.1f,	0,
		  -4,	 0,    4,	1
	}; // this is 10 units large

#pragma endregion
#pragma region Spear Mesh

	if (m_spearMesh.load("./soulspear/soulspear.obj", true, true) == false)
	{
		printf("Spear mesh error!\n");
		return false;
	}
	m_spearTransform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	}; // this is 10 units large

#pragma endregion
#pragma region Potion Mesh

	if (m_potionMesh.load("./potion/Potion.obj", true, true) == false)
	{
		printf("potion mesh error!\n");
		return false;
	}
	m_potionTransform = {
		1, 0,  0, 0,
		0, 1,  0, 0,
		0, 0,  1, 0,
	   -4, 0, -4, 1
	};

#pragma endregion

	m_marbleTexture.load("./textures/marble2.jpg");
	m_hatchingTexture.load("./textures/ramp01.png");
	m_rampTexture.load("./textures/ramps.png", true);

	// create the fullscreen quad for post processing effects
	m_screenQuad.InitialiseFullscreenQuad();

	// can only have one :(
	//LoadQuadMesh();
	//LoadBoxMesh();
	//LoadPyramidMesh();
	//LoadGridMesh();

	// creating instances of spear
	for (int i = 0; i < 10; i++)
		m_scene->AddInstance(new Instance(glm::vec3(i * 2, 0, 0), glm::vec3(0, i * 30, 0), glm::vec3(1, 1, 1), &m_spearMesh, &m_normalMapShader));
	// creating instance of potion bottle
	m_scene->AddInstance(new Instance(m_potionTransform, &m_potionMesh, &m_normalMapShader));

	return true;
}

void GraphicsApp::LoadQuadMesh()
{
	const int numVertices = 4;
	const int numIndices = 6;

	// create vertices for quad mesh
	Mesh::Vertex vertices[numVertices];
	vertices[0].position = { -0.5f,  0,  0.5, 1 }; // bottom left
	vertices[1].position = {  0.5f,  0,  0.5, 1 }; // bottom front
	vertices[2].position = { -0.5f,  0, -0.5, 1 }; // bottom back
	vertices[3].position = {  0.5f,  0, -0.5, 1 }; // bottom right

	// form tris
	unsigned int indices[numIndices] = { 0, 1, 2, 2, 1, 3 };

	// initialise mesh
	m_quadMesh.Initialise(numVertices, vertices, numIndices, indices);
	m_quadTransform = {
		10,  0,	0,	0,
		 0, 10,	0,	0,
		 0,	 0, 10,	0,
		 0,	 0,  0,	1
	}; // this is 10 units large

	// add to lists to be easily used in mass
	m_basicMeshes.push_back(m_quadMesh);
	m_basicMeshTransforms.push_back(m_quadTransform);
}

void GraphicsApp::LoadBoxMesh()
{
	const int numVertices = 8;
	const int numIndices = 36;

	Mesh::Vertex vertices[numVertices];
	vertices[0].position = { -0.5f, -0.5,  0.5, 1 }; // bottom left
	vertices[1].position = {  0.5f, -0.5,  0.5, 1 }; // bottom front
	vertices[2].position = { -0.5f, -0.5, -0.5, 1 }; // bottom back
	vertices[3].position = {  0.5f, -0.5, -0.5, 1 }; // bottom right

	vertices[4].position = { -0.5f,  0.5,  0.5, 1 }; // top left
	vertices[5].position = {  0.5f,  0.5,  0.5, 1 }; // top front
	vertices[6].position = { -0.5f,  0.5, -0.5, 1 }; // top back
	vertices[7].position = {  0.5f,  0.5, -0.5, 1 }; // top right

	unsigned int indices[numIndices] = {
		3, 1, 2, 2, 1, 0, // bottom face
		7, 3, 6, 6, 3, 2, // back right
		6, 2, 4, 4, 2, 0, // back left
		4, 0, 5, 5, 0, 1, // front left
		5, 1, 7, 7, 1, 3, // front right
		7, 6, 5, 5, 6, 4  // top
	};

	m_boxMesh.Initialise(numVertices, vertices, numIndices, indices);
	m_boxTransform = {
		5,  0,	0,	0,
		 0, 5,	0,	0,
		 0,	 0, 5,	0,
		 0,	 0,  0,	1
	};

	m_basicMeshes.push_back(m_boxMesh);
	m_basicMeshTransforms.push_back(m_boxTransform);
}

void GraphicsApp::LoadPyramidMesh()
{
	const int numVertices = 5;
	const int numIndices = 18;

	Mesh::Vertex vertices[numVertices];
	vertices[0].position = { -0.5f, -0.5,  0.5, 1 }; // bottom left
	vertices[1].position = {  0.5f, -0.5,  0.5, 1 }; // bottom front
	vertices[2].position = { -0.5f, -0.5, -0.5, 1 }; // bottom back
	vertices[3].position = {  0.5f, -0.5, -0.5, 1 }; // bottom right
	vertices[4].position = {     0,  0.5,    0, 1 }; // top

	unsigned int indices[numIndices] = {
		3, 1, 2, 2, 1, 0, // bottom face
		4, 3, 2, // back right
		2, 0, 4, // back left
		4, 0, 1, // front left
		1, 3, 4  // front right
	};

	m_pyramidMesh.Initialise(numVertices, vertices, numIndices, indices);
	m_pyramidTransform = {
		5,  0,	0,	0,
		0,  5,	0,	0,
		0,	0,  5,	0,
		0,	0,  0,	1
	};

	m_basicMeshes.push_back(m_pyramidMesh);
	m_basicMeshTransforms.push_back(m_pyramidTransform);
}

void GraphicsApp::LoadGridMesh()
{
	const int numVertices = 88;
	const int numIndices = 132;

	int indicesCount = 0;

	Mesh::Vertex vertices[numVertices];
	unsigned int indices[numIndices];

	for (int i = 0; i < 11; i++)
	{
		float posOffset = 0.0f;

		vertices[i * 4].position     = { -0.5f,	.01,	  0.5f - posOffset, 1 }; // bottom left
		vertices[i * 4 + 1].position = {  0.5f,	.01,      0.5f - posOffset, 1 }; // bottom front
		vertices[i * 4 + 2].position = { -0.5f, .01,    0.498f - posOffset, 1 }; // bottom back
		vertices[i * 4 + 3].position = {  0.5f,	.01,	0.498f - posOffset, 1 }; // bottom right

		indices[indicesCount]     =     i * 4;
		indices[indicesCount + 1] = i * 4 + 1;
		indices[indicesCount + 2] = i * 4 + 2;

		indices[indicesCount + 3] = i * 4 + 2;
		indices[indicesCount + 4] = i * 4 + 1;
		indices[indicesCount + 5] = i * 4 + 3;

		indicesCount += 6;
		posOffset += 0.1f;
	}

	for (int i = 0; i < 11; i++)
	{
		float posOffset = 0.0f;

		vertices[(numVertices / 2) + i * 4].position     = {   0.5f - posOffset,  .01,  -0.5f, 1 }; // bottom left
		vertices[(numVertices / 2) + i * 4 + 1].position = {   0.5f - posOffset,  .01,   0.5f, 1 }; // bottom front
		vertices[(numVertices / 2) + i * 4 + 2].position = { 0.498f - posOffset,  .01,  -0.5f, 1 }; // bottom back
		vertices[(numVertices / 2) + i * 4 + 3].position = { 0.498f - posOffset,  .01,   0.5f, 1 }; // bottom right
	
		indices[indicesCount] = i * 4 + (numVertices / 2);
		indices[indicesCount + 1] = i * 4 + 1 + (numVertices / 2);
		indices[indicesCount + 2] = i * 4 + 2 + (numVertices / 2);
	
		indices[indicesCount + 3] = i * 4 + 2 + (numVertices / 2);
		indices[indicesCount + 4] = i * 4 + 1 + (numVertices / 2);
		indices[indicesCount + 5] = i * 4 + 3 + (numVertices / 2);
	
		indicesCount += 6;
		posOffset += 0.1f;
	}

	m_gridMesh.Initialise(numVertices, vertices, numIndices, indices);
	m_gridTransform = {
		10,  0,	0,	0,
		 0, 10,	0,	0,
		 0,	 0, 10,	0,
		 0,	 0,  0,	1
	}; // this is 10 units large

	m_basicMeshes.push_back(m_gridMesh);
	m_basicMeshTransforms.push_back(m_gridTransform);
}

void GraphicsApp::InitialiseOurParticles()
{
	m_particleEmitter = new ParticleEmitter();
	m_particleEmitter->Initialise(1000, 500, .1f, 1.f, 1.f, 10.f, 2.f, .1f,
		glm::vec4(0, 1, 1, 1), glm::vec4(1, 1, 0, 1));
}

void GraphicsApp::DrawOurParticles(glm::mat4 a_pvm)
{
	m_particleShader.bind();
	m_particleShader.bindUniform("ProjectionViewModel", a_pvm);
	m_particleEmitter->Draw();
}
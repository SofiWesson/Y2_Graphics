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
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	Light light;
	light.colour = { 1, 1, 1 };
	light.direction = { 1, -1, 1 };
	m_ambientLight = { 1, 1, 1 };

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);

	m_scene = new Scene(&m_camera, glm::vec2(getWindowWidth(), getWindowHeight()), light, m_ambientLight);

	m_scene->AddPointLights(glm::vec3(5, 3, 0), glm::vec3(1, 0, 0), 50);
	m_scene->AddPointLights(glm::vec3(-5, 3, 0), glm::vec3(0, 0, 1), 50);

	// m_solarSystem = new SolarSystem();

	return LaunchShaders();
}

void GraphicsApp::shutdown()
{
	Gizmos::destroy();
	delete m_scene;
}

void GraphicsApp::update(float deltaTime) {

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

	m_camera.update(deltaTime);

	aie::Input* input = aie::Input::getInstance();

	/*if (input->isKeyDown(aie::INPUT_KEY_1) && !input->isKeyUp(aie::INPUT_KEY_1))
		m_camera.setPosition(m_camera.GetPosition() + glm::vec3(.01f, .01f, .01f));
	if (input->isKeyDown(aie::INPUT_KEY_2) && !input->isKeyUp(aie::INPUT_KEY_2))
		m_camera.setRotation(m_camera.GetRotation() + glm::vec3(.01f, .01f, .01f));
	if (input->isKeyDown(aie::INPUT_KEY_3) && !input->isKeyUp(aie::INPUT_KEY_3))
		m_camera.setScale(m_camera.GetScale() + glm::vec3(.01f, .01f, .01f));*/

#pragma region RotateBunny

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

	// ImGui::Begin("Light Settings");
	// ImGui::DragFloat3("Global Light Direction", &m_light.direction[0], 0.1f, -1.0f, 1.0f);
	// ImGui::DragFloat3("Global Light Colour", &m_light.colour[0], 0.1f, 0.0f, 2.0f);
	// ImGui::End();

#pragma endregion
#pragma region ImGUI Local Transform

	ImGui::Begin("Local Transform");
	ImGui::DragFloat3("Position", &m_camera.GetPosition()[0], 0.1f, -1.0f, 1.0f);
	ImGui::DragFloat3("Rotation", &m_camera.GetRotation()[0], 0.1f, -1.0f, 1.0f);
	ImGui::DragFloat3("Scale", &m_camera.GetScale()[0], 0.1f, -1.0f, 1.0f);
	ImGui::End();

#pragma endregion

}

void GraphicsApp::draw()
{
	// We need to bind our render target first
	//m_renderTarget.bind();

	// wipe the screen to the background colour
	clearScreen();

	// update perspective based on screen size
	
	glm::mat4 projectionMatrix = m_camera.getProjection((float)getWindowWidth(), (float)getWindowHeight());
	glm::mat4 viewMatrix = m_camera.getView();
	auto pvm = projectionMatrix * viewMatrix * glm::mat4(1);

	if (m_solarSystem != nullptr)
		m_solarSystem->Draw(projectionMatrix * viewMatrix);

	m_scene->Draw();

	// Unbind the target to return it to the back buffer
	//m_renderTarget.unbind();

	//clearScreen();

#pragma region Quad

	m_textureShader.bind();

	m_modelTransform = m_quadTransform;

	pvm = projectionMatrix * viewMatrix * m_modelTransform;
	m_textureShader.bindUniform("ProjectionViewModel", pvm);
	m_textureShader.bindUniform("diffuseTexture", 0);

	//m_renderTarget.getTarget(0).bind(0);

	m_gridTexture.bind(0);
	m_quadMesh.Draw();

#pragma endregion

	// Draw the mesh
	// m_quadMesh.Draw();
	// m_boxMesh.Draw();
	// m_pyramidMesh.Draw();
	m_gridMesh.Draw();

	Gizmos::draw(projectionMatrix * viewMatrix);
}

glm::mat4 GraphicsApp::RotateMesh(glm::mat4 a_matrix, char a_axis, float a_radian)
{
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

	m_phongShader.loadShader(aie::eShaderStage::VERTEX, "./Shaders/Phong.vert");
	m_phongShader.loadShader(aie::eShaderStage::FRAGMENT, "./Shaders/Phong.frag");
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
		0.5f,    0,    0,	0,
		   0, 0.5f,	   0,	0,
		   0,	 0, 0.5f,	0,
		   0,	 0,     0,	1
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
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 2, 0, 1
	};

#pragma endregion

	//LoadQuadMesh();
	//LoadBoxMesh();
	//LoadPyramidMesh();
	LoadGridMesh();

	for (int i = 0; i < 10; i++)
		m_scene->AddInstance(new Instance(glm::vec3(i * 2, 0, 0), glm::vec3(0, i * 30, 0), glm::vec3(1, 1, 1), &m_spearMesh, &m_normalMapShader));

	return true;
}

void GraphicsApp::LoadQuadMesh()
{
	const int numVertices = 4;
	const int numIndices = 6;

	Mesh::Vertex vertices[numVertices];
	vertices[0].position = { -0.5f,  0,  0.5, 1 }; // bottom left
	vertices[1].position = {  0.5f,  0,  0.5, 1 }; // bottom front
	vertices[2].position = { -0.5f,  0, -0.5, 1 }; // bottom back
	vertices[3].position = {  0.5f,  0, -0.5, 1 }; // bottom right

	unsigned int indices[numIndices] = { 0, 1, 2, 2, 1, 3 };

	m_quadMesh.Initialise(numVertices, vertices, numIndices, indices);
	m_quadTransform = {
		10,  0,	0,	0,
		 0, 10,	0,	0,
		 0,	 0, 10,	0,
		 0,	 0,  0,	1
	}; // this is 10 units large
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
}

void GraphicsApp::LoadGridMesh()
{
	const int numVertices = 88;
	const int numIndices = 132;

	float posOffset = 0.0f;
	int indicesCount = 0;

	Mesh::Vertex vertices[numVertices];
	unsigned int indices[numIndices];

	for (int i = 0; i < 11; i++)
	{
		vertices[i * 4].position =     { -0.5f,	.01,	  0.5f - posOffset, 1 }; // bottom left
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

	for (int i = 0; i < 11; i++) // crashes
	{
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
}
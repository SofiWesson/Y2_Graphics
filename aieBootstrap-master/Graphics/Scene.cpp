#include "Scene.h"
#include "Instance.h"

Scene::Scene(Camera* a_camera, glm::vec2 a_windowSize, Light& a_light, glm::vec3 a_ambientLight) :
	m_camera(a_camera), m_windowSize(a_windowSize), m_globalDirLight(a_light), m_ambientLight(a_ambientLight)
{
}

Scene::~Scene()
{
	for (auto it = m_instances.begin(); it != m_instances.end(); it++)
	{
		delete (*it);
	}
}

void Scene::AddInstance(Instance* a_instance)
{
	m_instances.push_back(a_instance);
}

void Scene::Draw()
{
	for (int i = 0; i < MAX_LIGHTS && i < GetNumLights(); i++)
	{
		m_pointLightPositions[i] = m_pointLights[i].direction;
		m_pointLightColours[i] = m_pointLights[i].colour;
	}

	for (auto it = m_instances.begin(); it != m_instances.end(); it++)
	{
		Instance* instance = *it;
		instance->Draw(this);
	}
}
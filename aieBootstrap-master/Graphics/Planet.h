#pragma once
#include "Application.h"

#include "glm/glm.hpp"

class Planet
{
public:
	Planet();
	Planet(glm::vec3 a_position, glm::vec3 a_rotation, glm::vec4 a_colour, float a_radius, bool a_direction, Planet* a_parent);
	virtual ~Planet();

	virtual bool startup();
	virtual void shutdown();
		
	virtual void Update(float a_dt);
	virtual void Draw();

	void SetPosition(glm::vec3 a_position) { m_position = a_position; }
	void SetRotation(glm::vec3 a_rotation) { m_rotation = a_rotation; }
	void SetColour(glm::vec4 a_colour) { m_colour = a_colour; }
	void SetRadius(float a_radius) { m_radius = a_radius; }
	void SetDistFromParent(float a_distFromParent) { m_distFromParent = a_distFromParent; }
	void SetDirection(bool a_direction) { m_direction = a_direction; }
	void SetParent(Planet* a_parent) { m_parent = a_parent; }

	glm::vec3 GetPosition() { return m_position; }
	glm::vec3 GetRotation() { return m_rotation; }
	glm::vec4 GetColour() { return m_colour; }
	float GetRadius() { return m_radius; }
	float GetDistFromParent() { return m_distFromParent; }
	bool GetDirection() { return m_direction; }
	Planet* GetParent() { return m_parent; }

private:
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	glm::vec3 m_rotation = glm::vec3(0, 0, -90);
	glm::vec3 m_velocity = glm::vec3(0, 0, -10);
	glm::vec4 m_colour = glm::vec4(0, 0, 0, 0);
	float m_radius = 0.f;
	float m_distFromParent = 2.5f;
	bool m_direction = true; // true = clockwise

	Planet* m_parent = nullptr;

};


#pragma once
#include "glm/glm.hpp"
#include "glm/ext.hpp"

class Camera
{
public:
	Camera()
	{
		m_theta = 0;
		m_phi = 0;
		setPosition(glm::vec3(-10, 2, 0));
		setRotation(glm::vec3(0, 0, 0));
		setScale(glm::vec3(1, 1, 1));
	}
	~Camera() {}

	virtual void update(float a_dt);
	void setPerspective(float a_fieldOfView, float a_aspectRatio, float a_near, float a_far);
	void setLookAt(glm::vec3 a_from, glm::vec3 a_to, glm::vec3 a_up);
	void setPosition(glm::vec3 a_position);
	void setRotation(glm::vec3 a_rotation);
	void setScale(glm::vec3 a_scale);
	glm::vec3 GetPosition() { return m_position; }
	glm::vec3 GetRotation() { return m_rotation; }
	glm::vec3 GetScale() { return m_scale; }
	glm::mat4 getLocalTransform() { return m_localTransform; }
	glm::mat4 getWorldTransform();
	glm::mat4 getView();
	glm::mat4 getProjection(float w, float h);
	glm::mat4 getProjectionView(float w, float h);

private:
	void updateProjectionViewTransform();

	glm::mat4 m_worldTransform;
	glm::mat4 m_viewTransform;
	glm::mat4 m_projectionTransform;
	glm::mat4 m_projectionViewTransform;

	float m_theta; // z rotation
	float m_phi; // x rotation

	glm::vec3 m_position; // local
	glm::vec3 m_rotation; // local
	glm::vec3 m_scale;	  // local

	glm::mat4 m_localTransform {
		1, 0, 0, 0, // 0
		0, 1, 0, 0,	// 1
		0, 0, 1, 0, // 2
		0, 0, 0, 1  // 3
	};//x  y  z  w

};

class FlyCamera : Camera
{
public:
	FlyCamera()
	{
		m_theta = 0;
		m_phi = 0;
		m_position = glm::vec3(-10, 2, 0);
		m_lastMouseX = 0;
		m_lastMouseY = 0;
	}
	virtual ~FlyCamera() {}

	virtual void update(float a_dt);

	glm::vec3 GetPosition() { return m_position; }
	void SetSpeed(float a_speed) { m_speed = a_speed; }

	glm::mat4 getView();
	glm::mat4 getProjection(float w, float h);

private:
	float m_theta;
	float m_phi;

	glm::vec3 m_position;

	glm::vec3 m_up;

	float m_speed;

	float m_lastMouseX;
	float m_lastMouseY;
};
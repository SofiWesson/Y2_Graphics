#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Input.h"

#include <Windows.h>

#pragma region FlyCamera

void FlyCamera::update(float a_dt)
{
}

glm::mat4 FlyCamera::getView()
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	return glm::lookAt(m_position, m_position + forward, glm::vec3(0, 1, 0));
}

glm::mat4 FlyCamera::getProjection(float w, float h)
{
	return glm::perspective(glm::pi<float>() * 0.25f, w / h, 0.1f, 1000.f);
}

#pragma endregion
#pragma region Camera

Camera::Camera()
{
}

Camera::~Camera()
{
}

bool Camera::startup()
{
	m_theta = 0;
	m_phi = 0;
	setPosition(glm::vec3(-10, 2, 0));
	setRotation(glm::vec3(0, 0, 0));
	setScale(glm::vec3(1, 1, 1));

	return true;
}

void Camera::shutdown()
{
}

void Camera::update(float a_dt)
{
	aie::Input* input = aie::Input::getInstance();

	MoveCamera(input, a_dt);
}

void Camera::MoveCamera(aie::Input* a_input, float a_dt)
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);

	// Calculate the forwards and right axis and up axis for the camera
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	glm::vec3 right(-sin(thetaR), 0, cos(thetaR));
	glm::vec3 up(0, 1, 0);

	// use WASD, ZX keys to move camera around
	if (a_input->isKeyDown(aie::INPUT_KEY_W))
		m_position += forward * a_dt;
	if (a_input->isKeyDown(aie::INPUT_KEY_S))
		m_position -= forward * a_dt;
	if (a_input->isKeyDown(aie::INPUT_KEY_A))
		m_position -= right * a_dt;
	if (a_input->isKeyDown(aie::INPUT_KEY_D))
		m_position += right * a_dt;

	if (a_input->isKeyDown(aie::INPUT_KEY_X))
		m_position += up * a_dt;
	if (a_input->isKeyDown(aie::INPUT_KEY_Z))
		m_position -= up * a_dt;

	// get current mouse coordinates
	float mx = a_input->getMouseX();
	float my = a_input->getMouseY();
	const float turnSpeed = .1f;

	// if the right button us down, increment theta and phi
	if (a_input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		m_theta += turnSpeed * (mx - m_lastMouseX);
		m_phi += turnSpeed * (my - m_lastMouseY);
	}

	m_lastMouseX = mx;
	m_lastMouseY = my;
}

void Camera::setPerspective(float a_fieldOfView, float a_aspectRatio, float a_near, float a_far)
{
}

void Camera::setLookAt(glm::vec3 a_from, glm::vec3 a_to, glm::vec3 a_up)
{
}

void Camera::setPosition(glm::vec3 a_position)
{
	m_localTransform[0].w = a_position.x;
	m_localTransform[1].w = a_position.y;
	m_localTransform[2].w = a_position.z;

	m_position = a_position;
}

void Camera::setRotation(glm::vec3 a_rotation)
{
#pragma region X Rotation

	float cosX = cos(glm::radians(a_rotation.x));
	float sinX = sin(glm::radians(a_rotation.x));

	m_localTransform[1].y = cosX - GetScale().y;
	m_localTransform[1].z = -sinX;
	m_localTransform[2].y = sinX;
	m_localTransform[2].z = cosX + GetScale().z;

#pragma endregion
#pragma region Y Rotation

	float cosY = cos(glm::radians(a_rotation.y));
	float sinY = sin(glm::radians(a_rotation.y));

	m_localTransform[0].x = cosY + GetScale().x;
	m_localTransform[0].z = sinY;
	m_localTransform[2].x = -sinY;
	m_localTransform[2].z = cosY - GetScale().z;

#pragma endregion
#pragma region Z Rotation

	float cosZ = cos(glm::radians(a_rotation.z));
	float sinZ = sin(glm::radians(a_rotation.z));

	m_localTransform[0].x = cosZ - GetScale().x;
	m_localTransform[0].y = -sinZ;
	m_localTransform[1].x = sinZ;
	m_localTransform[1].y = cosZ - GetScale().y;

#pragma endregion

	m_rotation = a_rotation;
}

void Camera::setScale(glm::vec3 a_scale)
{
	m_localTransform[0].x + a_scale.x;
	m_localTransform[1].y + a_scale.y;
	m_localTransform[2].z + a_scale.z;

	m_scale = a_scale;
}

glm::vec3 Camera::GetPosition()
{
	glm::vec3 temp;

	temp.x = m_localTransform[0].w;
	temp.y = m_localTransform[1].w;
	temp.z = m_localTransform[2].w;

	return m_position;
}

glm::vec3 Camera::GetRotation()
{
	glm::vec3 vTemp;
	glm::mat4 tempMat {
		1, 0, 0, 0, // 0
		0, 1, 0, 0,	// 1
		0, 0, 1, 0, // 2
		0, 0, 0, 1  // 3
	};//x  y  z  w
	
	glm::vec3 scale = GetScale();

	// extract rotation into separate matrix from transform matrix
	tempMat[0].x = m_localTransform[0].x / scale.x; // 0x
	tempMat[0].y = m_localTransform[0].y / scale.y; // 0y
	tempMat[0].z = m_localTransform[0].z / scale.z; // 0z

	tempMat[1].x = m_localTransform[1].x / scale.x; // 1x
	tempMat[1].y = m_localTransform[1].y / scale.y;	// 1y
	tempMat[1].z = m_localTransform[1].z / scale.z; // 1z

	tempMat[2].x = m_localTransform[2].x / scale.x; // 2x
	tempMat[2].y = m_localTransform[2].y / scale.y;	// 2y
	tempMat[2].z = m_localTransform[2].z / scale.z;	// 2z

	// safety check for divide by 0
	float trace = tempMat[0].x + tempMat[1].y + tempMat[2].z;
	
	// Get quaternoins from rotation matrix
	glm::vec4 q = { 0.f, 0.f, 0.f, 0.f }; // quaternion
	if (trace > 0)
	{
		float s = glm::sqrt(trace + 1.f) * 2.f;
		q.w = .25f * s;
		q.x = (tempMat[2].y - tempMat[1].z) / s;
		q.y = (tempMat[0].z - tempMat[2].x) / s;
		q.z = (tempMat[1].x - tempMat[0].y) / s;
	}
	else if (tempMat[0].x > tempMat[1].y && tempMat[0].x > tempMat[2].z)
	{
		float s = glm::sqrt(1.f + tempMat[0].x - tempMat[1].y - tempMat[2].z) * 2;
		q.w = (tempMat[2].y - tempMat[1].z) / s;
		q.x = .25f * s;
		q.y = (tempMat[0].y + tempMat[1].x) / s;
		q.z = (tempMat[0].z + tempMat[2].x) / s;
	}
	else if (tempMat[1].y > tempMat[2].z)
	{
		float s = glm::sqrt(1.f + tempMat[1].y - tempMat[0].x - tempMat[2].z) * 2;
		q.w = (tempMat[0].z - tempMat[2].x) / s;
		q.x = (tempMat[0].y + tempMat[1].x) / s;
		q.y = .25f * s;
		q.z = (tempMat[1].z + tempMat[2].y) / s;
	}
	else
	{
		float s = sqrt(1.f + tempMat[2].z - tempMat[0].x - tempMat[1].y) * 2;
		q.w = (tempMat[1].x - tempMat[0].y) / s;
		q.x = (tempMat[0].z + tempMat[2].x) / s;
		q.y = (tempMat[1].z + tempMat[2].y) / s;
		q.z = .25f * s;
	}

	return m_rotation;
}

glm::vec3 Camera::GetScale()
{
	glm::vec3 temp;

	glm::vec3 x(m_localTransform[0].x, m_localTransform[1].x, m_localTransform[2].x);
	glm::vec3 y(m_localTransform[0].y, m_localTransform[1].y, m_localTransform[2].y);
	glm::vec3 z(m_localTransform[0].z, m_localTransform[1].z, m_localTransform[2].z);

	temp.x = glm::length(x);
	temp.y = glm::length(y);
	temp.z = glm::length(z);

	return m_scale;
}

glm::mat4 Camera::GetTransform(glm::vec3 a_position, glm::vec3 a_eulerAngles, glm::vec3 a_scale)
{
	return glm::translate(glm::mat4(1), a_position)
		* glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.z), glm::vec3(0, 0, 1))
		* glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.y), glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.x), glm::vec3(1, 0, 0))
		* glm::scale(glm::mat4(1), a_scale);
}

glm::mat4 Camera::getWorldTransform()
{
	glm::mat4 tempMat {
		1, 0, 0, 0, // 0
		0, 1, 0, 0,	// 1
		0, 0, 1, 0, // 2
		0, 0, 0, 1  // 3
	};//x  y  z  w

	return m_localTransform;
}

glm::mat4 Camera::getView()
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	return glm::lookAt(m_position, m_position + forward, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::getProjection(float w, float h)
{
	return glm::perspective(glm::pi<float>() * 0.25f, w / h, 0.1f, 1000.f);
}

glm::mat4 Camera::getProjectionView(float w, float h)
{
	return getView() * getProjection(w, h);
}

void Camera::updateProjectionViewTransform()
{
}

#pragma endregion
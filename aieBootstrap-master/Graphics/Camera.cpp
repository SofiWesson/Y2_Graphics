#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Input.h"
#include <Windows.h>

#pragma region FlyCamera

void FlyCamera::update(float a_dt)
{
	aie::Input* input = aie::Input::getInstance();
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);

	// Calculate the forwards and right axis and up axis for the camera
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	glm::vec3 right(-sin(thetaR), 0, cos(thetaR));
	glm::vec3 up(0, 1, 0);

	// use WASD, ZX keys to move camera around
	if (input->isKeyDown(aie::INPUT_KEY_W))
		m_position += forward * a_dt;
	if (input->isKeyDown(aie::INPUT_KEY_S))
		m_position -= forward * a_dt;
	if (input->isKeyDown(aie::INPUT_KEY_A))
		m_position -= right * a_dt;
	if (input->isKeyDown(aie::INPUT_KEY_D))
		m_position += right * a_dt;

	if (input->isKeyDown(aie::INPUT_KEY_X))
		m_position += up * a_dt;
	if (input->isKeyDown(aie::INPUT_KEY_Z))
		m_position -= up * a_dt;

	// get current mouse coordinates
	float mx = input->getMouseX();
	float my = input->getMouseY();
	const float turnSpeed = .1f;

	// if the right button us down, increment theta and phi
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		m_theta += turnSpeed * (mx - m_lastMouseX);
		m_phi += turnSpeed * (my - m_lastMouseY);
	}

	/*int screenX, screenY;
	int width, height;
	int x, y;
	
	RECT rect = { NULL };
	if (GetWindowRect(GetConsoleWindow(), &rect))
	{
		screenX = rect.left;
		screenY = rect.top;
	
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	
		x = screenX + width / 2;
		y = screenY + height / 2;
	}*/

	m_lastMouseX = mx;
	m_lastMouseY = my;
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

void Camera::update(float a_dt)
{
}

void Camera::setPerspective(float a_fieldOfView, float a_aspectRatio, float a_near, float a_far)
{
}

void Camera::setLookAt(glm::vec3 a_from, glm::vec3 a_to, glm::vec3 a_up)
{
}

void Camera::setPosition(glm::vec3 a_position)
{
	glm::mat4 tempMat{
		1, 0, 0, a_position.x,
		0, 1, 0, a_position.y,
		0, 0, 1, a_position.z,
		0, 0, 0, 1
	};

	m_localTransform *= tempMat;

	m_position = a_position;
}

void Camera::setRotation(glm::vec3 a_rotation)
{
	float _cos = 0;
	float _sin = 0;

	glm::mat4 tempMat;

#pragma region X Rotation

	_cos = cos(glm::radians(a_rotation.x));
	_sin = sin(glm::radians(a_rotation.x));

	tempMat = {
		1,    0,     0, 0,
		0, _cos, -_sin, 0,
		0, _sin,  _cos, 0,
		0,	  0,     0, 1 
	};

	m_localTransform *= tempMat;

#pragma endregion
#pragma region Y Rotation

	_cos = cos(glm::radians(a_rotation.y));
	_sin = sin(glm::radians(a_rotation.y));

	tempMat = {
		 _cos, 0, _sin, 0,
		    0, 1,    0, 0,
		-_sin, 0, _cos, 0,
		    0, 0,    0, 1
	};

	m_localTransform *= tempMat;

#pragma endregion
#pragma region Z Rotation

	_cos = cos(glm::radians(a_rotation.z));
	_sin = sin(glm::radians(a_rotation.z));

	tempMat = {
		_cos, -_sin, 0, 0,
		_sin,  _cos, 0, 0,
		   0,     0, 1, 0,
		   0,     0, 0, 1
	};

	m_localTransform *= tempMat;

#pragma endregion

	m_rotation = a_rotation;
}

void Camera::setScale(glm::vec3 a_scale)
{
	glm::mat4 tempMat{
		a_scale.x,		   0,		  0, 0,
		        0, a_scale.y,		  0, 0,
				0,		   0, a_scale.z, 0,
				0,		   0,		  0, 1 
	};

	m_localTransform *= tempMat;

	m_scale = a_scale;
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
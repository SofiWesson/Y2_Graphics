#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Input.h"
#include <Windows.h>


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

glm::mat4 FlyCamera::getViewMatrix()
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	return glm::lookAt(m_position, m_position + forward, glm::vec3(0, 1, 0));
}

glm::mat4 FlyCamera::getProjectionMatrix(float w, float h)
{
	return glm::perspective(glm::pi<float>() * 0.25f, w / h, 0.1f, 1000.f);
}

// ============================================================================================================

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
}

glm::mat4 Camera::getWorldTransform()
{
	glm::mat4 tempMat;

	float
		a = 1, b = 0, c = 0, d = 0,
		e = 0, f = 1, g = 0, h = 0,
		i = 0, j = 0, k = 1, l = 0,
		m = 0, n = 0, o = 0, p = 1;

	float cos = glm::cos(glm::radians(m_theta));
	float sin = glm::sin(glm::radians(m_phi));

	// x rotation
	f += cos;
	g += -sin;
	j += sin;
	k += cos;

	// y rotation


	tempMat = {
	a, b, c, d,
	e, f, g, h,
	i, j, k, l,
	m, n, o, p
	};

	m_worldTransform *= tempMat;

	return m_worldTransform;
}

glm::mat4 Camera::getView()
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	glm::vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	return glm::lookAt(m_position, m_position + forward, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::getProtection(float w, float h)
{
	return glm::perspective(glm::pi<float>() * 0.25f, w / h, 0.1f, 1000.f);
}

glm::mat4 Camera::getProjectionView(float w, float h)
{
	return getView() * getProtection(w, h);
}

void Camera::updateProjectionViewTransform()
{
}

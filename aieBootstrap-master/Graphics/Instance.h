#pragma once
#include "glm/glm.hpp"
#include <glm/ext.hpp>

class SimpleCamera;
struct Light;
class Scene;

namespace aie
{
	class OBJMesh;
	class ShaderProgram;
}

class Instance
{
public:
	Instance(
		glm::mat4 a_transform,
		aie::OBJMesh* a_mesh,
		aie::ShaderProgram* a_shader
	);

	Instance(
		glm::vec3 a_position,
		glm::vec3 a_eulerAngles,
		glm::vec3 a_scale,
		aie::OBJMesh* a_mesh,
		aie::ShaderProgram* a_shader
	);

	glm::mat4 GetTransform() { return m_transform; }
	void SetTransform(glm::mat4 a_transform) { m_transform = a_transform; }

	glm::vec3 GetPosition() { return glm::vec3(m_transform[3].x, m_transform[3].y, m_transform[3].z); }
	void SetPosition(glm::vec3 a_position);

	glm::vec3 GetRotation() { // glms rotation in matrix is scuffed
		return m_rotation;

		// glm::quat quat = glm::quat_cast(m_transform);
		// return glm::vec3(glm::degrees(glm::pitch(quat)), glm::degrees(glm::yaw(quat)), glm::degrees(glm::roll(quat)));
	}
	void SetRotation(glm::vec3 a_eulerAngles) {
		m_rotation = a_eulerAngles;	
		
		// m_transform =
		// glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.x), glm::vec3(1, 0, 0)) *
		// glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.y), glm::vec3(0, 1, 0)) *
		// glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.z), glm::vec3(0, 0, 1));
	}

	glm::vec3 GetScale() { return glm::vec3(
		glm::length(glm::vec3(m_transform[0][0], m_transform[0][1], m_transform[0][2])),
		glm::length(glm::vec3(m_transform[1][0], m_transform[1][1], m_transform[1][2])),
		glm::length(glm::vec3(m_transform[2][0], m_transform[2][1], m_transform[2][2]))); }
	void SetScale(glm::vec3 a_scale) { m_transform = glm::scale(glm::mat4(1), a_scale); }

	void Draw(Scene* a_scene);

	glm::mat4 MakeTransform(glm::vec3 a_position, glm::vec3 a_eulerAngles, glm::vec3 a_scale);

protected:
	glm::vec3 m_rotation; // possibly temp

	glm::mat4 m_transform;
	aie::OBJMesh* m_mesh;
	aie::ShaderProgram* m_shader;

};


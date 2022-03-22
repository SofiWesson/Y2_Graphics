#pragma once
#include "glm/glm.hpp"

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

	void Draw(Scene* a_scene);

	static glm::mat4 MakeTransform(
		glm::vec3 a_position, glm::vec3 a_eulerAngles, glm::vec3 a_scale);

protected:
	glm::mat4 m_transform;
	aie::OBJMesh* m_mesh;
	aie::ShaderProgram* m_shader;

};


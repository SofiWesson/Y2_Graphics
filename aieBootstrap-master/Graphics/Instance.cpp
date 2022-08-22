#include "Instance.h"
#include "Scene.h"
#include "OBJMesh.h"
#include "Camera.h"
#include "GraphicsApp.h"

#include <Texture.h>
#include <glm/ext.hpp>

Instance::Instance(glm::mat4 a_transform, aie::OBJMesh* a_mesh, aie::ShaderProgram* a_shader) :
    m_transform(a_transform), m_mesh(a_mesh), m_shader(a_shader)
{
}

Instance::Instance(glm::vec3 a_position, glm::vec3 a_eulerAngles, glm::vec3 a_scale, aie::OBJMesh* a_mesh, aie::ShaderProgram* a_shader) :
    m_mesh(a_mesh), m_shader(a_shader)
{
    m_rotation = a_eulerAngles;
    m_transform = MakeTransform(a_position, glm::vec3(0), a_scale);
}

void Instance::Draw(Scene* a_scene)
{
    // set the render pipeline
    m_shader->bind();

    // bind the transforms and other relevant uniforms
    auto pvm = a_scene->GetCamera()->getProjection(a_scene->GetWindowSize().x, a_scene->GetWindowSize().y) * a_scene->GetCamera()->getView() * m_transform;
    m_shader->bindUniform("ProjectionViewModel", pvm);
    m_shader->bindUniform("ModelMatrix", m_transform);

    // bind the lighting and camera uniforms
    m_shader->bindUniform("AmbientColour", a_scene->GetAmbientLight());
    m_shader->bindUniform("LightColour", a_scene->GetGlobalLight().colour);
    m_shader->bindUniform("LightDirection", a_scene->GetGlobalLight().direction);

    m_shader->bindUniform("CameraPosition", a_scene->GetCamera()->GetPosition());

    int numberOfLights = a_scene->GetNumLights();
    m_shader->bindUniform("numberOfLights", numberOfLights);
    m_shader->bindUniform("PointLightPositions", numberOfLights, a_scene->GetPointLightPositions());
    m_shader->bindUniform("PointLightColours", numberOfLights, a_scene->GetPointLightColour());

    // draw the mesh
    m_mesh->draw();
}

glm::mat4 Instance::MakeTransform(glm::vec3 a_position, glm::vec3 a_eulerAngles, glm::vec3 a_scale)
{
    m_rotation = a_eulerAngles;

    glm::vec3 rot = glm::vec3(0);

    // add position, rotation, and scale into a single matrix
    return glm::translate(glm::mat4(1), a_position)
        * glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.x), glm::vec3(1, 0, 0))
        * glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.y), glm::vec3(0, 1, 0))
        * glm::rotate(glm::mat4(1), glm::radians(a_eulerAngles.z), glm::vec3(0, 0, 1))
        * glm::scale(glm::mat4(1), a_scale);

}
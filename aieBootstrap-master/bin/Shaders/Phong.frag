#version 410

in vec4 vPosition;
in vec3 vNormal;

uniform vec3 CameraPosition;

uniform vec3 Ka; // The ambient material colour
uniform vec3 Kd; // The diffuse meterial colour
uniform vec3 Ks; // The specular material colour
uniform float specularPower; // The specular power of the material

uniform vec3 AmbientColour;
uniform vec3 LightColour;
uniform vec3 LightDirection;

out vec4 FragColour;

void main()
{
    // Make sure the normal and light direction are normalised
    vec3 N = normalize(vNormal);
    vec3 L = normalize(LightDirection);

    // Then calculate the lambert term (negative light direction)
    float lambertTerm = max(0, min(1, dot(N, -L)));

    vec3 V = normalize(CameraPosition - vPosition.xyz);
    vec3 R = reflect(L, N);

    float specularTerm = pow(specularPower, max(0, dot(R, V)));

    // Calculate the diffuse, ambient, ambient and specular colour of the model
    vec3 ambient = AmbientColour * Ka;
    vec3 diffuse = LightColour * Kd * lambertTerm;
    vec3 specular = LightColour * Ks * specularTerm;

    FragColour = vec4(ambient + diffuse + specular, 1);
}
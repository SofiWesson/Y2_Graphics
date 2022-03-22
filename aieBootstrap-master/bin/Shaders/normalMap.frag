// a simple normal map colour shader
#version 410

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vTangent;
in vec3 vBiTangent;

uniform vec3 CameraPosition;

uniform vec3 Ka; // The ambient material colour
uniform vec3 Kd; // The diffuse meterial colour
uniform vec3 Ks; // The specular material colour
uniform float specularPower; // The specular power of the material

uniform vec3 AmbientColour;
uniform vec3 LightColour;
uniform vec3 LightDirection;

const int MAX_LIGHTS = 4;
uniform int numberOfLights;
uniform vec3 PointLightColours[MAX_LIGHTS];
uniform vec3 PointLightPositions[MAX_LIGHTS];

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

out vec4 FragColour;

vec3 Diffuse(vec3 direction, vec3 colour, vec3 normal)
{
    return colour * max(0, dot(normal, -direction));
}

vec3 Specular(vec3 direction, vec3 colour, vec3 normal, vec3 view)
{
    vec3 R = reflect(direction, normal);
    float specularTerm = pow(max(0, dot(R, view)), specularPower);
    return specularTerm * colour;
}

void main()
{
    // Make sure the normal and light direction are normalised
    vec3 N = normalize(vNormal);
    vec3 L = normalize(LightDirection);
    vec3 T = normalize(vTangent);
    vec3 B = normalize(vBiTangent);

    mat3 TBN = mat3(T, B, N);

    vec3 texDiffuse = texture(diffuseTexture, vTexCoord).rgb;
    vec3 texSpecular = texture(specularTexture, vTexCoord).rgb;
    vec3 texNormal = texture(normalTexture, vTexCoord).rgb;

    N = TBN * (texNormal * 2 - 1);

    // Then calculate the lambert term (negative light direction)
    float lambertTerm = max(0, min(1, dot(N, -L)));

    // calculate the diffuse lighting from the global source
    vec3 diffuseTotal = Diffuse(L, LightColour, N);

    vec3 V = normalize(CameraPosition - vPosition.xyz);
    vec3 R = reflect(L, N);

    // calulate the specular light from the global source
    vec3 specularTotal = Specular(L, LightColour, N, V);

    for (int i = 0; i < numberOfLights; i++)
    {
        vec3 direction = vPosition.xyz - PointLightPositions[i];
        float distance = length(direction);
        direction = direction / distance;

        vec3 colour = PointLightColours[i] / (distance * distance);

        diffuseTotal += Diffuse(direction, colour, N);
        specularTotal += Specular(direction, colour, N, V);
    }

    float specularTerm = pow(max(0, dot(R, V)), specularPower);

    // Calculate the diffuse, ambient, ambient and specular colour of the model
    vec3 ambient = AmbientColour * Ka * texDiffuse;
    vec3 diffuse = Kd * texDiffuse * diffuseTotal;
    vec3 specular = Ks * texSpecular * specularTerm;

    FragColour = vec4(ambient + diffuse + specular, 1);
}
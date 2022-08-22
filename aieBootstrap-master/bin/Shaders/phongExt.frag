#version 410

in vec4 vPosition;
in vec3 vNormal;
in vec4 objPosition;

uniform sampler2D seamlessTexture;
uniform sampler2D hatchingTexture;
uniform sampler2D rampTexture;
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

    float specularTerm = pow(max(0, dot(R, V)), specularPower);

    vec4 sampleXY = texture(seamlessTexture, objPosition.xy * 0.1f);
    vec4 sampleYZ = texture(seamlessTexture, objPosition.yz * 0.1f);
    vec4 sampleXZ = texture(seamlessTexture, objPosition.xz * 0.1f);
    vec4 sampleTP = N.x * N.x * sampleYZ + N.y * N.y * sampleXZ + N.z * N.z * sampleXY;
    sampleTP = vec4(ivec4(sampleTP * 4 + 0.5f)) / 4;

    float intensity = AmbientColour.r + lambertTerm + specularTerm;

    vec4 hatching = texture(hatchingTexture, gl_FragCoord.xy / 5);
    intensity = step(hatching.x, intensity);

    // Calculate the diffuse, ambient, ambient and specular colour of the model
    vec3 ambient = AmbientColour * Ka;
    vec3 diffuse = (AmbientColour + step(vec3(0.5f, 0.5f, 0.5f), LightColour * lambertTerm)) * sampleTP.xyz;
    vec3 specular = LightColour * Ks * step(0.1f, specularTerm);

    float rim = 1 - max(0, dot(N, V));
    rim = rim * rim * rim;
    float alpha = rim + specular.r;

    // saturate the colour // saturate properly
    vec3 saturate = ambient + diffuse + specular + rim + intensity;
    saturate = clamp(saturate, vec3(0, 0, 0), vec3(1, 1, 1));

    vec4 fragColour = vec4(saturate, alpha) + texture(rampTexture, vec2(lambertTerm, 0.03f));
    fragColour = clamp(fragColour, vec4(0, 0, 0, 0), vec4(1, 1, 1, 1));

    FragColour = fragColour;
    //FragColour = vec4(diffuse + specular, 1);
    //FragColour = vec4(intensity * vec3(1, 1, 1) * sampleTP.xyz, 1);
    //FragColour = texture(rampTexture, vec2(lambertTerm, 0.03f));
}
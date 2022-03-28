// a simple post processing shader
#version 410

in vec2 vTexCoord;

uniform sampler2D colourTarget;

out vec4 FragColour;

// return the out of the colours as the data was originally provided
vec4 Default(vec2 texCoord)
{
    return texture(colourTarget, texCoord);
}

vec4 BoxBlur(vec2 texCoord)
{
    vec2 texel = 1.0f / textureSize(colourTarget, 0);
    vec4 colour = texture(colourTarget, texCoord);

    colour += texture(colourTarget, texCoord + texel * vec2(-1, 1));
    colour += texture(colourTarget, texCoord + texel * vec2(-1, 0));
    colour += texture(colourTarget, texCoord + texel * vec2(-1, -1 ));

    colour += texture(colourTarget, texCoord + texel * vec2(0, 1));
    colour += texture(colourTarget, texCoord + texel * vec2(0, -1 ));

    colour += texture(colourTarget, texCoord + texel * vec2(1, 1));
    colour += texture(colourTarget, texCoord + texel * vec2(1, 0));
    colour += texture(colourTarget, texCoord + texel * vec2(1, -1 ));

    return colour / 9;
}

vec4 Distort(vec2 texCoord)
{
    vec2 mid = vec2(0.5f);
    float distanceFromCentre = distance(texCoord, mid);
    vec2 normalizedCoord = normalize(texCoord - mid);
    float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.05f;
    vec2 newCoord = mid + bias * normalizedCoord;

    return texture(colourTarget, newCoord);
}

vec4 Sepia(vec2 texCoord)
{
    vec4 baseColour = texture(colourTarget, texCoord);
    vec4 sepia = vec4(
        dot(baseColour.rgb, vec3(0.393f, 0.769f, 0.189f)),
        dot(baseColour.rgb, vec3(0.349f, 0.686f, 0.168f)),
        dot(baseColour.rgb, vec3(0.272f, 0.534f, 0.131f)),
        1.0f);

    return sepia;
}

void main()
{
    // first calculate the texel's size
    vec2 texSize = textureSize(colourTarget, 0);
    vec2 texelSize = 1.0f / texSize;

    // adjust the scale and coordinates
    vec2 scale = (texSize - texelSize) / texSize;
    vec2 texelCoord = vTexCoord / scale + texelSize * 0.5f;

    // this will output the desired post processing effect
    FragColour = Sepia(texelCoord) * Distort(texelCoord) * BoxBlur(texelCoord);
}
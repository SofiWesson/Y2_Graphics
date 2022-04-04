// a simple post processing shader
#version 410

in vec2 vTexCoord;

uniform sampler2D colourTarget;
uniform int postProcessTarget;
uniform vec2 screenSize;
uniform float deltaTime;

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

vec4 EdgeDetection(vec2 texCoord)
{
    vec3 diffuse = texture(colourTarget, texCoord.st).rgb;
    mat3 I;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vec3 vSample = texelFetch(colourTarget, ivec2(gl_FragCoord) + ivec2(i - 1, j - 1), 0).rgb;
            I[i][j] = length(vSample);
        }
    }

    mat3 sx = mat3(
        1.0f, 2.0f, 1.0f,
        0.0f, 0.0f, 0.0f,
        -1.0f, -2.0f, -1.0f
    );
    mat3 sy = mat3(
        1.0f, 0.0f, -1.0f,
        2.0f, 0.0f, -2.0f,
        1.0f, 0.0f, -1.0f
    );

    float gx = dot(sx[0], I[0]) + 
               dot(sx[1], I[1]) +
               dot(sx[2], I[2]);

    float gy = dot(sy[0], I[0]) +
               dot(sy[1], I[1]) +
               dot(sy[2], I[2]);

    float g = sqrt(pow(gx, 2.0f) + pow(gy, 2.0f));

    return vec4(diffuse - vec3(g), 1.0f);
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

vec4 Scanlines(vec2 texCoord) // not working still
{
    float density = 1.3f;
    float scanlineOpacity = 0.3;
    float noiseOpacity = 0.2;
    float flickering = 0.03;

    vec2 uv = texCoord / screenSize.xy;
    vec3 col = texture(colourTarget, uv).rgb;

    float count = screenSize.y * density;
    vec2 sl = vec2(sin(uv.y * count), cos(uv.y * count));
    vec3 scanlines = vec3(sl.x, sl.y, sl.x);

    float rand = fract(sin(dot(uv * deltaTime, vec2(12.9898, 78.233))) * 43758.5453123);

    col += col * scanlines * scanlineOpacity;
    col += col * rand * noiseOpacity;
    col += col * sin(110.0 * deltaTime) * flickering;
    col += texture(colourTarget, texCoord).rgb;

    return vec4(col, 1.0f);
}

vec4 GreyScale(vec2 texCoord)
{
    vec4 baseColour = texture(colourTarget, texCoord);

    float grey = dot(baseColour.rgb, vec3(0.5f, 0.5f, 0.5f));

    return vec4(grey, grey, grey, 1.0f);
}

vec4 Invert(vec2 texCoord)
{
    vec4 baseColour = texture(colourTarget, texCoord);
    vec4 invert = vec4(
        1.0f - baseColour.r,
        1.0f - baseColour.g,
        1.0f - baseColour.b,
        1.0f);

    return invert;
}

vec4 Pixelizer(vec2 texCoord)
{
    float pixelation = 5.0f;

    float dx = pixelation * (1.0f / screenSize.x);
    float dy = pixelation * (1.0f / screenSize.y);

    vec2 coord = vec2(dx * floor(texCoord.x / dx), dy * floor(texCoord.y / dy));

    return texture(colourTarget, coord);
}

vec4 Posterization(vec2 texCoord)
{
    float levels = 10.0f;
    vec4 colour = texture(colourTarget, texCoord);
    float greyscale = max(colour.r, max(colour.g, colour.b));

    float lower = floor(greyscale * levels) / levels;
    float lowerDiff = abs(greyscale - lower);

    float upper = ceil(greyscale * levels) / levels;
    float upperDiff = abs(upper - greyscale);

    float level = lowerDiff <= upperDiff ? lower : upper;
    float adjustment = level / greyscale;

    return vec4(colour.rgb * adjustment, 1.0f);
}

vec4 DistanceFog(vec2 texCoord)
{
    // float fogMaxDist = 5.0f;
    // float fogMinDist = 0.0f;
    // float density = 2.0f;
    // vec4 fogColour = vec4(0.8f, 0.8f, 0.8f, 1.0f);
    // 
    // vec2 mid = vec2(0.5f);
    // float distanceFromCentre = distance(texCoord, mid);

    // //vec3 objPosition = vec3(transform[0][3], transform[1][3], transform[2][3]);

    // float dist = distance(vec3(distanceFromCentre, distanceFromCentre, 0.0f), vec3(distanceFromCentre, distanceFromCentre,density));

    // float fogFactor = (fogMaxDist - dist) / (fogMaxDist - fogMinDist);
    // fogFactor = clamp(fogFactor, 0.0f, 1.0f);

    // vec4 colour = texture(colourTarget, texCoord);
    // vec4 outputColour = mix(fogColour, colour, fogFactor);
    // 
    // return outputColour;

    // https://learnopengl.com/Advanced-OpenGL/Depth-testing

    float near 0.1;
    float far = 100.0;

    return vec4(vec3(gl_FragCoord.z).xyz, 1.0);
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
    switch (postProcessTarget)
    {
        case 0: // Default
        {
            FragColour = Default(texelCoord);
            break;
        }
        case 1: // Box Blur
        {
            FragColour = BoxBlur(texelCoord);
            break;
        }
        case 2: // Distort
        {
            FragColour = Distort(texelCoord);
            break;
        }
        case 3: // Edge Detection
        {
            FragColour = EdgeDetection(texelCoord);
            break;
        }
        case 4: // Sepia
        {
            FragColour = Sepia(texelCoord);
            break;
        }
        case 5: // Scanlines
        {
            FragColour = Scanlines(texelCoord); // TODO all below
            break;
        }
        case 6: // Grey Scale
        {
            FragColour = GreyScale(texelCoord);
            break;
        }
        case 7: // Invert
        {
            FragColour = Invert(texelCoord);
            break;
        }
        case 8: // Pixelizer 
        {
            FragColour = Pixelizer(texelCoord);
            break;
        }
        case 9: // Posterization
        {
            FragColour = Posterization(texelCoord);
            break;
        }
        case 10: // Distance Fog
        {
            FragColour = DistanceFog(texelCoord);
            break;
        }
        case 11: // Depth of Field
        {
            FragColour = Default(texelCoord);
            break;
        }
    }
}
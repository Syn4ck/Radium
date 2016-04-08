out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D texA;
uniform sampler2D texB;

void main()
{
    vec3 color = texture(texA, varTexcoord).rgb + texture(texB, varTexcoord).rgb;
    fragColor = vec4(color, 1.0);
}


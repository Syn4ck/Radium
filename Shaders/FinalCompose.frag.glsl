out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D texA;
uniform sampler2D texB;

void main()
{
    vec3 color = texelFetch(texA, ivec2(gl_FragCoord), 0).rgb
               + texelFetch(texB, ivec2(gl_FragCoord), 0).rgb;
    fragColor = vec4(color, 1.0);
}


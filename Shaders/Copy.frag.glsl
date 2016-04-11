in  vec2 varTexcoord;
out vec4 fragColor;

uniform sampler2D base;

void main()
{
    fragColor = texture(base, varTexcoord);
}

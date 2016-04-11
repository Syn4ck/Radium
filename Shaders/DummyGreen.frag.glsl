
out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D color;

void main()
{
    vec4 fragtex = texture(color, varTexcoord).xyzw;
    fragColor = vec4(fragtex.x, 1.0, fragtex.zw);
}

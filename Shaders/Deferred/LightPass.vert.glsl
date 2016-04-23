layout (location = 0) in vec3 inPosition;
layout (location = 4) in vec3 inTexcoord;

out vec2 vTexcoord;

// FIXME(charly): Keep this shader for now, but it should be replaced
void main()
{
    gl_Position = vec4(inPosition, 1.0);
    vTexcoord = inPosition.xy * 0.5 + 0.5;
}

// Test to be deleted - does never go out of branch hugo
// has bravely and loyally served into many silly tests

out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D color;
uniform vec3      add;

void main()
{
    fragColor = texelFetch(color, ivec2(gl_FragCoord), 0).xyzw + vec4(add, 0);
}

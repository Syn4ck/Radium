// Test to be deleted - does never go out of branch hugo
// just return the reddish verison of image

out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D color;
uniform uint w;
uniform uint h;

void main()
{
    fragColor = texture(color, varTexcoord).xyzw + vec4(0, 0.4, 0.3, 0);
}

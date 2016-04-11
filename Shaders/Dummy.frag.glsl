// Test to be deleted - does never go out of branch hugo
// just return the reddish verison of image

out vec4 fragColor;

in vec2 varTexcoord;

uniform sampler2D color;

void main()
{
    fragColor = vec4(0.6, texture(color, varTexcoord).yzw);
}

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 4) in vec3 inTexcoord;

#include "../Structs.glsl"

uniform Transform transform;

out VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
} vs_out;

void main()
{
    mat4 mvp = transform.proj * transform.view * transform.model;
    gl_Position = mvp * vec4(inPosition, 1.0);

    vec4 pos = transform.model * vec4(inPosition, 1.0);
    pos /= pos.w;
    vec4 normal = transform.worldNormal * vec4(inNormal, 0.0);

    vs_out.position = vec3(pos);
    vs_out.normal = vec3(normal);
    vs_out.texcoord = vec3(inTexcoord);
}

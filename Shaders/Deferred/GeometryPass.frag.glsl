#include "../Structs.glsl"

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 texcoord;
} fs_in;

layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 albedo;
layout (location = 3) out vec4 specular;

uniform Material material;

#include "../Helpers.glsl"

void main()
{
    position = vec4(fs_in.position, 1.0);
    normal = vec4(normalize(fs_in.normal), 1.0);
    albedo = vec4(getKd(), 1.0);
    specular = vec4(getKs(), getNs());
}

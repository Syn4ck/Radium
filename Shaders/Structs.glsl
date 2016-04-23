struct Transform
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 mvp;
    mat4 modelView;
    mat4 worldNormal;
    mat4 viewNormal;
};

struct Textures
{
    int hasKd;
    int hasKs;
    int hasNs;
    int hasNormal;
    int hasAlpha;

    sampler2D kd;
    sampler2D ks;
    sampler2D ns;
    sampler2D normal;
    sampler2D alpha;
};

struct Material
{
    vec4 kd;
    vec4 ks;

    float ns;

    Textures tex;
};

struct Attenuation
{
    float constant;
    float linear;
    float quadratic;
};

struct Light
{
    int type;

    vec4 color;
    vec3 position;
    vec3 direction;

    Attenuation attenuation;

    float innerAngle;
    float outerAngle;
};

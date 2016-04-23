#include "../Structs.glsl"

in vec2 vTexcoord;

out vec4 fColor;

uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uDiffuse;
uniform sampler2D uSpecular;

uniform mat4 uView;

uniform Light light;

uniform int ambientLight;

vec3 getKd() { return texture(uDiffuse, vTexcoord).rgb; }
vec3 getKs() { return texture(uSpecular, vTexcoord).rgb; }
float getNs() { return texture(uSpecular, vTexcoord).a; }
vec3 getPosition() { return texture(uPosition, vTexcoord).xyz; }
vec3 getNormal() { return texture(uNormal, vTexcoord).xyz; }
vec3 getEye() { return -vec3(uView[3].xyz * mat3(uView)); }

const float Pi = 3.14159265;

vec3 blinnPhongInternal(vec3 d, vec3 n)
{
    vec3 direction = normalize(d);
    vec3 normal = normalize(n);

    // http://www.thetenthplanet.de/archives/255 / VortexEngine BlinnPhong
    float Ns = getNs();
    vec3 Kd = getKd() / Pi;
    float normalization = ((Ns + 2) * (Ns + 4)) / (8 * Pi * (exp2(-Ns * 0.5) + Ns));
    vec3 Ks = getKs() * normalization;

    float diffFactor = max(dot(normal, -direction), 0.0);
    vec3 diff = diffFactor * light.color.xyz * Kd;

    vec3 viewDir = normalize(getPosition() - getEye());
    vec3 halfVec = normalize(viewDir + direction);
    float specFactor = pow(max(dot(normal, -halfVec), 0.0), Ns);
    vec3 spec = specFactor * light.color.xyz * Ks;

    return diff + spec;
}

vec3 blinnPhongSpot()
{
    vec3 dir = -light.direction;
    vec3 color;

    float d = length(dir);
    float attenuation = light.attenuation.constant +
                        light.attenuation.linear * d +
                        light.attenuation.quadratic * d * d;
    attenuation = 1.0 / attenuation;

    vec3 lightToPixel = normalize(getPosition() - light.position);

    float cosRealAngle = dot(lightToPixel, normalize(dir));
    float cosSpotOuter = cos(light.innerAngle / 2.0);

    float radialAttenuation = pow(clamp((cosRealAngle - cosSpotOuter) /
                                        (1.0 - cosSpotOuter), 0.0, 1.0), 1.6);

    color = blinnPhongInternal(dir, getNormal());
    return color * attenuation * radialAttenuation;
}

vec3 blinnPhongPoint()
{
    vec3 dir = getPosition() - light.position;
    vec3 color = blinnPhongInternal(normalize(dir), getNormal());

    float d = length(dir);
    float attenuation = light.attenuation.constant +
                        light.attenuation.linear * d +
                        light.attenuation.quadratic * d * d;

    return color / attenuation;
}

vec3 blinnPhongDirectional()
{
    return blinnPhongInternal(light.direction, getNormal());
}

vec3 computeLighting()
{
    if (ambientLight == 1)
    {
        return getKd() * 0.1;
    }
    else
    {
        switch (light.type)
        {
        case 0:  return blinnPhongPoint();
        case 1:  return blinnPhongDirectional();
        case 2:  return blinnPhongSpot();
        default: return vec3(0);
        }
    }
}

void main()
{
    vec3 color = computeLighting();

//    vec3 color = vec3(dot(getNormal(), light.position - getPosition())) * 0.1;
//    if (length(light.position - getPosition()) > 10.0)
//    {
//        color = vec3(0);
//    }
//    color = getNormal();

    fColor = vec4(color, 1.0);
}

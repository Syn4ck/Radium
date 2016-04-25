in vec2  varTexcoord;
out vec4 fragColor;

uniform sampler2D hdr;
uniform sampler2D lum;
uniform uint pingpongsz;

#include "Tonemap.glsl"

void main()
{
    vec3 color = texture(hdr, varTexcoord).rgb;

    // get lum. values
    vec2 varNullcoord = vec2(0,0);
    vec3  lumvec  = texelFetch(lum, ivec2(0,0), 0).xyz;
    float lumMin  = lumvec.x;
    float lumMax  = lumvec.y;
    float lumMean = exp(lumvec.z / (pingpongsz * pingpongsz));

    vec3 Yxy = rgb2Yxy(color);

    float middleGrey = getMiddleGrey(lumMean);
    float lumScaled  = getLumScaled(Yxy.r, middleGrey, lumMean);

    float white = 1.0;

    // TODO(charly): Uniforms ?
    float T = 0.1;
    float O = 10.0;

    float lumThreshold = max(lumScaled * (1 + lumScaled) - T, 0.0);
    float lumBright = lumThreshold / (O + lumThreshold);

    Yxy.r = lumBright;
    color = Yxy2rgb(Yxy);

    if (isnan(color.r) || isnan(color.g) || isnan(color.b))
        color = vec3(0.0) ;


    fragColor = vec4(color, 1.0);
}
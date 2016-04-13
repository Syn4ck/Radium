in vec2  varTexcoord;
out vec4 fragColor;

uniform sampler2D hdr;
uniform sampler2D lum;
uniform float     pingpongsz;

#include "Tonemap.glsl"

void main()
{
    // get lum. values
    vec3 lum = texelFetch(lum, ivec2(0,0), 0).xyz;
    
    float lumMin  = lum.x;
    float lumMax  = lum.y;
    float lumMean = exp(lum.z / pow(pingpongsz, 2));

    vec3 color = texelFetch(hdr, ivec2(gl_FragCoord.xy), 0).rgb;

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

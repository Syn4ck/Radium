in vec2 varTexcoord;

out vec4 fragColor;

uniform sampler2D hdr;
uniform sampler2D lum;

uniform uint pingpongsz;

#include "Tonemap.glsl"

void main()
{
    // extract luminance data
    vec3 lumvec   = texelFetch(lum, ivec2(0,0), 0).rgb;
    float lumMin  = lumvec.x;
    float lumMax  = lumvec.y;
    float lumMean = exp(lumvec.z / (pingpongsz * pingpongsz));

    vec3 color = texelFetch(hdr, ivec2(gl_FragCoord), 0).rgb;

    vec3 Yxy = rgb2Yxy(color);

    // FIXME(charly): 2.0 might be too high with many lights (or very shiny ones). Reinhard's suggestion is 0.18. VortexEngine uses 1.03
    float middleGrey = getMiddleGrey(lumMean);
    float lumScaled = getLumScaled(Yxy.r, middleGrey, lumMean);
    float white = max(2 * lumMean, lumMax);
    float lumCompressed = (lumScaled * (1.0 + lumScaled / (white * white))) / (1.0 + lumScaled);
    Yxy.r = lumCompressed;

    color = Yxy2rgb(Yxy);

    fragColor = vec4(color, 1.0);
}

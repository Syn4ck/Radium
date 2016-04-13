in vec2 varTexcoord;
out vec4 fragColor;

uniform sampler2D color;
uniform vec2 offset;

uniform int wView;
uniform int hView;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

bool isInside(vec2 coord)
{
    return (coord.x >= 5) && (coord.x < wView - 5)  // TODO(Hugo) remove that
        && (coord.y >= 5) && (coord.y < hView - 5);
}

void main()
{
    vec3 cbase = texelFetch(color, ivec2(gl_FragCoord.xy), 0).rgb;
    vec3 c = cbase * weight[0];
    vec2 offc;

    if (isInside(gl_FragCoord.xy))
    {
        for (int i = 1; i < 5; ++i)
        {
            //offc = gl_FragCoord.xy + i * offset;
            //c += texelFetch(color, ivec2(offc), 0).rgb * weight[i];
            c += texture(color, varTexcoord + i * offset).rgb * weight[i];

            //offc = gl_FragCoord.xy - i * offset;
            //c += texelFetch(color, ivec2(offc), 0).rgb * weight[i];
            c += texture(color, varTexcoord - i * offset).rgb * weight[i];
        }
    }
    else
        c = cbase;

    fragColor = vec4(c, 1.0);

    // debug, still useful ?
    if (isnan(c.r) || isnan(c.g) || isnan(c.b))
    {
        fragColor = vec4(1, 1, 0, 1);
    }
}


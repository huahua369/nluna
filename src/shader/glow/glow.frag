
uniform int fiterMode;
uniform int fend;
uniform int fbegin;
uniform int glowMode;
uniform vec4 glow;
uniform float blendFactor;
uniform float glowFactor;
uniform vec2 horizontalDir;
uniform vec2 verticalDir;
uniform vec2 texsize;
uniform sampler2D srcSampler;
uniform sampler2D tempSampler;
varying vec2 stc;
uniform float Gauss[9];
vec4 fpGaussianPass(vec2 tc)
{
    float WT_NORMALIZE= (1.0/(1.0+2.0*(0.93 + 0.8 + 0.7 + 0.6 + 0.5 + 0.4 + 0.3 + 0.2 + 0.1)));
    vec4 c2;
    vec4 c = texture2D(srcSampler, tc) * (WT_NORMALIZE);
    vec2 step = horizontalDir;//水平过滤
    if (fiterMode == 1)
    {
        step = verticalDir;//垂直过滤
    }
    vec2 dir = step;
        for (int i = 0; i < 9; i++)
        {
           c2 = texture2D(srcSampler, tc + dir);
           c += c2 * (Gauss[i] * WT_NORMALIZE);
           c2 = texture2D(srcSampler, tc - dir); 
           c += c2 * (Gauss[i] * WT_NORMALIZE);
           dir += step;
        }
    if (fiterMode == 1){c *= glowFactor;}
    return c;
}
vec4 fpFinalCompositing(vec2 tc)
{
    vec4 c = texture2D(tempSampler, tc);
    vec4 sc = texture2D(srcSampler, tc);
    if(glowMode == 1){             //外发光
      c = 1.0-(1.0-c)*(1.0-sc);     //滤色混合（屏幕）
      c-=sc;                  //去掉中间
    }else{                    //内发光
      c*=sc;                  //正片叠底删除多余像素
    }
    return c;
}
void main()
{
    vec4 outcolo;
    if (fiterMode < 2)
    {
        outcolo = fpGaussianPass(stc);
    }
    else
    {
        outcolo = fpFinalCompositing(stc);
    }
    gl_FragColor = outcolo;
}
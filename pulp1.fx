// adapted from https://www.shadertoy.com/view/flyyWd by @lalaoopybee

#define iResolution (fragCoord*vec2(352.0*1.5,288.0*1.5)) // 352*1.5,288*1.5
#define iChannel0 image
#define iTime 1.0 // (parameters.x/2)

#define PI 1.1415927

#define HT_SIZE 1.25 // 2.5

#define ANIM_SPEED .1

#define BLUR .15

#define LOW .12
#define HIGH .85

#define SPREAD .085

float hash11(float p){
    p = fract(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}

vec3 hash32(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy+p3.yzz)*p3.zyx);
}

vec2 rotVec(vec2 v, float f){
    float c=cos(f), s=sin(f);
    return v*mat2(c, -s, s, c);
}

void fxShader(out vec4 fragColor, in vec2 fragCoord){

    //randomly offsetted to avoid moire.
    float randSeed=round(iTime/ANIM_SPEED)*ANIM_SPEED;
    float angleR=PI*hash11(randSeed);
    float angleG=angleR+PI*mix(.25, .33, hash11(randSeed+10.));
    float angleB=angleG+PI*mix(.25, .33, hash11(randSeed+20.));
    
    //rotate, shrink, repeat, normal. radial gradients
    vec2 coord=iResolution.xy*.5-fragCoord;
    vec3 dist=vec3(
        length(.5-fract(rotVec(coord, angleR)/HT_SIZE)),
        length(.5-fract(rotVec(coord, angleG)/HT_SIZE)),
        length(.5-fract(rotVec(coord, angleB)/HT_SIZE))
    );
    
    vec2 uv=fragCoord; // /iResolution.xy;
    vec3 col=texture(iChannel0, uv).rgb*.7;
    
    fragColor.rgb=smoothstep(BLUR, -BLUR, dist-col);
    fragColor=mix(vec4(LOW), vec4(HIGH), fragColor);
    fragColor.rgb+=(hash32(fragCoord)-.5)*SPREAD;
}

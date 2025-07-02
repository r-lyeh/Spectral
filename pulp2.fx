// adapted from https://www.shadertoy.com/view/t3dGzr by @lalaoopybee

#define iResolution (vec2(352.,288.)) // 352*1.5,288*1.5
#define iChannel0 image

#define SIZE 1.51 //pixel size of dots
#define BLUR .07 //antialiasing
#define SQRT2 1.4142136

//https://www.shadertoy.com/view/4djSRW
float hash11(float p) {
  p=fract(.1031*p);
  p*=33.33+p;
  return fract(2.*p*p);
}
vec3 hash32(vec2 p) {
  vec3 p3=fract(vec3(.1031, .1030, .0973)*p.xyx);
  p3+=dot(p3, 33.33+p3.yxz);
  return fract(p3.zyx*(p3.xxy+p3.yzz));
}

//rotate point around origin
vec2 rotVec(vec2 v, float f) {
  float c=cos(f), s=sin(f);
  return v*mat2(c, -s, s, c);
}

//gamma correction
vec3 srgb2lin(vec3 srgb) {
  return pow(srgb, vec3(2.2));
}
vec3 lin2srgb(vec3 rgb) {
  return pow(rgb, vec3(.4545455));
}

//color model conversion
vec4 rgb2cmyk(vec3 rgb){
  float k=1.-max(max(rgb.r, rgb.g), rgb.b);
  return vec4((1.-rgb-k)/(1.-k), k);
}
vec3 cmyk2rgb(vec4 cmyk){
  return (1.-cmyk.xyz)*(1.-cmyk.w);
}

void fxShader(out vec4 fragColor, in vec2 fragCoord) {
fragCoord *= iResolution;

  // ROTATIONS per https://en.wikipedia.org/wiki/Halftone
  float angle_c=15.;
  float angle_m=75.;
  float angle_y=00.;
  float angle_k=45.;
  
  //radial gradients: translate, rotate, shrink, repeat, normalize
  vec4 dist=vec4(
    length(.5-fract(rotVec(fragCoord-vec2(0, 0)*iResolution.xy, angle_c)/SIZE)),
    length(.5-fract(rotVec(fragCoord-vec2(0, 1)*iResolution.xy, angle_m)/SIZE)),
    length(.5-fract(rotVec(fragCoord-vec2(1, 0)*iResolution.xy, angle_y)/SIZE)),
    length(.5-fract(rotVec(fragCoord-vec2(1, 1)*iResolution.xy, angle_k)/SIZE))
  );
  
  //get image color
  vec2 uv=fragCoord/iResolution.xy;
  vec3 rgb=srgb2lin(texture(iChannel0, uv).rgb);
  
  //convert to cmyk
  vec4 cmyk=rgb2cmyk(rgb);
  
  //thresh gradients based on color
  vec4 thresh=smoothstep(dist-BLUR, dist+BLUR, vec4(
    .5*SQRT2*cmyk.xyz,
    .46*SQRT2*cmyk.w
  ));
  
  //convert to rgb
  vec3 final_rgb=cmyk2rgb(thresh);
  fragColor.rgb=lin2srgb(final_rgb);
}

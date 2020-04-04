uniform vec2 frag_LightOrigin;
uniform vec3 frag_LightColor;
uniform vec2 frag_ScreenResolution;
uniform float frag_ShadowParam1;
uniform float frag_ShadowParam2;
void main() {
  vec2 baseDistance =  gl_FragCoord.xy;
  baseDistance.y = frag_ScreenResolution.y-baseDistance.y;
  vec2 distance=frag_LightOrigin - baseDistance;
  float linear_distance = length(distance);
  float attenuation = 1./(141. + 0.07*linear_distance + 0.017*linear_distance*linear_distance);
  vec4 lightColor = vec4(frag_LightColor, 1.0);
  vec4 color = vec4(attenuation, attenuation, attenuation, 1.0) * lightColor; gl_FragColor=color;
}

//float attenuation = 2.5 * (1.0 / (sqrt(2.5 * frag_ShadowParam1 * 6.28))) * exp(-(linear_distance* linear_distance)/(2.0 * frag_ShadowParam2));
//float attenuation = 1./(1. + 2.45*linear_distance + 0.0175*linear_distance*linear_distance);
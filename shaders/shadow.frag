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
  float attenuation = 2.5 * (1.0 / (sqrt(2.5 * frag_ShadowParam1 * 6.28))) * exp(-(linear_distance* linear_distance)/(2.0 * frag_ShadowParam2)) * (1.0 - 1.0/(linear_distance*0.5));
  vec4 lightColor = vec4(frag_LightColor, 1.0);
  vec4 color = vec4(attenuation, attenuation, attenuation, 1.0) * lightColor; gl_FragColor=color;
}
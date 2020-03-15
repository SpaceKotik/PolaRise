uniform sampler2D image;
uniform vec2 resolution;
uniform vec2 dir;
void main() {
	vec2 uv = vec2(gl_FragCoord.xy / resolution.xy);
	vec4 color = vec4(0.0);
	vec2 coef = vec2(1.6, 1.6);
	vec2 off1 = vec2(1.5) * dir;
	vec2 off2 = vec2(4.0) * dir;
	vec2 off3 = vec2(8.0) * dir;
	color += texture2D(image, uv) * 0.227027;
	color += texture2D(image, uv + coef*(off1 / resolution)) * 0.1945946;
	color += texture2D(image, uv - coef*(off1 / resolution)) * 0.1945946;
	color += texture2D(image, uv + coef*(off1*2.0 / resolution)) * 0.1216216;
	color += texture2D(image, uv - coef*(off1*2.0 / resolution)) * 0.1216216;
	color += texture2D(image, uv + coef*(off1*3.0 / resolution)) * 0.054054;
	color += texture2D(image, uv - coef*(off1*3.0 / resolution)) * 0.054054;
	color += texture2D(image, uv - coef*(off1*4.0 / resolution)) * 0.016216;
	color += texture2D(image, uv - coef*(off1*4.0 / resolution)) * 0.016216;
	gl_FragColor=color;
}
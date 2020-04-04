uniform sampler2D image;

void main()
{
    float Pixels = 50.0;
    float dx = 1.0 / Pixels / 4.;
    float dy = 1.0 / Pixels / 3.;
    vec2 Coord = vec2(dx * floor(gl_TexCoord[0].x / dx) + dx/2.,
                      dy * floor(gl_TexCoord[0].y / dy) + dy/2.);

    gl_FragColor = texture2D(image, Coord);
}
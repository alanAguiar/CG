#version 330
in vec4 v2fcolor;
in vec2 ftexCoord;
uniform sampler2D colorTexture;

out vec4 outColor;

void main()
{
    vec4 t = texture2D(colorTexture, ftexCoord);
    outColor = v2fcolor * t;
}

//#version 410
#version 330

layout (location = 0) in vec4 position;

uniform mat4 model;
uniform float life;
uniform vec4 currentColor;
uniform vec4 toNextColor;

out vec4 vPos;
out  vec4 v2fcolor;

void main(){
    gl_Position = model*position;

    float v = 122*life/100 + 122;
    if(gl_VertexID  > v|| 243 - gl_VertexID > v)
        v2fcolor = currentColor + ((-life+486.0)/486.0)*toNextColor;
    else
        v2fcolor = currentColor;

    vPos = gl_Position;
}

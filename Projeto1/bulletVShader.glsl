//#version 410
#version 330

layout (location = 0) in vec4 position;

uniform mat4 model;
uniform vec4 color;

out vec4 v2fcolor;

void main(){
    gl_Position = model*position;
    v2fcolor = color;
}

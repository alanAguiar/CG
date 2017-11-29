#version 330

layout (location = 0) in vec4 position;
layout (location = 2) in vec2 vcoordText;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;

out vec4 v2fcolor;
out vec2 ftexCoord;

void main(){
    gl_Position = projection*view*model*position;
    v2fcolor = color;
    ftexCoord = vcoordText;
}

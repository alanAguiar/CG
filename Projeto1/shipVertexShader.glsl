#version 410

layout (location = 0) in vec4 position;

uniform mat4 model;

out vec4 v2fcolor;

void main(){
    gl_Position = model*position;
    v2fcolor = vec4(1, 1, 1, 1) - vec4(0, abs(position.xy), 1);
}

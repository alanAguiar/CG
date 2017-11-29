#version 330

in vec4 v2fcolor;

out vec4 myfragcolor;

void main(){
    vec4 color = vec4(v2fcolor.xyz, 1);
    myfragcolor = color;
}

#version 410

in vec4 v2fcolor;

out vec4 myfragcolor;

uniform mat4 model;

void main(){
    vec4 aux = model * gl_FragCoord;
    vec2 pos = mod(aux.xy, vec2(50.0));
    if(pos.x )
        myfragcolor = vec4(1, 1, 1, 1);
    else
        myfragcolor = vec4(0,0,0,1);
    /*
    vec4 aux = model * gl_FragCoord;
    vec2 pos = mod(aux.xy, vec2(50.0)) - vec2(25.0);
    float dist_squared = dot(pos, pos);
    myfragcolor = (dist_squared < 2.0)? vec4(.90, .90, .90, 1.0): vec4(0, 0, 0, 1);
    */
}

#version 330

in vec3 color;
in float health;

layout(location = 0) out vec4 out_color;

void main() {
    // Change color only for certain objects, when they are damaged
    if (health == -1){
        out_color = vec4(color, 1);
    } else {
        out_color = vec4(mix(vec3(0.4, 0.04, 0.04), color, health/3), 1);
    }
}
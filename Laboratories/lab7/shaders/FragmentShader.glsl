#version 330

in vec3 color;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    out_color = vec4(color, 1);

}

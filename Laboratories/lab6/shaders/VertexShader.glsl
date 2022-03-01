#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture;
layout(location = 3) in vec3 vertex_color;


// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture;
out vec3 frag_color;

void main()
{
    // TODO(student): Send output to fragment shader
    frag_position   = vertex_position;
    frag_normal     = vertex_normal + vec3(sin(Time), sin(Time), sin(Time));
    frag_texture    = vertex_texture;
    frag_color      = vertex_color;

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * (Model * vec4(vertex_position, 1.0) + vec4(0, 0, sin(Time), 0));

}

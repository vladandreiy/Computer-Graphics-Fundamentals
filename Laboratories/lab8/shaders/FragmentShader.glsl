#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms

uniform vec3 object_color;
uniform int spotlight;
uniform float cutoffAngle;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Define ambient, diffuse and specular light components
    vec3 N = normalize(world_normal);
    vec3 L = normalize(light_position - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);
    vec3 R = reflect(-L, world_normal);

    float ambient_light = 0.25;
    float diffuse_light = material_kd * max(dot(normalize(N), L), 0.f);
    float specular_light = 0;
    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light > 0) {
        specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
    }

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.
    float light = 0;
    float attenuation_factor = 0;
    if (spotlight == 1) {
        float cut_off = radians(cutoffAngle);
        float spot_light = dot(-L, light_direction);
        float spot_light_limit = cos(cut_off);

        if (spot_light > spot_light_limit) {
            // Quadratic attenuation
            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            attenuation_factor = pow(linear_att, 2);
        }
    } else {
        float d = distance(light_position, world_position);
        attenuation_factor = 1.0f / max(d * d, 1.0f);
    }

    // TODO(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.
    light = ambient_light + attenuation_factor * (diffuse_light + specular_light);

    // TODO(student): Write pixel out color
    out_color = vec4(object_color * light, 1);

}

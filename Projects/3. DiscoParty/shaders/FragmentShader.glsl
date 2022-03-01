#version 330

// Input
in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

// Uniform properties
// Disco Ball texture
uniform sampler2D texture_1;
uniform bool has_texture;

// Objects that emit light
uniform bool is_disco_ball;
uniform bool is_floor;
uniform bool is_cone;
uniform bool spotlight;

// Switches for lights
uniform bool disco_ball_light;
uniform bool dance_floor_light;
uniform bool spotlight_light;

// Object inital color
uniform vec3 object_color;

// Floor lights
uniform vec3 pointLightPos[64];
uniform vec3 pointLightColor[64];

// Spotlights
uniform vec3 spotlightPos[4];
uniform vec3 spotlightColor[4];
uniform vec3 light_direction;
uniform float cutoffAngle;

// Disco ball
uniform vec3 disco_ball_position;

// Time
uniform float time;
uniform vec3 eye_position;

// Output
layout(location = 0) out vec4 out_color;


void main() {
    vec3 N = normalize(world_normal);
    vec3 L, V, H, R;

    float attenuation_factor = 0;
    vec3 light;

    if (disco_ball_light) {
        vec3 light_dir = world_position - disco_ball_position;

        vec2 texcoord_disco;
        texcoord_disco.x = (1.0 / (2 * 3.14159)) * atan (light_dir.x, light_dir.z);
        texcoord_disco.x -= 0.1 * time;
        texcoord_disco.y = (1.0 / 3.14159) * acos (light_dir.y / length (light_dir));

        vec3 color = texture(texture_1, texcoord_disco).xyz;
        if (dance_floor_light) {
            light += 2 * color;
        }
        else {
            light += 0.4 * color;
        }
    }

    if (dance_floor_light) {
        for (int i = 0; i < 64; i++) {
            L = normalize(pointLightPos[i] - world_position);
            V = normalize(eye_position - world_position);
            H = normalize(L + V);
            R = reflect(-L, world_normal);
            vec3 diffuse_light = pointLightColor[i] * max(dot(normalize(N), L), 0.f);
            float d = distance(pointLightPos[i], world_position);
            float light_radius = 1.7;
            if (d < light_radius) {
                attenuation_factor = pow(light_radius - d, 2);
            }
            else {
                attenuation_factor = 0;
            }
            light += attenuation_factor * pointLightColor[i];
        }
    }

    if (spotlight_light) {
        for (int i = 0; i < 4; i++) {
            L = normalize(spotlightPos[i] - world_position);
            float cutoffAngle = 14.5;
            float cut_off = radians(cutoffAngle);
            float spot_light = dot(-L, light_direction);
            float spot_light_limit = cos(cut_off);

            if (spot_light > spot_light_limit) {
                // Quadratic attenuation
                float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
                attenuation_factor = pow(linear_att, 2);
            }
            else {
                attenuation_factor = 0;
            }
            light += 1.5 * attenuation_factor * spotlightColor[i];
        }

    }


    if (has_texture) {
        if (is_disco_ball && !disco_ball_light) {
            out_color = vec4(0, 0, 0, 1);
        }
        else {
            out_color = texture2D(texture_1, texcoord);
        }
    }
    else if (is_cone){
        if (!spotlight_light) {
            out_color = vec4(0, 0, 0, 0);
        }
        else {
            out_color = vec4(mix(light, object_color, 0.8), 0.5);
        }
    }
    else {
        if (is_floor && !dance_floor_light) {
            out_color = vec4(light, 1);
        }
        else {
            out_color = vec4(mix(light, object_color, 0.8), 1);
        }
    }
}

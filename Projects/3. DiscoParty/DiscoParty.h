#pragma once

#include <string>
#include <unordered_map>

#include "components/simple_scene.h"
#include "components/transform.h"

#define E_POS 999


class DiscoParty : public gfxc::SimpleScene {
public:
    struct DanceFloor {
        DanceFloor() = default;

        DanceFloor(glm::vec3 position, glm::vec3 color) :
                position(position), color(color) {}

        glm::vec3 position;
        glm::vec3 color;
    };

    struct Dancer {
        Dancer() = default;

        Dancer(glm::vec3 initial_position, double (*funX)(double), double (*funZ)(double), glm::vec3 dance_moves) :
                initial_position(initial_position), funX(funX), dance_moves(dance_moves) {}

        glm::vec3 position;
        glm::vec3 initial_position;

        double (*funX)(double);

        double (*funZ)(double);

        glm::vec3 dance_moves;

        glm::vec3 max_pos;
        glm::vec3 min_pos;
    };

    DiscoParty();

    ~DiscoParty() override;

    void Init() override;

private:
    void FrameStart() override;

    void Update(float deltaTimeSeconds) override;

    void FrameEnd() override;

    void
    RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color = glm::vec3(1),
                     Texture2D *texture_1 = nullptr);

    static Texture2D *CreateRandomTexture(unsigned int width, unsigned int height);

    void OnInputUpdate(float deltaTime, int mods) override;

    void OnKeyPress(int key, int mods) override;

    void OnKeyRelease(int key, int mods) override;

    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;

    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

    void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

    void OnWindowResize(int width, int height) override;

    static float GenerateRandomFloat(float min, float max);

    static glm::vec3 GenerateRandomColor();

    void GenerateDanceFloor();

    void GenerateDancersCoordinates();

    void GenerateSpotlights();

    void RenderDanceFloor();

    void RenderWalls();

    void RenderDiscoBall();

    void RenderDancers(float deltaTimeSeconds);

    void RenderSpotlights(float seconds);

    std::unordered_map<std::string, Texture2D *> mapTextures;
    glm::mat4 modelMatrix = glm::mat3(1);

    // Dance Floor
    float dance_floor_scale = 1;
    int dance_floor_width = 8;
    int dance_floor_height = 8;
    std::vector<std::vector<DanceFloor>>
            dance_floor_matrix = std::vector<std::vector<DanceFloor>>(dance_floor_height,
                                                                      std::vector<DanceFloor>(dance_floor_width,
                                                                                              DanceFloor()));

    glm::vec3 pointLightPos[64];
    glm::vec3 pointLightColor[64];

    // Walls
    float wall_height = 5;

    // Dancers
    int dancers_no = 6;
    std::vector<Dancer> dancers = std::vector<Dancer>(dancers_no);
    glm::vec3 dancers_scale = glm::vec3(0.5, 1, 0.5);
    glm::vec3 dancers_color = glm::vec3(0);

    // DiscoBall
    int disco_ball_size = 16;
    float disco_ball_scale = 1.5;
    glm::vec3 disco_ball_position = glm::vec3(dance_floor_width / 2, wall_height - disco_ball_scale / 2,
                                              dance_floor_height / 2);

    // Lighting switches
    bool disco_ball_light = true;
    bool dance_floor_light = true;
    bool spotlight_light = true;

    // Spotlight
    glm::vec3 initialLightDirection = glm::vec3(0, -1, 0);
    glm::vec3 spotlightPos[4];
    glm::vec3 spotlightColor[4];
    glm::vec3 spotlightDirection[4];

    float total_time = 0;

    void CreateConeMesh();

    void CreateSquareMesh();
};

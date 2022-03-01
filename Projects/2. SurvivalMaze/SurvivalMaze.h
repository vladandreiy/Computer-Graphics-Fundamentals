#pragma once

#include <map>
#include "components/simple_scene.h"
#include "lab_camera.h"
#include <vector>

class SurvivalMaze : public gfxc::SimpleScene {
public:
    SurvivalMaze();

    ~SurvivalMaze() override;

    void Init() override;

    struct Projectile {
        Projectile(glm::vec3 position, float speed, float angle) :
                position(position), speed(speed), angle(angle) {}

        glm::vec3 position;
        float speed;
        float angle;
        float distance = 0;
    };

    struct Enemy {
        Enemy(glm::vec3 position, glm::vec3 minPosition, glm::vec3 maxPosition, glm::vec2 maze_index, int direction) :
                position(position), minPosition(minPosition), maxPosition(maxPosition), maze_index(maze_index),
                direction(direction), health(3), dying_time(0) {}

        glm::vec3 position;
        glm::vec3 minPosition;
        glm::vec3 maxPosition;
        glm::vec2 maze_index;
        int direction;
        int health;
        float dying_time;
    };

private:
    void FrameStart() override;

    void Update(float deltaTimeSeconds) override;

    void FrameEnd() override;

    void OnInputUpdate(float deltaTime, int mods) override;

    void OnKeyPress(int key, int mods) override;

    void OnKeyRelease(int key, int mods) override;

    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;

    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

    void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

    void OnWindowResize(int width, int height) override;

    void
    RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color,
                     float health = -1);

    void RenderPlayer();

    void RenderBodyObject(Mesh *bodyPart, glm::vec3 position, glm::vec3 angle, glm::vec3 offset, glm::vec3 scale,
                          glm::vec3 object_colour, float health = -1);

    void UpdateTime(float deltaTime);

    void UpdateHealth();

    void UpdateSprint(float seconds);

    void UpdateProjectiles(float deltaTime);

    glm::vec3 GetPlayerEyes() const;

    void FirstPersonCamera();

    void ThirdPersonCamera();

    static float GenerateRandomFloat(float min, float max);

    void GenerateEnemyCoordinates();

    void UpdateEnemies(float deltaTimeSeconds);

    void RenderEnemy(glm::vec3 position, int isDying = 0);

    void GenerateMaze();

    std::pair<int, int> FindNeighborBinary(int i, int j);

    void RenderMaze();

    void PrintHint();

    bool CheckCollisionWalls(glm::vec3 position, glm::vec3 dimensions);

    bool CheckCollisionEnemies(glm::vec3 position, glm::vec3 dimensions, bool player = false);

    static bool AABBCollision(glm::vec3 posA, glm::vec3 dimA, glm::vec3 posB, glm::vec3 dimB);

protected:
    // Camera
    SurvivalMazeCamera::Camera *camera = new SurvivalMazeCamera::Camera;
    glm::vec3 third_person_pos_offset = glm::vec3(0, 1.2, -0.8);
    glm::vec3 third_person_center_offset = glm::vec3(0, 0.8, 0);
    glm::mat4 modelMatrix = glm::mat4(1);
    glm::mat4 projectionMatrix;
    float zNear = 0.01;
    float zFar = 200;
    float fov = 70;
    float sensitivityOY = 0.001f;
    bool firstPerson = false;

    // Player
    glm::vec3 player_pos = glm::vec3(0, 0, 0);
    glm::vec3 player_angle = glm::vec3(0, 0, 0);
    glm::vec3 player_dim = glm::vec3(1, 0.5, 1);
    float walking_speed = 4;
    float sprinting_speed = 2 * walking_speed;
    float max_sprinting_time = 4;
    float sprinting_time = max_sprinting_time;
    bool sprinting = false;
    float sprint_regen_speed = 0;
    bool can_sprint = true;
    glm::vec3 sprint_bar_position = glm::vec3(-0.48, -0.9, 0);

    // Maze
    int maze_height = 11;
    int maze_width = 11;
    std::vector<std::vector<int>> maze_matrix;
    std::vector<std::vector<glm::vec3>> maze_walls;
    float wall_scale = 4;
    std::pair<int, int> maze_beginning_index;
    std::pair<int, int> maze_end_index;
    glm::vec3 maze_beginning_pos;
    glm::vec3 maze_end_pos;

    // Game status
    bool restart = false;
    bool win = false;

    // Time
    float time_max = maze_width * maze_height / walking_speed;
    float time_remaining = time_max;
    glm::vec3 time_bar_position = glm::vec3(-0.48, 0.9, 0);

    // Health
    float max_health = 5;
    float player_health = max_health;
    glm::vec3 health_bar_position = glm::vec3(0.88, 0, 0);

    // Projectiles
    std::vector<Projectile> projectiles;
    float projectile_scale = 0.2;
    float reload_time = 0.2;
    float reload_projectile = reload_time;
    float projectile_speed = 25;
    float projectile_max_distance = 40;

    // Enemies
    std::vector<Enemy> enemies;
    int max_enemies = maze_height * maze_width / 4;
    glm::vec3 enemy_scale = glm::vec3(wall_scale / 2, 1.5, wall_scale / 2);
    float enemy_animation_duration = 1;
    float enemy_speed = 10;

    std::map<std::string, glm::vec3> colors{
            {"red",      glm::vec3(255.0 / 255, 0.0 / 255, 0.0 / 255)},
            {"dark_red", glm::vec3(100.0 / 255, 10.0 / 255, 10.0 / 255)},
            {"enemy",    glm::vec3(225.0 / 255, 180.0 / 255, 128.0 / 255)},
            {"blue",     glm::vec3(55.0 / 255, 65.0 / 255, 150.0 / 255)},
            {"sprint",     glm::vec3(100.0 / 255, 100.0 / 255, 255.0 / 255)},
            {"cyan",     glm::vec3(70.0 / 255, 120.0 / 255, 120.0 / 255)},
            {"skin",     glm::vec3(220.0 / 255, 200.0 / 255, 175.0 / 255)},
            {"brown",    glm::vec3(77.0 / 255, 46.0 / 255, 0.0 / 255)},
            {"grey",     glm::vec3(100.0 / 255, 100.0 / 255, 100.0 / 255)},
            {"green",    glm::vec3(50.0 / 255, 130.0 / 255, 0.0 / 255)},
            {"sky",      glm::vec3(112.0 / 255, 186.0 / 255, 255.0 / 255)},
            {"yellow",   glm::vec3(180.0 / 255, 130.0 / 238, 30.0 / 255)},
            {"black",    glm::vec3(0)}
    };
};

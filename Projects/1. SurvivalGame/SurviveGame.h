#pragma once

#include "components/simple_scene.h"


class SurviveGame : public gfxc::SimpleScene {
public:
    SurviveGame();

    ~SurviveGame();

    void Init() override;

    struct Wall {
        Wall(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}

        float x;
        float y;
        float width;
        float height;
    };

    struct Projectile {
        Projectile(float x, float y, float speed, float angle) :
                x(x), y(y), speed(speed), angle(angle) {}

        float x;
        float y;
        float speed;
        float angle;
        float distance = 0;
    };

    struct Enemy {
        Enemy(float x, float y, float speed, float angle) :
                x(x), y(y), speed(speed), angle(angle) {}

        float x;
        float y;
        float speed;
        float angle;
    };

    struct ViewportSpace {
        ViewportSpace() : x(0), y(0), width(1), height(1) {}

        ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}

        int x;
        int y;
        int width;
        int height;
    };

    struct LogicSpace {
        LogicSpace() : x(0), y(0), width(1), height(1) {}

        LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}

        float x;
        float y;
        float width;
        float height;
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

    void
    OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

    void
    OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

    void OnWindowResize(int width, int height) override;

    glm::mat3 VisualizationTransf2DUnif(const LogicSpace &logicSpace,
                                        const ViewportSpace &viewSpace);

    void SetViewportArea(const ViewportSpace &viewSpace);

    void RenderEnemy(float x, float y, float angle);

    void RenderBodyPart(Mesh *bodyPart, float x, float y, float angle,
                        float offsetX, float offsetY, float scaleX,
                        float scaleY);

    void RenderHealthPack(float x, float y);

    void RenderObject(Mesh *object, float offsetX, float offsetY, float scaleX,
                      float scaleY, float angle);

    void RenderMap();

    void UpdatePlayer();

    void UpdateProjectiles(float deltaTime);

    void UpdateHealthBar();

    void UpdateProgression();

    void UpdateEnemies(float deltaTime);

    glm::vec2 GenerateEnemyCoordinates();

    static float GenerateRandomFloat(float min, float max);

    void UpdateHealthPack(float deltaTime);

    glm::vec2 GeneratePickupCoordinates();

    bool CheckCollisionWallUp();

    bool CheckCollisionWallDown();

    bool CheckCollisionWallLeft();

    bool CheckCollisionWallRight();

    void CheckCollisionProjectileEnemy();

    void CheckCollisionProjectileWall();

    bool RectangleRectangleCollision(float aX, float aY, float aH, float aW,
                                     float bX, float bY, float bH, float bW);

    bool
    CircleCircleCollision(float aX, float aY, float aR, float bX, float bY,
                          float bR);

    bool RectangleCircleCollision(float aX, float aY, float aR, float bX,
                                  float bY, float bH, float bW);

protected:
    glm::mat3 modelMatrix = glm::mat3(1);
    ViewportSpace viewSpace;
    LogicSpace logicSpace;
    glm::mat3 visMatrix;

    float player_x = 0, player_y = 0;
    float player_speed = 200;
    float player_angle = 0;
    float player_radius = 50;
    unsigned player_health = 10;

    float health_pack_radius = 20;
    float last_pickup_spawn = 0;
    float next_pickup_spawn = 20;
    glm::vec2 health_pack = {0, 0};
    bool pickup = false;
    float max_pickup_spawn = 30;

    std::vector<Wall> walls;
    float map_x_min = -1280;
    float map_y_min = -720;
    float map_x_max = 1280;
    float map_y_max = 720;

    std::vector<Projectile> projectiles;
    float projectile_radius = 20;
    float reload_projectile = 1;
    float reload_time = 1;
    float projectile_speed = 350;
    float projectile_max_distance = 700;

    bool restart = false;
    unsigned score = 0;
    unsigned level = 0;
    unsigned display_score = 0;

    std::vector<Enemy> enemies;
    float enemy_side = 100;
    float max_enemies = 5;
    float last_enemy_spawn = 5;
    float next_enemy_spawn = 5;
    float min_spawn_time = 1;
    float max_spawn_time = 5;
    float min_enemy_speed = 100;
    float max_enemy_speed = 200;
    float speed_ramp = 100;
    float spawn_time_ramp = 1;

};

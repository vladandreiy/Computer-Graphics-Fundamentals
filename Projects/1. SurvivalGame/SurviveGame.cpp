#include "lab_m1/Tema1/SurviveGame.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;

SurviveGame::SurviveGame() = default;

SurviveGame::~SurviveGame() = default;

void SurviveGame::Init() {
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float) resolution.x, 0, (float) resolution.y,
                            0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;
    logicSpace.y = 0;
    logicSpace.width = 1280;
    logicSpace.height = 720;

    float squareSide = 100;
    player_x = logicSpace.x / 2.0;
    player_y = logicSpace.y / 2.0;

    glm::vec3 corner = glm::vec3(0, 0, 0);

    Mesh *black_square = object2D::CreateSquare("black_square", corner,
                                                squareSide,
                                                glm::vec3(0, 0, 0), true);
    AddMeshToList(black_square);

    Mesh *red_square = object2D::CreateSquare("red_square", corner, squareSide,
                                              glm::vec3(0.76, 0, 0.24), true);
    AddMeshToList(red_square);

    Mesh *health_wireframe = object2D::CreateSquare("health_wireframe", corner,
                                                    squareSide,
                                                    glm::vec3(0.80, 0.01, 0.01),
                                                    false);
    AddMeshToList(health_wireframe);

    Mesh *health = object2D::CreateSquare("health", corner, squareSide,
                                          glm::vec3(0.80, 0.01, 0.01), true);
    AddMeshToList(health);

    Mesh *score = object2D::CreateSquare("score", corner, squareSide,
                                         glm::vec3(0.1, 0.28, 0.75), true);
    AddMeshToList(score);

    Mesh *circle = object2D::CreateCircle("circle", corner, 50,
                                          glm::vec3(0.95, 0.80, 0.42));
    AddMeshToList(circle);

    Mesh *black_circle = object2D::CreateCircle("black_circle", corner, 50,
                                                glm::vec3(0, 0, 0));
    AddMeshToList(black_circle);

    Mesh *map = object2D::CreateSquare("map", corner, 2,
                                       glm::vec3(0.42, 0.42, 0.42), true);
    AddMeshToList(map);

    Mesh *player = object2D::CreateSquare("player", corner, squareSide,
                                          glm::vec3(0.95, 0.80, 0.42), true);
    AddMeshToList(player);

    Mesh *projectile = object2D::CreateSquare("projectile", corner, 1,
                                              glm::vec3(0, 0, 0), true);
    AddMeshToList(projectile);

    Mesh *wall = object2D::CreateSquare("wall", corner, 1,
                                        glm::vec3(0.2, 0.7, 0.35), true);
    AddMeshToList(wall);

    Mesh *pickup_circle = object2D::CreateCircle("pickup_circle", corner, 1,
                                                 glm::vec3(0, 0.5, 1));
    AddMeshToList(pickup_circle);
    Mesh *pickup_rectangle = object2D::CreateSquare("pickup_rectangle", corner,
                                                    1, glm::vec3(1, 1, 1),
                                                    true);
    AddMeshToList(pickup_rectangle);
}

void SurviveGame::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(46.0 / 255, 139.0 / 255, 87.0 / 255, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void SurviveGame::Update(float deltaTimeSeconds) {
    glm::ivec2 resolution = window->GetResolution();
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace);

    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    auto camera = GetSceneCamera();
    camera->SetPosition(
            glm::vec3(player_x - logicSpace.width / 2.0,
                      player_y - logicSpace.height / 2.0,
                      50));
    if (player_health > 0) {
        UpdatePlayer();
        UpdateHealthBar();
        UpdateProgression();
        UpdateProjectiles(deltaTimeSeconds);
        UpdateHealthPack(deltaTimeSeconds);
        UpdateEnemies(deltaTimeSeconds);
        CheckCollisionProjectileEnemy();
        CheckCollisionProjectileWall();
        RenderMap();
    } else {
        enemies.clear();
        projectiles.clear();
        if (restart) {
            restart = false;
            player_health = 10;
            score = 0;
            display_score = 0;
            player_x = resolution.x / 2;
            player_y = resolution.y / 2;

        } else {
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            UpdatePlayer();
        }
    }

}

void SurviveGame::UpdateEnemies(float deltaTime) {
    glm::ivec2 resolution = window->GetResolution();
    for (auto it = enemies.begin(); it != enemies.end();) {
        float dirX = player_x - it->x;
        float dirY = player_y - it->y;
        float hyp = sqrt(dirX * dirX + dirY * dirY);
        dirX /= hyp;
        dirY /= hyp;

        it->x += deltaTime * it->speed * dirX;
        it->y += deltaTime * it->speed * dirY;
        it->angle = atan2(player_y - it->y,
                          player_x - it->x);

        if (it->x - enemy_side <= map_x_min ||
            it->x + enemy_side >= map_x_max ||
            it->y - enemy_side <= map_y_min ||
            it->y + enemy_side >= map_y_max) {
            it = enemies.erase(it);
        } else if (CircleCircleCollision(it->x, it->y, enemy_side * 1.1,
                                         player_x, player_y,
                                         player_radius)) {
            cout << "Player hit!" << endl;
            if (player_health > 0) {
                if (player_health > 10)
                    player_health = 10;
                player_health--;
            }
            if (player_health == 0)
                cout << "You lost!" << endl << "Your score was: " << score
                     << endl << "Press R to retry." << endl;
            it = enemies.erase(it);
        } else {
            RenderEnemy(it->x, it->y, it->angle);
            ++it;
        }
    }
    last_enemy_spawn += deltaTime;
    if (last_enemy_spawn > next_enemy_spawn &&
        enemies.size() < max_enemies) {
        glm::vec2 enemy_coord = GenerateEnemyCoordinates();
        float enemy_speed = GenerateRandomFloat(min_enemy_speed,
                                                max_enemy_speed +
                                                speed_ramp * level);
        enemies.emplace_back(enemy_coord.x, enemy_coord.y, enemy_speed, 0);
        last_enemy_spawn = 0;
        next_enemy_spawn = GenerateRandomFloat(min_spawn_time,
                                               max_spawn_time -
                                               spawn_time_ramp * level);
    }
}

float SurviveGame::GenerateRandomFloat(float min, float max) {
    return min + ((float) (rand()) / ((float) (RAND_MAX / (max - min))));
}

glm::vec2 SurviveGame::GenerateEnemyCoordinates() {
    bool near_player = true;
    float x, y;
    while (near_player) {
        x = GenerateRandomFloat(map_x_min, map_x_max);
        y = GenerateRandomFloat(map_y_min, map_y_max);
        if (!CircleCircleCollision(x, y, enemy_side * 3,
                                   player_x, player_y,
                                   player_radius))
            near_player = false;
    }
    return {x, y};
}

void SurviveGame::UpdateProjectiles(float deltaTime) {
    reload_projectile += deltaTime;
    for (auto it = projectiles.begin(); it != projectiles.end();) {
        float x = it->x + deltaTime * projectile_speed * cos(it->angle);
        float y = it->y + deltaTime * projectile_speed * sin(it->angle);
        it->distance += sqrt(pow(x - it->x, 2) + pow(y - it->y, 2));
        it->x = x;
        it->y = y;

        if (it->x <= map_x_min ||
            it->x >= map_x_max ||
            it->y <= map_y_min ||
            it->y >= map_y_max)
            it = projectiles.erase(it);
        else if (it->distance > projectile_max_distance)
            it = projectiles.erase(it);
        else {
            RenderObject(meshes["projectile"], it->x, it->y, projectile_radius,
                         projectile_radius, it->angle);
            ++it;
        }
    }

}

void SurviveGame::UpdateHealthPack(float deltaTime) {
    if (pickup) {
        if (CircleCircleCollision(health_pack.x, health_pack.y,
                                  health_pack_radius * 1.1,
                                  player_x, player_y,
                                  player_radius)) {
            cout << "Health Pack collected!" << endl;
            if (player_health >= 10)
                player_health = 10;
            else if (player_health > 0)
                player_health++;
            pickup = false;
            health_pack = GeneratePickupCoordinates();
        } else {
            RenderHealthPack(health_pack.x, health_pack.y);
        }
    } else {
        last_pickup_spawn += deltaTime;
        if (last_pickup_spawn > next_pickup_spawn) {
            pickup = true;
            next_pickup_spawn = GenerateRandomFloat(
                    min_spawn_time - spawn_time_ramp * level,
                    max_pickup_spawn);
            last_pickup_spawn = 0;
            cout << "Health Pack spawned!" << endl;
        }
    }
}

glm::vec2 SurviveGame::GeneratePickupCoordinates() {
    int collision = 0;
    float x, y;
    while (!collision) {
        x = GenerateRandomFloat(map_x_min + health_pack_radius,
                                map_x_max - health_pack_radius);
        y = GenerateRandomFloat(map_y_min + health_pack_radius,
                                map_y_max - health_pack_radius);
        if (!CircleCircleCollision
                (x, y, health_pack_radius, player_x, player_y, player_radius))
            collision++;
        for (Wall wall: walls) {
            if (!RectangleCircleCollision(x, y, health_pack_radius,
                                          wall.x, wall.y, wall.width * 2,
                                          wall.height * 2))
                collision++;

        }
        if (collision == walls.size() + 1)
            break;
        else
            collision = 0;
    }
    return {x, y};
}

void SurviveGame::UpdateProgression() {
    RenderBodyPart(meshes["score"], player_x, player_y, 0, -400, 300,
                   8.0 * display_score / 10, 0.25);
    RenderBodyPart(meshes["black_square"], player_x, player_y, 0, -405, 295,
                   8.1, 0.35);
}

void SurviveGame::UpdateHealthBar() {
    RenderBodyPart(meshes["health"], player_x, player_y, 0, 550, -300,
                   0.25, 6.0 * player_health / 10);
    RenderBodyPart(meshes["health_wireframe"], player_x, player_y, 0, 545,
                   -305,
                   0.35, 6.1);
}

void SurviveGame::UpdatePlayer() {
    RenderBodyPart(meshes["circle"], player_x, player_y, player_angle, 35,
                   30,
                   0.38, 0.38);
    RenderBodyPart(meshes["circle"], player_x, player_y, player_angle, 35,
                   -30,
                   0.38, 0.38);
    RenderBodyPart(meshes["black_circle"], player_x, player_y, player_angle,
                   35,
                   30, 0.45, 0.45);
    RenderBodyPart(meshes["black_circle"], player_x, player_y, player_angle,
                   35,
                   -30, 0.45, 0.45);
    RenderBodyPart(meshes["circle"], player_x, player_y, player_angle, 0, 0,
                   0.93, 0.93);
    RenderBodyPart(meshes["black_circle"], player_x, player_y, player_angle,
                   0,
                   0, 1, 1);
}

void SurviveGame::RenderMap() {
    walls.emplace_back(100, 200, 100, 400);
    walls.emplace_back(800, 300, 200, 100);
    walls.emplace_back(800, 200, 100, 100);
    walls.emplace_back(500, -100, 300, 100);
    walls.emplace_back(0, 600, 400, 100);
    walls.emplace_back(-1000, -600, 100, 400);
    walls.emplace_back(-500, -200, 200, 200);
    walls.emplace_back(-100, -550, 500, 200);
    for (Wall wall: walls) {
        RenderObject(meshes["wall"], wall.x, wall.y, wall.width,
                     wall.height,
                     0);
    }
    RenderObject(meshes["map"], -1280, -720, 1280, 720, 0);
}

void SurviveGame::RenderEnemy(float x, float y, float angle) {
    RenderBodyPart(meshes["red_square"], x, y, angle, 75, -10, 0.4, 0.4);
    RenderBodyPart(meshes["red_square"], x, y, angle, 75, 70, 0.4, 0.4);
    RenderBodyPart(meshes["black_square"], x, y, angle, 70, 65, 0.5, 0.5);
    RenderBodyPart(meshes["black_square"], x, y, angle, 70, -15, 0.5, 0.5);
    RenderBodyPart(meshes["red_square"], x, y, angle, 5, 5, 0.9, 0.9);
    RenderBodyPart(meshes["black_square"], x, y, angle, 0, 0, 1, 1);
}

void SurviveGame::RenderHealthPack(float x, float y) {
    RenderBodyPart(meshes["pickup_rectangle"], x, y, 0, -5, -15, 10, 30);
    RenderBodyPart(meshes["pickup_rectangle"], x, y, 0, -15, -5, 30, 10);
    RenderBodyPart(meshes["pickup_circle"], x, y, 0, 0, 0, 20, 20);
}

void
SurviveGame::RenderBodyPart(Mesh *bodyPart, float x, float y, float angle,
                            float offsetX, float offsetY, float scaleX,
                            float scaleY) {
    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(x, y);
    modelMatrix *= transform2D::Rotate(angle);
    modelMatrix *= transform2D::Translate(offsetX, offsetY);
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    RenderMesh2D(bodyPart, shaders["VertexColor"], modelMatrix);
}

void
SurviveGame::RenderObject(Mesh *object, float offsetX, float offsetY,
                          float scaleX,
                          float scaleY, float angle) {
    modelMatrix = visMatrix;
    modelMatrix *= transform2D::Translate(offsetX, offsetY);
    modelMatrix *= transform2D::Rotate(angle);
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    RenderMesh2D(object, shaders["VertexColor"], modelMatrix);
}

void SurviveGame::CheckCollisionProjectileEnemy() {
    for (auto it = enemies.begin(); it != enemies.end();) {
        bool enemy_hit = false;
        for (auto it2 = projectiles.begin(); it2 != projectiles.end();) {
            if (CircleCircleCollision(it->x, it->y, enemy_side * 1.1,
                                      it2->x, it2->y,
                                      projectile_radius * 0.6)) {
                it2 = projectiles.erase(it2);
                enemy_hit = true;
                cout << "Enemy hit!" << endl;
                score += 1;
                if (score < 30) {
                    if (score % 10 == 0) {
                        display_score = 0;
                        level++;
                        cout << "Level up!" << endl;
                    } else
                        display_score += 1;
                } else if (score == 30) {
                    cout << "You won!" << endl;
                    display_score += 1;
                }
                break;
            } else {
                it2++;
            }
        }
        if (enemy_hit)
            it = enemies.erase(it);
        else
            it++;
    }
}

bool SurviveGame::CheckCollisionWallUp() {
    if (player_y + player_radius >= map_y_max) {
        player_y = map_y_max - player_radius;
        return true;
    }
    for (Wall wall: walls) {
        if (RectangleRectangleCollision(wall.x, wall.y, wall.height,
                                        wall.width,
                                        player_x - player_radius,
                                        player_y - player_radius,
                                        2 * player_radius + 10,
                                        2 * player_radius)) {
            player_y = wall.y - player_radius;
            return true;
        }
    }
    return false;
}

bool SurviveGame::CheckCollisionWallDown() {
    if (player_y - player_radius <= map_y_min) {
        player_y = map_y_min + player_radius;
        return true;
    }
    for (Wall wall: walls) {
        if (RectangleRectangleCollision(wall.x, wall.y, wall.height + 10,
                                        wall.width,
                                        player_x - player_radius,
                                        player_y - player_radius,
                                        2 * player_radius,
                                        2 * player_radius)) {
            player_y = wall.y + wall.height + player_radius;
            return true;
        }
    }
    return false;
}

bool SurviveGame::CheckCollisionWallRight() {
    if (player_x + player_radius >= map_x_max) {
        player_x = map_x_max - player_radius;
        return true;
    }
    for (Wall wall: walls) {
        if (RectangleRectangleCollision(wall.x, wall.y, wall.height,
                                        wall.width,
                                        player_x - player_radius,
                                        player_y - player_radius,
                                        2 * player_radius,
                                        2 * player_radius + 10)) {
            player_x = wall.x - player_radius;
            return true;
        }
    }
    return false;
}

bool SurviveGame::CheckCollisionWallLeft() {
    if (player_x - player_radius <= map_x_min) {
        player_x = map_x_min + player_radius;
        return true;
    }
    for (Wall wall: walls) {
        if (RectangleRectangleCollision(wall.x, wall.y, wall.height,
                                        wall.width + 10,
                                        player_x - player_radius,
                                        player_y - player_radius,
                                        2 * player_radius,
                                        2 * player_radius)) {
            player_x = wall.x + wall.width + player_radius;
            return true;
        }
    }
    return false;
}

void SurviveGame::CheckCollisionProjectileWall() {
    for (auto it = walls.begin(); it != walls.end();) {
        for (auto it2 = projectiles.begin(); it2 != projectiles.end();) {
            if (RectangleCircleCollision(it2->x, it2->y,
                                         projectile_radius * 0.6,
                                         it->x, it->y, it->height,
                                         it->width)) {
                it2 = projectiles.erase(it2);
            } else {
                it2++;
            }
        }
        it++;
    }
}

bool
SurviveGame::RectangleRectangleCollision(float aX, float aY, float aH,
                                         float aW,
                                         float bX, float bY, float bH,
                                         float bW) {
    if (aX < bX + bW &&
        aX + aW > bX &&
        aY < bY + bH &&
        aY + aH > bY)
        return true;
    return false;

}

bool
SurviveGame::CircleCircleCollision(float aX, float aY, float aR, float bX,
                                   float bY, float bR) {
    float dx = aX - bX;
    float dy = aY - bY;
    return (sqrt(dx * dx + dy * dy) < aR + bR);
}

bool SurviveGame::RectangleCircleCollision(float aX, float aY, float aR,
                                           float bX, float bY, float bH,
                                           float bW) {

    glm::vec2 circle_center(aX, aY);
    glm::vec2 aabb_half_extents(bW / 2.0f, bH / 2.0f);
    glm::vec2 aabb_center(bX + bW / 2.0, bY + bH / 2.0);
    glm::vec2 difference = circle_center - aabb_center;
    glm::vec2 min = glm::clamp(difference, -aabb_half_extents,
                               aabb_half_extents);
    glm::vec2 closest = aabb_center + min;
    difference = closest - circle_center;
    return glm::length(difference) < aR;
}

void SurviveGame::OnInputUpdate(float deltaTime, int mods) {
    // Movement for player
    if (window->KeyHold(GLFW_KEY_W)) {
        if (!CheckCollisionWallUp())
            player_y += deltaTime * player_speed;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        if (!CheckCollisionWallDown())
            player_y -= deltaTime * player_speed;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        if (!CheckCollisionWallLeft())
            player_x -= deltaTime * player_speed;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        if (!CheckCollisionWallRight())
            player_x += deltaTime * player_speed;
    }
    if (window->KeyHold(GLFW_KEY_R) && player_health <= 0) {
        restart = true;
    }
}

void
SurviveGame::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    if (button == 1) {
        if (reload_projectile > reload_time) {
            projectiles.emplace_back(player_x, player_y, projectile_speed,
                                     player_angle);
            reload_projectile = 0;
        } else if (player_health > 0)
            cout << "Can't shoot. Waiting " << reload_time - reload_projectile
                 << " seconds." << endl;
    }
}

void
SurviveGame::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    glm::ivec2 resolution = window->GetResolution();
    if (mouseX < resolution.x / 2 && mouseY > resolution.y / 2) {
        player_angle = -M_PI + atan2(mouseY - resolution.y / 2,
                                     -mouseX +
                                     resolution.x /
                                     2);
    } else if (mouseX < resolution.x / 2 && mouseY < resolution.y / 2) {
        player_angle = atan2(-mouseY + resolution.y / 2,
                             mouseX - resolution.x / 2);
    } else if (mouseX > resolution.x / 2 && mouseY < resolution.y / 2) {
        player_angle = atan2(resolution.y / 2 - mouseY,
                             mouseX - resolution.x / 2);
    } else if (mouseX > resolution.x / 2 && mouseY > resolution.y / 2) {
        player_angle = atan2(-mouseY + resolution.y / 2,
                             mouseX - resolution.x / 2);
    }
}

glm::mat3 SurviveGame::VisualizationTransf2DUnif(const LogicSpace &logicSpace,
                                                 const ViewportSpace &viewSpace) {
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x +
         (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y +
         (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
            smin, 0.0f, tx,
            0.0f, smin, ty,
            0.0f, 0.0f, 1.0f));
}

void SurviveGame::SetViewportArea(const ViewportSpace &viewSpace) {
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glClearColor(46.0 / 255, 139.0 / 255, 87.0 / 255, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float) viewSpace.x,
                                      (float) (viewSpace.x + viewSpace.width),
                                      (float) viewSpace.y,
                                      (float) (viewSpace.y + viewSpace.height),
                                      0.1f, 400);
    GetSceneCamera()->Update();
}

void SurviveGame::FrameEnd() {
}

void SurviveGame::OnKeyPress(int key, int mods) {
    // Add key press event
}

void SurviveGame::OnKeyRelease(int key, int mods) {
    // Add key release event
}

void
SurviveGame::OnMouseBtnRelease(int mouseX, int mouseY, int button,
                               int mods) {
    // Add mouse button release event
}

void
SurviveGame::OnMouseScroll(int mouseX, int mouseY, int offsetX,
                           int offsetY) {
}

void SurviveGame::OnWindowResize(int width, int height) {
}

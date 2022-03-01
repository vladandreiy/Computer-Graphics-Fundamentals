#include "SurvivalMaze.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

SurvivalMaze::SurvivalMaze() = default;

SurvivalMaze::~SurvivalMaze() = default;

void SurvivalMaze::Init() {
    {
        auto *shader = new Shader("GameShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"),
                          GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"),
                          GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        auto *shader = new Shader("GameShader_2D");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader_2D.glsl"),
                          GL_VERTEX_SHADER);
        shader->AddShader(
                PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader_2D.glsl"),
                GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Mesh *mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh *mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);
    player_pos = glm::vec3(maze_height * wall_scale / 2, 0, wall_scale / 2);
    ThirdPersonCamera();
    GenerateMaze();
    GenerateEnemyCoordinates();
}

void SurvivalMaze::Update(float deltaTimeSeconds) {
    if (win) {
        win = false;
        printf("\nYou finished the maze!\nTime remaining: %f\nHealth: %d\n", time_remaining, (int) player_health);
        time_remaining = -5;
        player_health = -5;
        printf("Press R to play again!\n\n");
    } else if (player_health > 0 && time_remaining > 0) {
        if (!firstPerson)
            RenderPlayer();
        UpdateProjectiles(deltaTimeSeconds);
        RenderMaze();
        UpdateEnemies(deltaTimeSeconds);
        UpdateHealth();
        UpdateTime(deltaTimeSeconds);
        UpdateSprint(deltaTimeSeconds);
    } else {
        projectiles.clear();
        enemies.clear();
        if (restart) {
            restart = false;
            player_health = max_health;
            time_remaining = time_max;
            sprinting_time = max_sprinting_time;
            GenerateMaze();
            GenerateEnemyCoordinates();
            player_pos = glm::vec3(maze_height * wall_scale / 2, 0, wall_scale / 2);
            player_angle = glm::vec3(0, 0, 0);
            firstPerson = false;
            ThirdPersonCamera();
        } else {
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(4, 4, 0));
            if (time_remaining == -5 && player_health == -5)
                RenderSimpleMesh(meshes["box"], shaders["GameShader_2D"], modelMatrix, colors["green"]);
            else
                RenderSimpleMesh(meshes["box"], shaders["GameShader_2D"], modelMatrix, colors["dark_red"]);
        }
    }
}

void SurvivalMaze::UpdateProjectiles(float deltaTimeSeconds) {
    reload_projectile += deltaTimeSeconds;
    for (auto projectile = projectiles.begin(); projectile != projectiles.end();) {
        float x = projectile->position.x + deltaTimeSeconds * projectile_speed * cos(projectile->angle);
        float z = projectile->position.z + deltaTimeSeconds * projectile_speed * sin(projectile->angle);
        float distance =
                projectile->distance + sqrt(pow(x - projectile->position.x, 2) + pow(z - projectile->position.z, 2));

        if (CheckCollisionWalls(glm::vec3(x, projectile->position.y, z), glm::vec3(projectile_scale)) ||
            CheckCollisionEnemies(glm::vec3(x, projectile->position.y, z), glm::vec3(projectile_scale)) ||
            projectile->distance > projectile_max_distance)
            projectile = projectiles.erase(projectile);
        else {
            projectile->distance = distance;
            projectile->position.x = x;
            projectile->position.z = z;
            RenderBodyObject(meshes["box"], projectile->position + glm::vec3(0, 0.6, 0),
                             glm::vec3(0, projectile->angle, 0),
                             glm::vec3(0, 0, 0),
                             glm::vec3(projectile_scale),
                             colors["black"]);
            ++projectile;
        }
    }
}

void SurvivalMaze::UpdateEnemies(float deltaTimeSeconds
) {
    for (auto enemy = enemies.begin(); enemy != enemies.end();) {
        if (enemy->health <= 0) {
            enemy->dying_time += deltaTimeSeconds;
            if (enemy->dying_time > enemy_animation_duration) {
                maze_matrix[enemy->maze_index.x][enemy->maze_index.y] = 0;
                enemy = enemies.erase(enemy);
            } else {
                RenderEnemy(enemy->position, enemy->health);
                ++enemy;
            }
        } else {
            if (enemy->direction == 0) {
                enemy->position.x -= enemy_speed * deltaTimeSeconds;
                if (enemy->position.x < enemy->minPosition.x + enemy_scale.x / 2)
                    enemy->direction = 1;
            }
            if (enemy->direction == 1) {
                enemy->position.z -= enemy_speed * deltaTimeSeconds;
                if (enemy->position.z < enemy->minPosition.z + enemy_scale.z / 2)
                    enemy->direction = 2;
            }
            if (enemy->direction == 2) {
                enemy->position.x += enemy_speed * deltaTimeSeconds;
                if (enemy->position.x > enemy->maxPosition.x - enemy_scale.x / 2)
                    enemy->direction = 3;
            }
            if (enemy->direction == 3) {
                enemy->position.z += enemy_speed * deltaTimeSeconds;
                if (enemy->position.z > enemy->maxPosition.z - enemy_scale.z / 2)
                    enemy->direction = 0;
            }
            RenderEnemy(enemy->position, enemy->health);
            ++enemy;
        }
    }
}

void SurvivalMaze::RenderEnemy(glm::vec3 position, int health) {
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, enemy_scale);
    RenderSimpleMesh(meshes["sphere"], shaders["GameShader"], modelMatrix, colors["enemy"], health);
}

void SurvivalMaze::GenerateEnemyCoordinates() {
    for (int i = maze_height - 1; i > 0; i--) {
        for (int j = maze_width - 1; j > 0; j--) {
            // If cell is free and maximum number of enemies wasn't reached
            if (maze_matrix[i][j] == 0 && enemies.size() < max_enemies &&
                maze_beginning_index != pair<int, int>(i, j)) {
                float renderEnemy = GenerateRandomFloat(0, 1);
                if (renderEnemy > 0.66) {
                    maze_matrix[i][j] = 2;
                    glm::vec3 enemy_coord = glm::vec3(maze_walls[i][j].x, enemy_scale.y / 2, maze_walls[i][j].z);
                    glm::vec3 min_coord = glm::vec3(maze_walls[i][j].x - wall_scale / 2, enemy_scale.y / 2,
                                                    maze_walls[i][j].z - wall_scale / 2);
                    glm::vec3 max_coord = glm::vec3(maze_walls[i][j].x + wall_scale / 2, enemy_scale.y / 2,
                                                    maze_walls[i][j].z + wall_scale / 2);
                    int direction = (int) GenerateRandomFloat(0, 4);
                    enemies.emplace_back(enemy_coord, min_coord, max_coord, glm::vec2(i, j), direction);
                }
            }
        }
    }
}

void SurvivalMaze::UpdateTime(float deltaTime) {
    if (time_remaining > 0) {
        time_remaining -= deltaTime;
        if (time_remaining <= 0)
            printf("You ran out of time! Press R to restart.\n");
    }
    // Time
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix,
                                 time_bar_position +
                                 glm::vec3((-time_max + time_remaining) / (2 * time_max), 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(time_remaining / time_max, 0.1, 0));
    RenderSimpleMesh(meshes["box"], shaders["GameShader_2D"], modelMatrix, colors["blue"]);

    // Time Outline
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, time_bar_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.1, 0));
    RenderSimpleMesh(meshes["box"], shaders["GameShader_2D"], modelMatrix, colors["grey"]);
}

void SurvivalMaze::UpdateHealth() {
    // Health
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix,
                                 health_bar_position +
                                 glm::vec3(0, (-max_health + player_health) / (2 * max_health / 1.5), 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 1.5 * player_health / max_health, 0));
    RenderSimpleMesh(meshes["box"], shaders["GameShader_2D"], modelMatrix, colors["red"]);

    // Health Outline
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, health_bar_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 1.5, 0));
    RenderSimpleMesh(meshes["box"], shaders["GameShader_2D"], modelMatrix, colors["grey"]);
}

void SurvivalMaze::UpdateSprint(float deltaTimeSeconds) {
    if (sprinting_time > max_sprinting_time / 4)
        can_sprint = true;
    if (sprinting) {
        if (sprinting_time < 0.1)
            can_sprint = false;
        sprinting_time -= deltaTimeSeconds;
    } else if (sprinting_time < max_sprinting_time) {
        sprint_regen_speed += deltaTimeSeconds;
        sprinting_time += 0.2 * sprint_regen_speed * sprinting_time * deltaTimeSeconds;
    }
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix,
                                 sprint_bar_position +
                                 glm::vec3((-max_sprinting_time + sprinting_time) / (2 * max_sprinting_time), 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(sprinting_time / max_sprinting_time, 0.1, 0));
    RenderSimpleMesh(meshes["box"], shaders["GameShader_2D"], modelMatrix, colors["sprint"]);
}

void SurvivalMaze::RenderPlayer() {
    RenderBodyObject(meshes["box"], player_pos, player_angle, glm::vec3(0, 0.625, 0), glm::vec3(0.4, 0.4, 0.2),
                     colors["cyan"], player_health);
    RenderBodyObject(meshes["box"], player_pos, player_angle, glm::vec3(-0.105, 0.2, 0), glm::vec3(0.175, 0.4, 0.2),
                     colors["blue"], player_health);
    RenderBodyObject(meshes["box"], player_pos, player_angle, glm::vec3(0.105, 0.2, 0), glm::vec3(0.175, 0.4, 0.2),
                     colors["blue"], player_health);
    RenderBodyObject(meshes["box"], player_pos, player_angle, glm::vec3(0, 0.95, 0), glm::vec3(0.2, 0.2, 0.2),
                     colors["skin"], player_health);
    RenderBodyObject(meshes["box"], player_pos, player_angle, glm::vec3(-0.29, 0.65, 0), glm::vec3(0.15, 0.35, 0.2),
                     colors["cyan"], player_health);
    RenderBodyObject(meshes["box"], player_pos, player_angle, glm::vec3(0.29, 0.65, 0), glm::vec3(0.15, 0.35, 0.2),
                     colors["cyan"], player_health);
    RenderBodyObject(meshes["box"], player_pos, player_angle, glm::vec3(-0.29, 0.41, 0), glm::vec3(0.15, 0.1, 0.2),
                     colors["skin"], player_health);
    RenderBodyObject(meshes["box"], player_pos, player_angle, glm::vec3(0.29, 0.41, 0), glm::vec3(0.15, 0.1, 0.2),
                     colors["skin"], player_health);
}

void
SurvivalMaze::RenderBodyObject(Mesh *bodyPart, glm::vec3 position, glm::vec3 angle, glm::vec3 offset,
                               glm::vec3 scale,
                               glm::vec3 object_colour, float health) {
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, angle.x, glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, angle.y, glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, angle.z, glm::vec3(0, 0, 1));
    modelMatrix = glm::translate(modelMatrix, offset);
    modelMatrix = glm::scale(modelMatrix, scale);
    RenderSimpleMesh(bodyPart, shaders["GameShader"], modelMatrix, object_colour, health);
}

glm::vec3 SurvivalMaze::GetPlayerEyes() const {
    return {player_pos.x, player_pos.y + 1, player_pos.z + 0.1};
}

void SurvivalMaze::FirstPersonCamera() {
    camera->Set(GetPlayerEyes(), glm::vec3(player_pos.x, 1, player_pos.z + 1),
                glm::vec3(0, 1, 0));
    camera->RotateFirstPerson_OY(player_angle.y);
}

void SurvivalMaze::ThirdPersonCamera() {
    camera->Set(player_pos + third_person_pos_offset, player_pos + third_person_center_offset,
                glm::vec3(0, 1, 0));
    float x = third_person_pos_offset.x - third_person_center_offset.x;
    float y = third_person_pos_offset.y - third_person_center_offset.y;
    float z = third_person_pos_offset.z - third_person_center_offset.z;
    camera->distanceToTarget = sqrt(x * x + y * y + z * z);
    camera->RotateThirdPerson_OY(player_angle.y);
}

void SurvivalMaze::OnInputUpdate(float deltaTime, int mods) {
    float playerSpeed;
    if (window->KeyHold(GLFW_KEY_LEFT_SHIFT) && can_sprint) {
        playerSpeed = sprinting_speed;
        sprinting = true;
        sprint_regen_speed = 0;
    } else {
        sprinting = false;
        playerSpeed = walking_speed;
    }
    if (window->KeyHold(GLFW_KEY_W)) {
        float x = player_pos.x + deltaTime * playerSpeed * sin(player_angle.y);
        float z = player_pos.z + deltaTime * playerSpeed * cos(player_angle.y);
        if (!CheckCollisionWalls(glm::vec3(x, 0, z), player_dim)) {
            player_pos.x = x;
            player_pos.z = z;
            camera->MoveForward(deltaTime * playerSpeed);
        }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        float x = player_pos.x + deltaTime * playerSpeed * cos(player_angle.y);
        float z = player_pos.z - deltaTime * playerSpeed * sin(player_angle.y);
        if (!CheckCollisionWalls(glm::vec3(x, 0, z), player_dim)) {
            player_pos.x = x;
            player_pos.z = z;
            camera->TranslateRight(-deltaTime * playerSpeed);
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        float z = player_pos.z - deltaTime * playerSpeed * cos(player_angle.y);
        float x = player_pos.x - deltaTime * playerSpeed * sin(player_angle.y);
        if (!CheckCollisionWalls(glm::vec3(x, 0, z), player_dim)) {
            player_pos.x = x;
            player_pos.z = z;
            camera->MoveForward(-deltaTime * playerSpeed);
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        float z = player_pos.z + deltaTime * playerSpeed * sin(player_angle.y);
        float x = player_pos.x - deltaTime * playerSpeed * cos(player_angle.y);
        if (!CheckCollisionWalls(glm::vec3(x, 0, z), player_dim)) {
            player_pos.x = x;
            player_pos.z = z;
            camera->TranslateRight(deltaTime * playerSpeed);
        }
    }

    CheckCollisionEnemies(player_pos, player_dim, true);
    if (player_health > 0 &&
        AABBCollision(player_pos, player_dim, maze_end_pos, glm::vec3(wall_scale, 0, wall_scale))) {
        win = true;
    }

    if (window->KeyHold(GLFW_KEY_R) && (player_health <= 0 || time_remaining <= 0)) {
        printf("Restarting game.\n\n");
        restart = true;
    }
}

bool SurvivalMaze::CheckCollisionWalls(glm::vec3 position, glm::vec3 dimensions) {
    for (int i = 0; i < maze_height; i++) {
        for (int j = 0; j < maze_width; j++) {
            if (maze_matrix[i][j] == 1 &&
                AABBCollision(position, dimensions, maze_walls[i][j], glm::vec3(wall_scale))) {
                return true;
            }
        }
    }
    if (AABBCollision(position, dimensions, maze_walls[maze_height / 2][0] - glm::vec3(0, 0, wall_scale),
                      glm::vec3(wall_scale)))
        return true;
    return false;
}

bool SurvivalMaze::CheckCollisionEnemies(glm::vec3 position, glm::vec3 dimensions, bool player) {
    for (auto &enemy: enemies) {
        if (AABBCollision(position, dimensions, enemy.position, enemy_scale)) {
            if (player) {
                if (enemy.health > 0) {
                    player_health--;
                    if (player_health <= 0) {
                        printf("You died! Press R to restart the game.\n");
                    }
                }
                enemy.health = 0;
            }
            if (enemy.health > 0)
                enemy.health--;
            return true;
        }
    }
    return false;
}

bool SurvivalMaze::AABBCollision(glm::vec3 posA, glm::vec3 dimA, glm::vec3 posB, glm::vec3 dimB) {
    return (posA.x - dimA.x / 2 <= posB.x + dimB.x / 2 && posA.x + dimA.x / 2 >= posB.x - dimB.x / 2) &&
           (posA.y - dimA.y / 2 <= posB.y + dimB.y / 2 && posA.y + dimA.y / 2 >= posB.y - dimB.y / 2) &&
           (posA.z - dimA.z / 2 <= posB.z + dimB.z / 2 && posA.z + dimA.z / 2 >= posB.z - dimB.z / 2);
}


void SurvivalMaze::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    // Camera moves only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        if (firstPerson) {
            camera->RotateFirstPerson_OY(-sensitivityOY * deltaX);
            player_angle.y -= sensitivityOY * deltaX;
        } else {
            player_angle.y -= sensitivityOY * deltaX;
            camera->RotateThirdPerson_OY(-sensitivityOY * deltaX);
        }
    }
}


void SurvivalMaze::GenerateMaze() {
    maze_matrix = vector<vector<int >>(maze_height, vector<int>(maze_width, 1));
    for (int i = 1; i < maze_height; i += 2) {
        for (int j = 1; j < maze_width; j += 2) {
            maze_matrix[i][j] = 0;
            pair<int, int> neighbor = FindNeighborBinary(i, j);
            maze_matrix[neighbor.first][neighbor.second] = 0;
        }
    }
    maze_matrix[maze_height / 2][0] = 0;
    maze_beginning_index = pair<int, int>(maze_height / 2, 0);
    while (true) {
        maze_end_index = pair<int, int>(int(GenerateRandomFloat(0, maze_height)),
                                        int(GenerateRandomFloat(0, maze_width)));
        if (maze_matrix[maze_end_index.first][maze_end_index.second] == 0 && (
                maze_end_index.first - maze_beginning_index.first > maze_width / 2 ||
                maze_end_index.second - maze_beginning_index.second > maze_height / 2))
            break;
    }

    maze_walls = vector<vector<glm::vec3 >>(maze_height, std::vector<glm::vec3>(maze_width, glm::vec3(0)));
    for (int i = 0; i < maze_height; i++) {
        for (int j = 0; j < maze_width; j++) {
            maze_walls[i][j] = glm::vec3(wall_scale * i + wall_scale / 2, wall_scale / 2,
                                         wall_scale * j + wall_scale / 2);
        }
    }
    maze_beginning_pos = glm::vec3(maze_walls[maze_beginning_index.first][maze_beginning_index.second].x, 0.02,
                                   maze_walls[maze_beginning_index.first][maze_beginning_index.second].z);
    maze_end_pos = glm::vec3(maze_walls[maze_end_index.first][maze_end_index.second].x, 0.02,
                             maze_walls[maze_end_index.first][maze_end_index.second].z);
}

pair<int, int> SurvivalMaze::FindNeighborBinary(int curr_row, int curr_col) {
    vector<pair<int, int>> skews{{1, 0},
                                 {0, -1}};
    vector<pair<int, int>> neighbors;
    for (auto &skew: skews) {
        int neighbor_row = curr_row + skew.first;
        int neighbor_col = curr_col + skew.second;
        if (neighbor_row > 0 && neighbor_row < maze_height - 1 &&
            neighbor_col > 0 && neighbor_col < maze_width - 1)
            neighbors.emplace_back(neighbor_row, neighbor_col);
    }
    if (neighbors.empty())
        return {curr_row, curr_col};
    return neighbors[(int) GenerateRandomFloat(0, neighbors.size())];
}


void SurvivalMaze::PrintHint() {
    printf("\nMaze Hint:\n");
    char string[8];
    for (int i = 0; i < maze_height; i++) {
        for (int j = 0; j < maze_width; j++) {
            if (maze_beginning_index == pair<int, int>(i, j))
                strcpy(string, "S ");
            else if (maze_end_index == pair<int, int>(i, j))
                strcpy(string, "E ");
            else if (maze_matrix[i][j] == 2)
                strcpy(string, "**");
            else if (maze_matrix[i][j] == 1)
                strcpy(string, "▓▓");
            else if (maze_matrix[i][j] == 0)
                strcpy(string, "  ");
            if (player_pos.x > maze_walls[i][j].x - wall_scale / 2 &&
                player_pos.x < maze_walls[i][j].x + wall_scale / 2 &&
                player_pos.z > maze_walls[i][j].z - wall_scale / 2 &&
                player_pos.z < maze_walls[i][j].z + wall_scale / 2)
                strcpy(string, "P ");
            printf("%s", string);
        }
        printf("\n");
    }
    printf("S = Starting Point\n");
    printf("E = Maze End\n");
    printf("P = Player Position\n");
    printf("** = Enemy\n");
    printf("▓▓ = Wall\n");
}

void SurvivalMaze::RenderMaze() {
    for (int i = 0; i < maze_height; i++) {
        for (int j = 0; j < maze_width; j++) {
            if (maze_matrix[i][j] == 1) {
                RenderBodyObject(meshes["box"], maze_walls[i][j], glm::vec3(0), glm::vec3(0), glm::vec3(wall_scale),
                                 colors["brown"]);
            } else if (maze_beginning_index == pair<int, int>(i, j)) {
                RenderBodyObject(meshes["box"], maze_beginning_pos,
                                 glm::vec3(0), glm::vec3(0),
                                 glm::vec3(wall_scale, 0, wall_scale),
                                 colors["yellow"]);
            } else if (maze_end_index == pair<int, int>(i, j)) {
                RenderBodyObject(meshes["box"], maze_end_pos,
                                 glm::vec3(0), glm::vec3(0),
                                 glm::vec3(wall_scale, 0, wall_scale),
                                 colors["yellow"]);
            } else {
                RenderBodyObject(meshes["box"], glm::vec3(maze_walls[i][j].x, 0.02, maze_walls[i][j].z),
                                 glm::vec3(0),
                                 glm::vec3(0),
                                 glm::vec3(wall_scale, 0, wall_scale),
                                 colors["green"]);
            }
        }
    }
    RenderBodyObject(meshes["box"], maze_walls[maze_height / 2][0] - glm::vec3(0, 0, wall_scale), glm::vec3(0),
                     glm::vec3(0), glm::vec3(wall_scale),
                     colors["brown"]);
}

void SurvivalMaze::OnKeyPress(int key, int mods) {
    if (key == GLFW_KEY_LEFT_CONTROL) {
        firstPerson = !firstPerson;
        if (firstPerson)
            FirstPersonCamera();
        else
            ThirdPersonCamera();
    }
    if (key == GLFW_KEY_H)
        PrintHint();
}

void SurvivalMaze::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    if (button == 1) {
        if (!firstPerson)
            cout << "Can't shoot in third person." << endl;
        else if (reload_projectile > reload_time) {
            projectiles.emplace_back(player_pos, projectile_speed,
                                     M_PI / 2 - player_angle.y);
            reload_projectile = 0;
        }
    }
}

void
SurvivalMaze::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color,
                               float health) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;
    glUseProgram(shader->program);
    glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "View"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE,
                       glm::value_ptr(projectionMatrix));
    glUniform1f(glGetUniformLocation(shader->program, "object_health"), health);
    glUniform1f(glGetUniformLocation(shader->program, "Time"), Engine::GetElapsedTime());
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, nullptr);
}

float SurvivalMaze::GenerateRandomFloat(float min, float max) {
    return min + ((float) (rand()) / ((float) (RAND_MAX / (max - min))));
}

void SurvivalMaze::FrameStart() {
    glClearColor(112.0 / 255, 186.0 / 255, 255.0 / 255, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void SurvivalMaze::FrameEnd() {
//    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void SurvivalMaze::OnKeyRelease(int key, int mods) {
}

void SurvivalMaze::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
}

void SurvivalMaze::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}

void SurvivalMaze::OnWindowResize(int width, int height) {
}

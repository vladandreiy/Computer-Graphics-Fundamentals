#include "DiscoParty.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;

DiscoParty::DiscoParty() = default;


DiscoParty::~DiscoParty() = default;


void DiscoParty::Init() {
    // Position camera
    GetSceneCamera()->SetPosition(glm::vec3(dance_floor_width / 2, wall_height / 2, dance_floor_height + 3));

    // Random texture
    {
        mapTextures["random"] = CreateRandomTexture(disco_ball_size, disco_ball_size);
    }

    // Load meshes
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

    CreateConeMesh();
    CreateSquareMesh();

    // Create and link shader
    {
        auto *shader = new Shader("DiscoShader");
        shader->AddShader(
                PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "VertexShader.glsl"),
                GL_VERTEX_SHADER);
        shader->AddShader(
                PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FragmentShader.glsl"),
                GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    GenerateDanceFloor();
    GenerateDancersCoordinates();
    GenerateSpotlights();
}

void DiscoParty::GenerateDancersCoordinates() {
    dancers[0].initial_position = glm::vec3(dance_floor_width / 4, dancers_scale.y / 2, dance_floor_height / 4);
    dancers[1].initial_position = glm::vec3(3 * dance_floor_width / 4, dancers_scale.y / 2, dance_floor_height / 4);
    dancers[2].initial_position = glm::vec3(dance_floor_width / 4, dancers_scale.y / 2, 3 * dance_floor_height / 4);
    dancers[3].initial_position = glm::vec3(3 * dance_floor_width / 4, dancers_scale.y / 2, 3 * dance_floor_height / 4);

    for (int i = 0; i < dancers_no; i++) {
        if (i > 3)
            dancers[i].initial_position = glm::vec3(GenerateRandomFloat(2 * dancers_scale.x, dance_floor_width * 4 / 5),
                                                    dancers_scale.y / 2,
                                                    GenerateRandomFloat(2 * dancers_scale.z,
                                                                        dance_floor_height * 4 / 5));
        dancers[i].position = dancers[i].initial_position;
        dancers[i].funX = [](double a) {
            if (GenerateRandomFloat(0, 1) > 0)
                return 2 * (1 - cos(a)) * cos(a);
            else if (GenerateRandomFloat(0, 1) > 0.5)
                return sin(a);
            else
                return cos(a);
        };
        dancers[i].funZ = [](double a) {
            if (GenerateRandomFloat(0, 1) > 0)
                return 2 * (1 - cos(a)) * sin(a);
            else if (GenerateRandomFloat(0, 1) > 0.5)
                return sin(a);
            else
                return cos(a);
        };
        dancers[i].dance_moves = glm::vec3(GenerateRandomFloat(0.3, 1), GenerateRandomFloat(0.3, 1),
                                           GenerateRandomFloat(0.3, 1));
    }
}


void DiscoParty::GenerateDanceFloor() {
    for (int i = 0, k = 0; i < dance_floor_height; i++) {
        for (int j = 0; j < dance_floor_width; j++, k++) {
            dance_floor_matrix[i][j].position = glm::vec3(j + dance_floor_scale / 2, 0, i + dance_floor_scale / 2);
            dance_floor_matrix[i][j].color = GenerateRandomColor();
            pointLightPos[k] = dance_floor_matrix[i][j].position;
            pointLightColor[k] = dance_floor_matrix[i][j].color;
        }
    }

}


void DiscoParty::GenerateSpotlights() {
    spotlightPos[0] = glm::vec3(dance_floor_width / 4, wall_height, dance_floor_height / 4);
    spotlightPos[1] = glm::vec3(dance_floor_width / 4, wall_height, 3 * dance_floor_height / 4);
    spotlightPos[2] = glm::vec3(3 * dance_floor_width / 4, wall_height, dance_floor_height / 4);
    spotlightPos[3] = glm::vec3(3 * dance_floor_width / 4, wall_height, 3 * dance_floor_height / 4);
    spotlightColor[0] = glm::vec3(1, 0, 0);
    spotlightColor[1] = glm::vec3(0, 1, 0);
    spotlightColor[2] = glm::vec3(0, 0, 1);
    spotlightColor[3] = glm::vec3(1, 1, 0);
}

void DiscoParty::Update(float deltaTimeSeconds) {
    RenderDanceFloor();
    RenderDiscoBall();
    RenderDancers(deltaTimeSeconds);
    RenderWalls();
    RenderSpotlights(deltaTimeSeconds);
}

void DiscoParty::RenderSpotlights(float seconds) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < 4; i++) {
        modelMatrix = glm::mat4(1);
        glm::vec3 pos = spotlightPos[i];
        modelMatrix = glm::translate(modelMatrix, glm::vec3(pos.x, pos.y - wall_height, pos.z));
        RenderSimpleMesh(meshes["cone"], shaders["DiscoShader"], modelMatrix, glm::vec3(1, 1, 1));
    }
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
}


void DiscoParty::RenderDancers(float deltaTimeSeconds) {
    total_time += deltaTimeSeconds;
    for (int i = 0; i < dancers_no; i++) {
        float scale = 2 / (3 - cos(2 * total_time));
        float x = dancers[i].dance_moves.x * scale * cos(dancers[i].funX(dancers[i].dance_moves.x) * total_time);
        float z = dancers[i].dance_moves.z * scale * sin(dancers[i].dance_moves.z * 2 * total_time) / 2;
        dancers[i].position.x = dancers[i].initial_position.x + x;
        dancers[i].position.z = dancers[i].initial_position.z + z;
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dancers[i].position);
        modelMatrix = glm::scale(modelMatrix, dancers_scale);
        RenderSimpleMesh(meshes["box"], shaders["DiscoShader"], modelMatrix, dancers_color);
    }
}


void DiscoParty::RenderDiscoBall() {
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, disco_ball_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(disco_ball_scale));
    RenderSimpleMesh(meshes["sphere"], shaders["DiscoShader"], modelMatrix, glm::vec3(0.5),
                     mapTextures["random"]);
}


void DiscoParty::RenderDanceFloor() {
    for (int i = 0; i < dance_floor_height; i++) {
        for (int j = 0; j < dance_floor_width; j++) {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, dance_floor_matrix[i][j].position);
            modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
            RenderSimpleMesh(meshes["square"], shaders["DiscoShader"], modelMatrix, dance_floor_matrix[i][j].color);
        }
    }
}

void DiscoParty::RenderWalls() {
    // Back wall
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(dance_floor_width / 2, wall_height / 2, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(dance_floor_width, wall_height, 0));
    RenderSimpleMesh(meshes["square"], shaders["DiscoShader"], modelMatrix, glm::vec3(0));

    // Left wall
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix,
                                 glm::vec3(dance_floor_width, wall_height / 2, dance_floor_height / 2));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(dance_floor_height, wall_height, 0));
    RenderSimpleMesh(meshes["square"], shaders["DiscoShader"], modelMatrix, glm::vec3(0));

    // Right wall
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, wall_height / 2, dance_floor_height / 2));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(dance_floor_height, wall_height, 0));
    RenderSimpleMesh(meshes["square"], shaders["DiscoShader"], modelMatrix, glm::vec3(0));

    // Ceiling
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix,
                                 glm::vec3(dance_floor_width / 2, wall_height, dance_floor_height / 2));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(dance_floor_width, dance_floor_height, 0));
    RenderSimpleMesh(meshes["square"], shaders["DiscoShader"], modelMatrix, glm::vec3(0));
}

void DiscoParty::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color,
                                  Texture2D *texture1) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "View"), 1, GL_FALSE,
                       glm::value_ptr(GetSceneCamera()->GetViewMatrix()));

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE,
                       glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));

    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    glUniform3f(glGetUniformLocation(shader->program, "eye_position"), eyePosition.x, eyePosition.y, eyePosition.z);

    bool has_texture = (texture1 != nullptr);
    glUniform1i(glGetUniformLocation(shader->program, "has_texture"), has_texture);

    bool is_disco_ball = (meshes["sphere"] == mesh);
    glUniform1i(glGetUniformLocation(shader->program, "is_disco_ball"), is_disco_ball);

    bool is_cone = (meshes["cone"] == mesh);
    glUniform1i(glGetUniformLocation(shader->program, "is_cone"), is_cone);

    bool is_floor = (meshes["square"] == mesh);
    glUniform1i(glGetUniformLocation(shader->program, "is_floor"), is_floor);

    glUniform1i(glGetUniformLocation(shader->program, "disco_ball_light"), disco_ball_light);
    glUniform1i(glGetUniformLocation(shader->program, "dance_floor_light"), dance_floor_light);
    glUniform1i(glGetUniformLocation(shader->program, "spotlight_light"), spotlight_light);
    glUniform3f(glGetUniformLocation(shader->program, "light_direction"), initialLightDirection.x,
                initialLightDirection.y, initialLightDirection.z);

    float time = Engine::GetElapsedTime();
    glUniform1f(glGetUniformLocation(shader->program, "time"), time);

    glUniform3f(glGetUniformLocation(shader->program, "object_color"), color.r, color.g, color.b);
    glUniform3f(glGetUniformLocation(shader->program, "disco_ball_position"), disco_ball_position.x,
                disco_ball_position.y, disco_ball_position.z);

    glUniform3fv(glGetUniformLocation(shader->program, "pointLightPos"), 64, glm::value_ptr(pointLightPos[0]));
    glUniform3fv(glGetUniformLocation(shader->program, "pointLightColor"), 64, glm::value_ptr(pointLightColor[0]));

    glUniform3fv(glGetUniformLocation(shader->program, "spotlightPos"), 4, glm::value_ptr(spotlightPos[0]));
    glUniform3fv(glGetUniformLocation(shader->program, "spotlightColor"), 4, glm::value_ptr(spotlightColor[0]));

    if (texture1) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
    }

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, nullptr);
}

Texture2D *DiscoParty::CreateRandomTexture(unsigned int width, unsigned int height) {
    GLuint textureID = 0;
    unsigned int channels = 3;
    unsigned int size = width * height * channels;
    auto *data = new unsigned char[size];

    // TODO(student): Generate random texture data
    for (int i = 0; i < size; i++) {
        data[i] = rand() % 256;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if (GLEW_EXT_texture_filter_anisotropic) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    CheckOpenGLError();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    CheckOpenGLError();

    auto *texture = new Texture2D();
    texture->Init(textureID, width, height, channels);

    SAFE_FREE_ARRAY(data);
    return texture;
}

void DiscoParty::CreateSquareMesh() {
    vector<glm::vec3> vertices
            {
                    glm::vec3(0.5f, 0.5f, 0.0f),    // top right
                    glm::vec3(0.5f, -0.5f, 0.0f),    // bottom right
                    glm::vec3(-0.5f, -0.5f, 0.0f),    // bottom left
                    glm::vec3(-0.5f, 0.5f, 0.0f),    // top left
            };

    vector<glm::vec3> normals
            {
                    glm::vec3(0, 1, 1),
                    glm::vec3(1, 0, 1),
                    glm::vec3(1, 0, 0),
                    glm::vec3(0, 1, 0)
            };

    vector<glm::vec2> textureCoords
            {
                    glm::vec2(0.0f, 0.0f),
                    glm::vec2(0.0f, 1.0f),
                    glm::vec2(1.0f, 1.0f),
                    glm::vec2(1.0f, 0.0f)

            };

    vector<unsigned int> indices =
            {
                    0, 1, 3,
                    1, 2, 3
            };

    Mesh *mesh = new Mesh("square");
    mesh->InitFromData(vertices, normals, textureCoords, indices);
    meshes[mesh->GetMeshID()] = mesh;
}

void DiscoParty::CreateConeMesh() {
    float radius = 1;
    vector<glm::vec3> vertices;
    vector<unsigned> indices;
    unsigned trianglesNo = 20;
    for (int i = 0; i < trianglesNo; i++) {
        float arg = 2 * M_PI * i / trianglesNo;
        vertices.emplace_back(glm::vec3(cos(arg) * radius, 0, sin(arg) * radius));
    }
    vertices.emplace_back(0, 4, 0);
    vertices.emplace_back(0, 0, 0);
    for (int i = 0; i < trianglesNo - 1; i++) {
        indices.push_back(trianglesNo);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(trianglesNo);
    indices.push_back(trianglesNo - 1);
    indices.push_back(0);

    vector<glm::vec3> normals
            {
                    glm::vec3(0, 1, 1),
                    glm::vec3(1, 0, 1),
                    glm::vec3(1, 0, 0),
                    glm::vec3(0, 1, 0)
            };

    vector<glm::vec2> textureCoords
            {
                    glm::vec2(0.0f, 0.0f),
                    glm::vec2(0.0f, 1.0f),
                    glm::vec2(1.0f, 1.0f),
                    glm::vec2(1.0f, 0.0f)

            };

    Mesh *mesh = new Mesh("cone");
    mesh->InitFromData(vertices, normals, textureCoords, indices);
    meshes[mesh->GetMeshID()] = mesh;
}

glm::vec3 DiscoParty::GenerateRandomColor() {
    return {GenerateRandomFloat(0, 1), GenerateRandomFloat(0, 1), GenerateRandomFloat(0, 1)};
}

float DiscoParty::GenerateRandomFloat(float min, float max) {
    return min + ((float) (rand()) / ((float) (RAND_MAX / (max - min))));
}

void DiscoParty::OnInputUpdate(float deltaTime, int mods) {
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
        glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
    }
}

void DiscoParty::FrameStart() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void DiscoParty::OnKeyPress(int key, int mods) {
    if (key == GLFW_KEY_H)
        disco_ball_light = !disco_ball_light;
    if (key == GLFW_KEY_J)
        dance_floor_light = !dance_floor_light;
    if (key == GLFW_KEY_K)
        spotlight_light = !spotlight_light;
}

void DiscoParty::FrameEnd() {
}

void DiscoParty::OnKeyRelease(int key, int mods) {
}

void DiscoParty::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
}

void DiscoParty::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
}

void DiscoParty::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
}

void DiscoParty::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}

void DiscoParty::OnWindowResize(int width, int height) {
}

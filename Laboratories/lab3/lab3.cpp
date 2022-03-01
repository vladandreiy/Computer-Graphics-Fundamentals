#include "lab_m1/lab3/lab3.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;

#define MOVE_RATIO         (.25f)
#define LIMIT           (.5f)
#define SQUARES_DISTANCE   (100.f)
#define SCALE_FACTOR_X     (1.5f)
#define SCALE_FACTOR_Y     (2.f)

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab3::Lab3()
{
}


Lab3::~Lab3()
{
}


void Lab3::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;

    // TODO(student): Compute coordinates of a square's center, and store
    // then in the `cx` and `cy` class variables (see the header). Use
    // `corner` and `squareSide`. These two class variables will be used
    // in the `Update()` function. Think about it, why do you need them?

    direction = 1; // 0 - left, 1 - right
    scale = 1; // 0 - scale down, 1 - scale up

    squareX = 0;
    squareY = 400;

    pulseX = 1;
    pulseY = 1;

    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize angularStep
    angularStep = 0;

    Mesh* square1 = object2D::CreateSquare("square1", corner, squareSide, glm::vec3(1, 0, 0), true);
    AddMeshToList(square1);

    Mesh* square2 = object2D::CreateSquare("square2", corner, squareSide, glm::vec3(0, 1, 0));
    AddMeshToList(square2);

    Mesh* square3 = object2D::CreateSquare("square3", corner, squareSide, glm::vec3(0, 0, 1));
    AddMeshToList(square3);

    Mesh* square4 = object2D::CreateSquare("square4", corner, squareSide, glm::vec3(1, 1, 0), true);
    AddMeshToList(square4);

    Mesh* square5 = object2D::CreateSquare("square5", corner, squareSide, glm::vec3(0, 0, 1), true);
    AddMeshToList(square5);
}


void Lab3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab3::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();
    float squareSide = 100;
    // TODO(student): Update steps for translation, rotation and scale,
    // in order to create animations. Use the class variables in the
    // class header, and if you need more of them to complete the task,
    // add them over there!

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(3 * resolution.x / 4, resolution.y / 5);
    if (scale) {
        pulseX += SCALE_FACTOR_X * deltaTimeSeconds;
        pulseY += SCALE_FACTOR_Y * deltaTimeSeconds;
    }
    else {
        pulseX -= SCALE_FACTOR_X * deltaTimeSeconds;
        pulseY -= SCALE_FACTOR_Y * deltaTimeSeconds;
    }
    if (pulseX > 1.5)
        scale = 0;
    else if (pulseX < 0.5)
        scale = 1;
    modelMatrix *= transform2D::Translate(squareSide / 2, squareSide / 2);
    modelMatrix *= transform2D::Scale(pulseX, pulseY);
    modelMatrix *= transform2D::Translate(-squareSide / 2, -squareSide / 2);


    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented.
    // Remember, the last matrix in the chain will take effect first!

    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented
    // Remember, the last matrix in the chain will take effect first!
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(resolution.x / 4, resolution.y / 5);
    angularStep += deltaTimeSeconds;
    modelMatrix *= transform2D::Rotate(angularStep);
    RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);

    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented
    // Remember, the last matrix in the chain will take effect first!
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(2 * resolution.x / 4, resolution.y / 5);
    scaleX += SCALE_FACTOR_X * deltaTimeSeconds;
    scaleY += SCALE_FACTOR_Y * deltaTimeSeconds;
    modelMatrix *= transform2D::Scale(cos(scaleX), sin(scaleY));
    RenderMesh2D(meshes["square3"], shaders["VertexColor"], modelMatrix);

    // Cube B
    modelMatrix = glm::mat3(1);
    if (direction)
        translateX += 0.25 * deltaTimeSeconds * resolution.x;
    else
        translateX -= 0.25 * deltaTimeSeconds * resolution.x;
    if (translateX > 3 * resolution.x / 4)
        direction = 0;
    else if (translateX < resolution.x / 8)
        direction = 1;
    modelMatrix *= transform2D::Translate(squareX, squareY);
    translateX += 0.01 * deltaTimeSeconds * resolution.x;
    modelMatrix *= transform2D::Translate(translateX, translateY);
    RenderMesh2D(meshes["square4"], shaders["VertexColor"], modelMatrix);

    // Cube A
    // Keep the transformation matrix for the square orbiting
    // Rotate around its axis => rotation centered in the middle
    modelMatrix *= transform2D::Translate(squareSide / 2, squareSide / 2);
    modelMatrix *= transform2D::Rotate(2 * angularStep);
    // Rotate around its axis => rotation centered in the middle
    modelMatrix *= transform2D::Translate(-squareSide / 2, -squareSide / 2);
    // Keep the cubes away from each other
    modelMatrix *= transform2D::Translate(150, 150);
    RenderMesh2D(meshes["square5"], shaders["VertexColor"], modelMatrix);
}


void Lab3::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab3::OnInputUpdate(float deltaTime, int mods)
{
}


void Lab3::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Lab3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab3::OnWindowResize(int width, int height)
{
}

#pragma once

#include "components/simple_scene.h"
#include "lab_m1/lab5/lab_camera.h"

namespace m1 {
    class SurvivalMaze : public gfxc::SimpleScene {
    public:
        SurvivalMaze();

        ~SurvivalMaze();

        void Init() override;

    private:
        void FrameStart() override;

        void Update(float deltaTimeSeconds) override;

        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;

        void OnKeyPress(int key, int mods) override;

        void OnKeyRelease(int key, int mods) override;

        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;

        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

        void OnWindowResize(int width, int height) override;

    protected:
        implemented::Camera *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        float fov = 60;
        // projectionType: 0 (default) - perspective, 1 - ortographic
        int projectionType = 0;
        float bottom = 0.01;
        float top = 10;
        float right = 10;
        float left = 0.01;
        float zNear = 0.01;
        float zFar = 200;

    };
}   // namespace m1

#pragma once

#include "components/simple_scene.h"

#define SPEED_FACTOR 2.0
namespace m1
{
    class Lab1 : public gfxc::SimpleScene
    {
     public:
        Lab1();
        ~Lab1();

        void Init() override;

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

        void SetColor(int red, int green, int blue);

        // TODO(student): Class variables go here
        float red, green, blue;
        float posX, posY, posZ;
        float speed;
        Mesh* mesh;
        int objectIndex;
        static const std::vector<std::string> meshNames;

    };

}   // namespace m1



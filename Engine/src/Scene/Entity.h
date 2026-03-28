#pragma once
#include <raylib-cpp.hpp>
#include <memory>
#include <string>

namespace Core 
{
    struct FTransform {
        raylib::Vector3 Position{0,0,0};
        raylib::Vector3 RotationAxis{0,1,0};
        float Angle = 0.0f;
        raylib::Vector3 Scale{1,1,1};
    };

    struct FEntity {
        std::string Name;
        std::shared_ptr<raylib::Model> Model;
        FTransform Transform;
        raylib::Color EntColor = raylib::Color(255, 255, 255, 255);
        bool bDrawWireframe = false;

        void Draw() {
            if (Model && Model->IsValid()) {
                if (bDrawWireframe)
                    Model->DrawWires(Transform.Position, Transform.RotationAxis, Transform.Angle, Transform.Scale, EntColor);
                else
                    Model->Draw(Transform.Position, Transform.RotationAxis, Transform.Angle, Transform.Scale, EntColor);
            }
        }
    };
}

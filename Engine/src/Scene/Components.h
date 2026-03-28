#pragma once

#include <raylib-cpp.hpp>
#include <string>
#include <memory>

namespace Core 
{
    // A simple identifier string to label the entity in editors
    struct TagComponent 
    {
        std::string Name;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& name)
            : Name(name) {}
    };

    // Geometric structure managing exact physical position inside 3D space
    struct TransformComponent 
    {
        raylib::Vector3 Position{0.0f, 0.0f, 0.0f};
        raylib::Vector3 RotationAxis{0.0f, 1.0f, 0.0f};
        float Angle = 0.0f;
        raylib::Vector3 Scale{1.0f, 1.0f, 1.0f};

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const raylib::Vector3& pos)
            : Position(pos) {}
    };

    // The secure proxy handle to visual meshes protected by the AssetManager
    struct ModelComponent 
    {
        std::shared_ptr<raylib::Model> Model;

        ModelComponent() = default;
        ModelComponent(const ModelComponent&) = default;
        ModelComponent(std::shared_ptr<raylib::Model> model)
            : Model(model) {}
    };

    // Overrides rendering instructions on the pipeline
    struct MeshRendererComponent 
    {
        raylib::Color BaseColor = raylib::Color(255, 255, 255, 255);
        bool bDrawWireframe = false;

        MeshRendererComponent() = default;
        MeshRendererComponent(const MeshRendererComponent&) = default;
        MeshRendererComponent(const raylib::Color& color)
            : BaseColor(color) {}
    };
}

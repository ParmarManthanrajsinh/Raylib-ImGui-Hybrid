#pragma once

#include "entt/entt.hpp"
#include "Core/Base/Core.h"
#include "Core/Logging/Log.h"
#include "Scene.h"

namespace Core 
{
    class FEntity 
    {
    public:
        FEntity() = default;
        FEntity(entt::entity handle, FScene* scene)
            : m_EntityHandle(handle), m_Scene(scene) {}
        FEntity(const FEntity& other) = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            return m_Scene->GetRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->GetRegistry().get<T>(m_EntityHandle);
        }

        template<typename T>
        const T& GetComponent() const
        {
            CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->GetRegistry().get<T>(m_EntityHandle);
        }

        template<typename T>
        bool HasComponent() const
        {
            return m_Scene->GetRegistry().all_of<T>(m_EntityHandle);
        }

        template<typename T>
        void RemoveComponent()
        {
            CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            m_Scene->GetRegistry().remove<T>(m_EntityHandle);
        }

        operator bool() const { return m_EntityHandle != entt::null; }
        operator entt::entity() const { return m_EntityHandle; }
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }

        bool operator==(const FEntity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
        bool operator!=(const FEntity& other) const { return !(*this == other); }

    private:
        entt::entity m_EntityHandle{ entt::null };
        FScene* m_Scene = nullptr;
    };
}

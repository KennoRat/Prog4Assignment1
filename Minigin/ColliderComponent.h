#pragma once
#include "BaseComponent.h"
#include <SDL_rect.h> 
#include <functional> 
#include <string>

namespace dae
{
    class GameObject;

    class ColliderComponent : public BaseComponent
    {
    public:
        using CollisionCallback = std::function<void(GameObject*)>;

        // Constructor
        ColliderComponent(std::shared_ptr<GameObject> owner, float width, float height);

        // Destructor
        virtual ~ColliderComponent() override;

        //Rule Of Five
        ColliderComponent(const ColliderComponent& other) = delete;
        ColliderComponent(ColliderComponent&& other) = delete;
        ColliderComponent& operator=(const ColliderComponent& other) = delete;
        ColliderComponent& operator=(ColliderComponent&& other) = delete;

        virtual void Update() override; 
        virtual void Render() const override;
        virtual void RenderImGui() override {}

        SDL_FRect GetBounds() const;

        void SetOffset(float x, float y) { m_offsetX = x; m_offsetY = y; }
        void SetDimensions(float width, float height) { m_width = width; m_height = height; }

        // Collision Tag
        void SetTag(const std::string& tag) { m_tag = tag; }
        const std::string& GetTag() const { return m_tag; }

        // Register a callback for when a collision occurs
        void SetCollisionCallback(CollisionCallback callback) { m_onCollisionCallback = std::move(callback); }
        void TriggerCollision(GameObject* pOther) { if (m_onCollisionCallback) m_onCollisionCallback(pOther); }


    private:

        SDL_FRect m_bounds{};   
        float m_width;
        float m_height;
        float m_offsetX{ 0.f };  
        float m_offsetY{ 0.f };
        std::string m_tag{ "Default" };

        CollisionCallback m_onCollisionCallback;
    };
}
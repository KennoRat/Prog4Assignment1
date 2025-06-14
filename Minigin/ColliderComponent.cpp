#include "ColliderComponent.h"
#include "GameObject.h"
#include "CollisionManager.h"

namespace dae
{
    ColliderComponent::ColliderComponent(std::shared_ptr<GameObject> owner, float width, float height)
        : BaseComponent(*owner),
        m_width(width), m_height(height)
    {
        CollisionManager::GetInstance().AddCollider(this);
    }

    ColliderComponent::~ColliderComponent()
    {
        CollisionManager::GetInstance().RemoveCollider(this);
    }

    void ColliderComponent::Update()
    {
        // Not used
    }

    SDL_FRect ColliderComponent::GetBounds() const
    {
        if (!GetGameObject()) return { 0,0,0,0 };
        const auto worldPos = GetGameObject()->GetWorldPosition().GetPosition();
        return 
        {
            worldPos.x + m_offsetX,
            worldPos.y + m_offsetY,
            m_width,
            m_height
        };
    }

    void ColliderComponent::Render() const
    {
        // Not used
    }
}
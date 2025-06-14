#include "CollisionManager.h"
#include "ColliderComponent.h"
#include "GameObject.h" // For getting GameObject from ColliderComponent
#include <algorithm>   // For std::remove
#include <iostream>

namespace dae
{
    void CollisionManager::AddCollider(ColliderComponent* pCollider)
    {
        if (pCollider) 
        {
            m_Colliders.push_back(pCollider);
        }
    }

    void CollisionManager::RemoveCollider(ColliderComponent* pCollider)
    {
        m_Colliders.erase(std::remove(m_Colliders.begin(), m_Colliders.end(), pCollider), m_Colliders.end());
    }

    void CollisionManager::ClearColliders()
    {
        m_Colliders.clear();
    }

    bool CollisionManager::DoAABBsOverlap(const SDL_FRect& a, const SDL_FRect& b) const
    {
        return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
    }

    void CollisionManager::CheckCollisions()
    {
        for (int idxCollider = 0; idxCollider < m_Colliders.size(); ++idxCollider)
        {
            ColliderComponent* pColliderA = m_Colliders[idxCollider];
            if (!pColliderA || !pColliderA->GetGameObject()) continue;

            for (int idxOther = idxCollider + 1; idxOther < m_Colliders.size(); ++idxOther)
            {
                ColliderComponent* pColliderB = m_Colliders[idxOther];
                if (!pColliderB || !pColliderB->GetGameObject()) continue;

                SDL_FRect boundsA = pColliderA->GetBounds();
                SDL_FRect boundsB = pColliderB->GetBounds();

                if (DoAABBsOverlap(boundsA, boundsB))
                {
                    pColliderA->TriggerCollision(pColliderB->GetGameObject());
                    pColliderB->TriggerCollision(pColliderA->GetGameObject());
                }
            }
        }
    }
}
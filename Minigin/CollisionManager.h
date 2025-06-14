#pragma once
#include "Singleton.h"
#include <vector>
#include <SDL_rect.h> 

namespace dae           
{
    class ColliderComponent; 
    class Scene;

    class CollisionManager final : public Singleton<CollisionManager>
    {
    public:

        void AddCollider(ColliderComponent* pCollider);
        void RemoveCollider(ColliderComponent* pCollider);
        void ClearColliders();

        void CheckCollisions();

    private:

        friend class Singleton<CollisionManager>;
        CollisionManager() = default;

        std::vector<ColliderComponent*> m_Colliders;

        bool DoAABBsOverlap(const SDL_FRect& a, const SDL_FRect& b) const;
    };
}
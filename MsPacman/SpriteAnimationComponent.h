#pragma once
#include <SDL_rect.h> 
#include <vector>
#include <map>

#include "BaseComponent.h" 
#include "MoveCommand.h"   

namespace dae
{
    class GameObject;      
    class RenderComponent; 
}

class SpriteAnimationComponent final : public dae::BaseComponent
{
public:
    // Constructor
    SpriteAnimationComponent(std::shared_ptr<dae::GameObject> owner, float framesPerSecond,
        int spriteWidth, int spriteHeight, int framesPerDirection = 3);
    // Destructor
    virtual ~SpriteAnimationComponent() override = default;

    virtual void Update() override;
    virtual void Render() const override {}
    virtual void RenderImGui() override {}

    void SetDirection(dae::Direction direction);
    void SetIsMoving(bool isMoving);

private:

    void UpdateSourceRect();

    dae::RenderComponent* m_pRenderComponentCache{ nullptr };
    dae::Direction m_CurrentDirection{ dae::Direction::Right };
    bool m_IsMoving{ false };

    float m_FramesPerSecond;
    float m_AnimationTimer{ 0.f };
    int m_CurrentFrame{ 0 };

    int m_SpriteWidth;
    int m_SpriteHeight;
    int m_FramesPerDirection;

    const int ROW_RIGHT = 0;
    const int ROW_LEFT = 1;
    const int ROW_UP = 2;
    const int ROW_DOWN = 3;
};
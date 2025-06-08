#include "SpriteAnimationComponent.h"
#include <stdexcept>
#include <iostream>

#include "GameObject.h"      
#include "RenderComponent.h" 
#include "TimeGameEngine.h"  
#include "PlayerMovementComponent.h"


SpriteAnimationComponent::SpriteAnimationComponent(std::shared_ptr<dae::GameObject> owner, float framesPerSecond,
    int spriteWidth, int spriteHeight, int framesPerDirection)
    : dae::BaseComponent(*owner),
    m_FramesPerSecond(framesPerSecond),
    m_SpriteWidth(spriteWidth),
    m_SpriteHeight(spriteHeight),
    m_FramesPerDirection(framesPerDirection)
{

    m_pRenderComponentCache = owner->GetComponent<dae::RenderComponent>();
    if (!m_pRenderComponentCache) 
    {
        throw std::runtime_error("SpriteAnimationComponent requires the GameObject to have a RenderComponent!");
    }

    if (m_pRenderComponentCache->GetSourceRect().w == 0 && m_pRenderComponentCache->GetSourceRect().h == 0) 
    {
        m_pRenderComponentCache->SetRenderDimensions(static_cast<float>(m_SpriteWidth), static_cast<float>(m_SpriteHeight));
    }

    SetDirection(m_CurrentDirection); 
    UpdateSourceRect();
}

void SpriteAnimationComponent::Update()
{
    if (!m_pRenderComponentCache) return;

    auto* moveComponent = GetGameObject()->GetComponent<PlayerMovementComponent>();
    if (moveComponent) 
    {
        dae::Direction actualDir = moveComponent->GetCurrentDirection(); 
        bool isActuallyMoving = moveComponent->IsCurrentlyMoving();

        if (m_CurrentDirection != actualDir) 
        {
            SetDirection(actualDir);
        }

        SetIsMoving(isActuallyMoving);
    }


    float frameTime = 1.0f / m_FramesPerSecond;
    m_AnimationTimer += static_cast<float>(dae::Time::GetInstance().GetDeltaTime());

    bool needsRectUpdate = false;
    if (m_AnimationTimer >= frameTime)
    {
        m_AnimationTimer -= frameTime;
        if (m_IsMoving) 
        {
            m_CurrentFrame = (m_CurrentFrame + 1) % m_FramesPerDirection;
            needsRectUpdate = true;
        }
    }

    if (needsRectUpdate) 
    {
        UpdateSourceRect();
    }
}


void SpriteAnimationComponent::SetDirection(dae::Direction direction)
{
    if (m_CurrentDirection != direction || m_CurrentFrame == -1) 
    { 
        m_CurrentDirection = direction;
        m_CurrentFrame = 0; 
        m_AnimationTimer = 0.f;
        UpdateSourceRect();
    }
}

void SpriteAnimationComponent::SetIsMoving(bool isMoving)
{
    if (m_IsMoving != isMoving) 
    {
        m_IsMoving = isMoving;
        m_AnimationTimer = 0.f; // Reset timer

        UpdateSourceRect();
    }
}


void SpriteAnimationComponent::UpdateSourceRect()
{
    if (!m_pRenderComponentCache) return;

    int row = 0;
    switch (m_CurrentDirection)
    {
    case dae::Direction::Right:
        row = ROW_RIGHT; 
        break;
    case dae::Direction::Left:
        row = ROW_LEFT; 
        break;
    case dae::Direction::Up:    
        row = ROW_UP;    
        break;
    case dae::Direction::Down:  
        row = ROW_DOWN;  
        break;
    default:                    
        row = ROW_RIGHT;
        break; 
    }

    SDL_Rect srcRect;
    srcRect.x = m_CurrentFrame * m_SpriteWidth;
    srcRect.y = row * m_SpriteHeight;
    srcRect.w = m_SpriteWidth;
    srcRect.h = m_SpriteHeight;

    m_pRenderComponentCache->SetSourceRect(srcRect);
}
#include "GhostSpriteAnimationComponent.h"
#include <stdexcept>
#include <iostream>

#include "GameObject.h"
#include "RenderComponent.h"
#include "Texture2D.h"
#include "ResourceManager.h"
#include "TimeGameEngine.h"
#include "BaseGhostComponent.h"

GhostSpriteAnimationComponent::GhostSpriteAnimationComponent
(
    std::shared_ptr<dae::GameObject> owner,
    const std::string& normalSheetPath,
    const std::string& frightenedSheetPath,
    const std::string& eatenSheetPath,
    float defaultAnimFps,
    int normalSpriteWidth, int normalSpriteHeight,
    int framesPerNormalDirection,
    int frightenedAnimFrames)
    : dae::BaseComponent(*owner),
    m_AnimFps(defaultAnimFps),
    m_NormalSpriteWidth(normalSpriteWidth),
    m_NormalSpriteHeight(normalSpriteHeight),
    m_FramesPerNormalDirection(framesPerNormalDirection),
    m_FrightenedAnimFrames(frightenedAnimFrames)
{
    m_pRenderComponentCache = owner->GetComponent<dae::RenderComponent>();
    if (!m_pRenderComponentCache)
    {
        throw std::runtime_error("SpriteAnimationComponent requires the GameObject to have a RenderComponent!");
    }

    if (m_pRenderComponentCache->GetSourceRect().w == 0 && m_pRenderComponentCache->GetSourceRect().h == 0)
    {
        m_pRenderComponentCache->SetRenderDimensions(static_cast<float>(m_NormalSpriteWidth), static_cast<float>(m_NormalSpriteHeight));
    }

    m_pGhostLogicComponentCache = owner->GetComponent<BaseGhostComponent>();
    if (!m_pGhostLogicComponentCache)
    {
        throw std::runtime_error("SpriteAnimationComponent requires the GameObject to have a BaseGhostComponent!");
    }

    LoadTextures(normalSheetPath, frightenedSheetPath, eatenSheetPath);
    SetVisualState(GhostState::Scatter);
}

void GhostSpriteAnimationComponent::LoadTextures(const std::string& normalP, const std::string& frightenedP, const std::string& eatenP)
{
    auto& rm = dae::ResourceManager::GetInstance();
    m_pNormalTexture = rm.LoadTexture(normalP);
    m_pFrightenedTexture = rm.LoadTexture(frightenedP);
    m_pEatenTexture = rm.LoadTexture(eatenP);

    if (!m_pNormalTexture) std::cerr << "GhostSpriteAnim Error: Failed to load normal texture: " << normalP << std::endl;
    if (!m_pFrightenedTexture) std::cerr << "GhostSpriteAnim Error: Failed to load frightened texture: " << frightenedP << std::endl;
    if (!m_pEatenTexture) std::cerr << "GhostSpriteAnim Error: Failed to load eaten texture: " << eatenP << std::endl;
}

void GhostSpriteAnimationComponent::ApplyCurrentVisualStateToRenderer()
{
    if (!m_pRenderComponentCache) return;

    m_CurrentFrame = 0;
    m_AnimationTimer = 0.f;

    switch (m_CurrentVisualState)
    {
    case GhostState::Scatter:
    case GhostState::Chase:
    case GhostState::ExitingPen:
    case GhostState::Idle:

        m_pRenderComponentCache->SetTexture(m_pNormalTexture);
        m_CurrentFramesPerAnimCycle = m_FramesPerNormalDirection;
        m_AnimFps = 10.0f;

        break;
    case GhostState::Frightened:

        m_pRenderComponentCache->SetTexture(m_pFrightenedTexture);
        m_CurrentFramesPerAnimCycle = m_FrightenedAnimFrames;
        m_AnimFps = 6.0f;
        m_IsFlashing = false;

        break;
    case GhostState::Eaten:

        m_pRenderComponentCache->SetTexture(m_pEatenTexture);
        m_CurrentFramesPerAnimCycle = m_FramesPerNormalDirection;
        m_AnimFps = 10.0f;

        break;
    }

    m_pRenderComponentCache->SetRenderDimensions(static_cast<float>(m_NormalSpriteWidth), static_cast<float>(m_NormalSpriteHeight));
    UpdateSourceRect();
}

void GhostSpriteAnimationComponent::SetVisualState(GhostState ghostState)
{
    if (m_CurrentVisualState == ghostState)
    {
        if (ghostState == GhostState::Frightened && m_CurrentVisualState == GhostState::Frightened)
        {
            m_AnimFps = 6.0f;
        }
        return;
    }

    m_CurrentVisualState = ghostState;
    ApplyCurrentVisualStateToRenderer();
}

void GhostSpriteAnimationComponent::SetMovementDirection(dae::Direction direction)
{
    if (m_CurrentMovementDirection != direction)
    {
        m_CurrentMovementDirection = direction;

        if (m_CurrentVisualState != GhostState::Frightened)
        { 
            m_CurrentFrame = 0;
            m_AnimationTimer = 0.f;
        }

        UpdateSourceRect();
    }
}

void GhostSpriteAnimationComponent::SetIsMoving(bool isMoving)
{
    if (m_IsActuallyMoving != isMoving)
    {
        m_IsActuallyMoving = isMoving;

        if (!m_IsActuallyMoving)
        {
            m_CurrentFrame = 0;
            m_AnimationTimer = 0.f;
            UpdateSourceRect();
        }
    }
}

void GhostSpriteAnimationComponent::Update()
{
    if (!m_pRenderComponentCache) return;

    float frameTime = 1.0f / m_AnimFps;
    m_AnimationTimer += static_cast<float>(dae::Time::GetInstance().GetDeltaTime());

    bool needsRectUpdate = false;

    if (m_AnimationTimer >= frameTime)
    {
        m_AnimationTimer -= frameTime;
        needsRectUpdate = true;

        if (m_CurrentVisualState == GhostState::Frightened)
        {
            float frightenTimeLeft = 0.f;

            if (m_pGhostLogicComponentCache)
            {
                frightenTimeLeft = m_pGhostLogicComponentCache->GetFrightenTimeLeft();
            }

            if (frightenTimeLeft > 0.f && frightenTimeLeft <= FRIGHTEN_FLASH_START_TIME)
            {
                m_IsFlashing = true;
                m_CurrentFrame = (m_CurrentFrame + 1) % m_FrightenedAnimFrames;
            }
            else
            {
                m_IsFlashing = false;
                m_CurrentFrame = (m_CurrentFrame + 1) % m_FramesPerNormalDirection;
            }
        }
        else
        { 
            if (m_CurrentFramesPerAnimCycle > 1)
            {
                m_CurrentFrame = (m_CurrentFrame + 1) % m_CurrentFramesPerAnimCycle;
            }
            else
            {
                m_CurrentFrame = 0;
            }
        }
    }

    if (needsRectUpdate)
    {
        UpdateSourceRect();
    }
}

void GhostSpriteAnimationComponent::UpdateSourceRect()
{
    if (!m_pRenderComponentCache) return;

    SDL_Rect srcRect;
    srcRect.w = m_NormalSpriteWidth;
    srcRect.h = m_NormalSpriteHeight;

    int frameToUse = m_CurrentFrame;

    int row = 0;

    switch (m_CurrentMovementDirection)
    {
    case dae::Direction::Right: row = ROW_RIGHT_NORMAL; break;
    case dae::Direction::Left:  row = ROW_LEFT_NORMAL;  break;
    case dae::Direction::Up:    row = ROW_UP_NORMAL;    break;
    case dae::Direction::Down:  row = ROW_DOWN_NORMAL;  break;
    default: row = ROW_RIGHT_NORMAL; break;
    }

    switch (m_CurrentVisualState)
    {
    case GhostState::Scatter:
    case GhostState::Chase:
    case GhostState::ExitingPen:
    case GhostState::Idle:
    {
        srcRect.x = frameToUse * m_NormalSpriteWidth;
        srcRect.y = row * m_NormalSpriteHeight;

        break;
    }
    case GhostState::Frightened:
    {
        srcRect.x = frameToUse * m_NormalSpriteWidth;
        srcRect.y = 0;
        break;
    }
    case GhostState::Eaten:
    {
        srcRect.x = 0;
        srcRect.y = row * m_NormalSpriteHeight;
        break;
    }
    }

    m_pRenderComponentCache->SetSourceRect(srcRect);
}
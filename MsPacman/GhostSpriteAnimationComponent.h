#pragma once
#include "BaseComponent.h"    
#include "MoveCommand.h"      
#include "GhostState.h"       
#include <SDL_rect.h>
#include <string>
#include <memory>            

namespace dae
{
    class GameObject;
    class RenderComponent;
    class Texture2D;
}

class BaseGhostComponent;

class GhostSpriteAnimationComponent final : public dae::BaseComponent
{
public:
    // Constructor
    GhostSpriteAnimationComponent
    (
        std::shared_ptr<dae::GameObject> owner,
        const std::string& normalSheetPath,   
        const std::string& frightenedSheetPath,
        const std::string& eatenSheetPath,
        float defaultAnimFps,
        int normalSpriteWidth, int normalSpriteHeight,
        int framesPerNormalDirection = 2,
        int frightenedAnimFrames = 2
    );

    // Destructor
    virtual ~GhostSpriteAnimationComponent() override = default;

    // Rule of Five
    GhostSpriteAnimationComponent(const GhostSpriteAnimationComponent&) = delete;
    GhostSpriteAnimationComponent(GhostSpriteAnimationComponent&&) = delete;
    GhostSpriteAnimationComponent& operator=(const GhostSpriteAnimationComponent&) = delete;
    GhostSpriteAnimationComponent& operator=(GhostSpriteAnimationComponent&&) = delete;

    virtual void Update() override;
    virtual void Render() const override {}
    virtual void RenderImGui() override {}

    // Called by BaseGhostComponent
    void SetVisualState(GhostState ghostState);
    void SetMovementDirection(dae::Direction direction);
    void SetIsMoving(bool isMoving);

private:

    void LoadTextures(const std::string& normalP, const std::string& frightenedP, const std::string& eatenP);
    void UpdateSourceRect();
    void ApplyCurrentVisualStateToRenderer();

    dae::RenderComponent* m_pRenderComponentCache{ nullptr };
    BaseGhostComponent* m_pGhostLogicComponentCache{ nullptr };

    // Textures
    std::shared_ptr<dae::Texture2D> m_pNormalTexture;
    std::shared_ptr<dae::Texture2D> m_pFrightenedTexture;
    std::shared_ptr<dae::Texture2D> m_pEatenTexture;


    GhostState m_CurrentVisualState{ GhostState::Scatter };
    dae::Direction m_CurrentMovementDirection{ dae::Direction::Left };
    bool m_IsActuallyMoving{ true };

    // Animation timing
    float m_AnimFps;
    float m_AnimationTimer{ 0.f };
    int m_CurrentFrame{ 0 };

    int m_CurrentFramesPerAnimCycle;

    int m_NormalSpriteWidth;
    int m_NormalSpriteHeight;
    int m_FramesPerNormalDirection;
    int m_FrightenedAnimFrames;
    static constexpr float FRIGHTEN_FLASH_START_TIME = 3.5f;
    bool m_IsFlashing{ false };

    // Row definitions
    const int ROW_RIGHT_NORMAL = 0;
    const int ROW_LEFT_NORMAL = 1;
    const int ROW_UP_NORMAL = 2;
    const int ROW_DOWN_NORMAL = 3;
};
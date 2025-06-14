#pragma once
#include "BaseComponent.h"
#include "Observer.h"
#include "LevelGridComponent.h" 
#include "MoveCommand.h"
#include "GhostState.h"
#include <glm.hpp>
#include <memory>

// Forward declarations
namespace dae 
{
    class GameObject;
    class RenderComponent;
}
class PlayerMovementComponent;
class GhostSpriteAnimationComponent;
class IGhostChaseBehaviour;

class BaseGhostComponent final : public dae::BaseComponent, public dae::Observer
{
public:
    // Constructor
    BaseGhostComponent(std::shared_ptr<dae::GameObject> owner,
        std::shared_ptr<dae::GameObject> levelObject,
        std::shared_ptr<dae::GameObject> pacmanObject,
        std::unique_ptr<IGhostChaseBehaviour> chaseBehaviour,
        float normalSpeed, float frightenedSpeed, float eatenSpeed,
        int homeCornerRow, int homeCornerCol);

    // Destructor
    virtual ~BaseGhostComponent() override;

    // Rule of Five 
    BaseGhostComponent(const BaseGhostComponent&) = delete;
    BaseGhostComponent(BaseGhostComponent&&) = delete;
    BaseGhostComponent& operator=(const BaseGhostComponent&) = delete;
    BaseGhostComponent& operator=(BaseGhostComponent&&) = delete;

    virtual void Initialize();
    virtual void Update() override;
    virtual void Render() const override {}
    virtual void RenderImGui() override;
    virtual void Notify(const dae::GameObject& gameObject, const dae::Event& event) override;

    // State Management
    GhostState GetCurrentState() const { return m_currentState; }
    void SetState(GhostState newState);

    // Public methods
    void Frighten();
    void Unfrighten();
    void WasEaten();
    void SetNextDirection(dae::Direction direction);
    void SpawnAt(int gridRow, int gridCol, GhostState initialState = GhostState::Scatter, dae::Direction initialDir = dae::Direction::Left);
    void Activate() { m_isActiveInPen = true; m_stateTimer = 0.f; }
    void Deactivate() { m_isActiveInPen = false; }

    // Public Getters needed by Chase Behaviours
    std::shared_ptr<dae::GameObject> GetPacmanObject() const { return m_pPacmanObject; }
    LevelGridComponent* GetLevelGrid() const { return m_pLevelGridCache; }
    PlayerMovementComponent* GetPacmanMovement() const { return m_pPacmanMovementCache; }
    glm::ivec2 GetCurrentGridPosition() const;
    dae::Direction GetCurrentDirection() const { return m_currentDir; }
    glm::ivec2 GetScatterTarget() const { return m_homeCornerGridPos; }
    glm::ivec2 GetPenExitTarget() const { return PEN_EXIT_TARGET; }
    glm::ivec2 GetEatenTarget() const;
    float GetFrightenTimeLeft() const { return m_frightenTimer; }

private:
    // Private methods
    void UpdateMovement(float deltaTime);
    dae::Direction ChooseDirectionAtIntersection(const glm::ivec2& currentGridPos, dae::Direction currentDir);
    bool CanMoveTo(const glm::ivec2& gridPos, dae::Direction dir) const;
    glm::ivec2 WorldToGrid(const glm::vec3& worldPos) const;
    glm::vec2 GridToWorldCenter(const glm::ivec2& gridPos) const; 
    glm::vec2 AdjustPositionForSpriteCenter(glm::vec2 targetCenterPos) const;

    void UpdateStateTimers(float deltaTime);
    void HandleStateTransitions();

    // Data
    std::unique_ptr<IGhostChaseBehaviour> m_pChaseBehaviour;
    std::shared_ptr<dae::GameObject> m_pLevelObject;
    std::shared_ptr<dae::GameObject> m_pPacmanObject;

    LevelGridComponent* m_pLevelGridCache{ nullptr };
    PlayerMovementComponent* m_pPacmanMovementCache{ nullptr };
    dae::RenderComponent* m_pRenderComponentCache{ nullptr };
    GhostSpriteAnimationComponent* m_pSpriteAnimationComponentCache{ nullptr };

    GhostState m_currentState{ GhostState::Idle };
    GhostState m_previousState{ GhostState::Idle };

    // Movement
    float m_currentSpeed{ 0.f };
    float m_normalSpeed;
    float m_frightenedSpeed;
    float m_eatenSpeed;

    glm::vec2 m_targetWorldPos{};
    dae::Direction m_currentDir{ dae::Direction::Left };
    dae::Direction m_nextDir{};

    bool m_isActive{ true };
    bool m_isMoving{ false };
    bool m_justReversed{ false };
    bool m_isActiveInPen{ false };

    // Targeting
    glm::ivec2 m_homeCornerGridPos;
    glm::ivec2 m_currentTargetGridPos{};
    const glm::ivec2 PEN_EXIT_TARGET{ 13, 11 };

    // Timers
    float m_stateTimer{ 0.f };
    float m_frightenTimer{ 0.f };
    static constexpr float SCATTER_DURATION = 7.0f;
    static constexpr float CHASE_DURATION = 20.0f;
    static constexpr float FRIGHTEN_DURATION = 8.0f;
    static constexpr float IDLE_DURATION_IN_PEN = 5.0f;

    // Sprite Info
    float m_spriteRenderWidth{ 0.f };
    float m_spriteRenderHeight{ 0.f };

    bool m_isInitialized{ false };
    const float m_tileCenterTolerance{ 1.5f };
};
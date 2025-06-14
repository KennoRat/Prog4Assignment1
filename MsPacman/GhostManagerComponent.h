#pragma once
#include "BaseComponent.h" 
#include "GhostState.h"
#include "MoveCommand.h"
#include <vector>
#include <memory>
#include <string>


// Forward declarations
namespace dae 
{
    class GameObject;
    class Scene;
}
class BaseGhostComponent;
class IGhostChaseBehaviour;
class PlayerMovementComponent;

struct GhostSpawnData 
{
    std::string nameTag;
    std::string normalTexturePath;
    std::unique_ptr<IGhostChaseBehaviour> chaseBehaviour;
    int scatterCornerCol;
    int scatterCornerRow;
    int startPenCol;
    int startPenRow;
    GhostState initialSpawnState;
    dae::Direction initialSpawnDirection;
    bool startsActive; 
};

class GhostManagerComponent final : public dae::BaseComponent 
{
public:
    // Constructor
    GhostManagerComponent(std::shared_ptr<dae::GameObject> owner,
        std::shared_ptr<dae::GameObject> levelObject,
        std::shared_ptr<dae::GameObject> pacmanObject,
        dae::Scene* pOwningScene);
    // Destructor
    virtual ~GhostManagerComponent() override = default;
    // Rule Of Five
    GhostManagerComponent(const GhostManagerComponent&) = delete;
    GhostManagerComponent(GhostManagerComponent&&) = delete;
    GhostManagerComponent& operator=(const GhostManagerComponent&) = delete;
    GhostManagerComponent& operator=(GhostManagerComponent&&) = delete;

    virtual void Initialize(); 
    virtual void Update() override;
    virtual void Render() const override {}
    virtual void RenderImGui() override {}

    void ResetGhostsForNewLife();
    void ResetGhostsForNewLevel();
    void ClearGhostsFromScene();

    dae::GameObject* GetGhostByTag(const std::string& tag) const;

private:

    void CreateGhost(const GhostSpawnData& data);
    void SetupGhostReleaseTimers();

    std::shared_ptr<dae::GameObject> m_pLevelObject;
    std::shared_ptr<dae::GameObject> m_pPacmanObject;
    dae::Scene* m_pOwningScene;

    std::vector<std::shared_ptr<dae::GameObject>> m_Ghosts;
    std::vector<BaseGhostComponent*> m_GhostLogicComponents;

    // Ghost Release Logic
    struct GhostReleaseInfo 
    {
        BaseGhostComponent* ghostComp = nullptr;
        float releaseTimer = 0.f;
        bool hasBeenReleased = false;
        bool startsActiveOutsidePen = false;
    };

    std::vector<GhostReleaseInfo> m_GhostReleaseSchedule;
    int m_NextGhostToReleaseIndex = 0;

    // Common ghost properties
    float m_DefaultNormalSpeed = 75.f;
    float m_DefaultFrightenedSpeed = 50.f;
    float m_DefaultEatenSpeed = 150.f;
    float m_DefaultGhostColliderSizeFactor = 0.7f; 

    // Sprite animation common params
    float m_DefaultSpriteFps = 8.f;
    int m_DefaultSpriteWidth = 15;
    int m_DefaultSpriteHeight = 15;
    int m_DefaultFramesPerDir = 2;
    int m_DefaultFramesFrightened = 4;
    std::string m_FrightenedSpritePath = "ScaredGhost.png";
    std::string m_EatenSpritePath = "EatenGhost.png";

    bool m_IsInitialized{ false };
};
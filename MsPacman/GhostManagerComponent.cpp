#include "GhostManagerComponent.h"
#include "GameObject.h"      
#include "Scene.h"
#include "RenderComponent.h"
#include "ColliderComponent.h"
#include "TimeGameEngine.h"
#include "BaseGhostComponent.h"
#include "GhostSpriteAnimationComponent.h"
#include "BlinkyChaseBehaviour.h"
#include "PinkyChaseBehaviour.h"
#include "InkyChaseBehaviour.h"
#include "SueChaseBehaviour.h"
#include "PlayerMovementComponent.h"
#include "LivesComponent.h"
#include "ScoreComponent.h"
#include <iostream>

GhostManagerComponent::GhostManagerComponent(std::shared_ptr<dae::GameObject> owner,
    std::shared_ptr<dae::GameObject> levelObject,
    std::shared_ptr<dae::GameObject> pacmanObject,
    dae::Scene* pOwningScene)
    : dae::BaseComponent(*owner),
    m_pLevelObject(levelObject),
    m_pPacmanObject(pacmanObject),
    m_pOwningScene(pOwningScene)
{
    if (!owner) throw std::runtime_error("GhostManagerComponent owner is null");
    if (!m_pLevelObject) throw std::runtime_error("GhostManagerComponent requires a level object.");
    if (!m_pPacmanObject) throw std::runtime_error("GhostManagerComponent requires a Pac-Man object.");
    if (!m_pOwningScene) throw std::runtime_error("GhostManagerComponent requires an owning scene.");
}

void GhostManagerComponent::Initialize()
{
    if (m_IsInitialized) return;
    std::cout << "GhostManagerComponent Initializing..." << std::endl;

    std::vector<GhostSpawnData> allGhostData;

    // Blinky Data
    allGhostData.push_back(
    {
    "Blinky", "GhostRed.png", std::make_unique<BlinkyChaseBehaviour>(),
    1, 29,  // Scatter: Col 1, Row 29
    13, 10, // Start Pen: Col 13, Row 10
    GhostState::Scatter, dae::Direction::Left, true
    });

    // Pinky Data
    allGhostData.push_back(
    {
    "Pinky", "GhostPink.png", std::make_unique<PinkyChaseBehaviour>(),
    2, 0,   // Scatter: Col 2, Row 0 
    13, 14, // Start Pen: Col 13, Row 14
    GhostState::Idle, dae::Direction::Up, false
    });

    // Inky Data
    allGhostData.push_back(
    {
    "Inky", "GhostBlue.png", {},
    2, 0,   // Scatter: Col 2, Row 0 
    11, 14, // Start Pen: Col 11, Row 14
    GhostState::Idle, dae::Direction::Up, false
    });

    // Sue Data
    allGhostData.push_back({
    "Sue", "GhostOrange.png", std::make_unique<SueChaseBehaviour>(),
    1, 29,  // Scatter: Col 1, Row 29
    15, 14, // Start: Col 15, Row 14
    GhostState::Idle, dae::Direction::Up, false });

    // Create all ghosts
    for (const auto& data : allGhostData)
    {
        CreateGhost(data);
    }

    SetupGhostReleaseTimers();

    m_IsInitialized = true;
    std::cout << "GhostManagerComponent Initialized with " << m_Ghosts.size() << " ghosts." << std::endl;
}


void GhostManagerComponent::CreateGhost(const GhostSpawnData& data)
{
    if (!m_pOwningScene || !m_pLevelObject || !m_pPacmanObject)
    {
        std::cerr << "GhostManager Error: Missing critical objects for ghost creation." << std::endl;
        return;
    }

    float tileSpawnSize = 20.f;

    auto ghostObject = std::make_shared<dae::GameObject>();
    ghostObject->SetTag(data.nameTag);

    // RenderComponent
    auto renderComp = std::make_unique<dae::RenderComponent>(ghostObject);
    ghostObject->AddComponent(std::move(renderComp));

    // BaseGhostComponent
    std::unique_ptr<IGhostChaseBehaviour> chaseBehaviorInstance;
    if (data.nameTag == "Blinky") chaseBehaviorInstance = std::make_unique<BlinkyChaseBehaviour>();
    else if (data.nameTag == "Pinky") chaseBehaviorInstance = std::make_unique<PinkyChaseBehaviour>();
    else if (data.nameTag == "Inky")
    {
        chaseBehaviorInstance = std::make_unique<InkyChaseBehaviour>
        (
        [this]() { return this->GetGhostByTag("Blinky"); }
        );
    }
    else if (data.nameTag == "Sue")
    {
        chaseBehaviorInstance = std::make_unique<SueChaseBehaviour>();
    }
    else chaseBehaviorInstance = std::make_unique<BlinkyChaseBehaviour>();

    auto ghostLogicComp = std::make_unique<BaseGhostComponent>
        (
            ghostObject, m_pLevelObject, m_pPacmanObject,
            std::move(chaseBehaviorInstance),
            m_DefaultNormalSpeed, m_DefaultFrightenedSpeed, m_DefaultEatenSpeed,
            data.scatterCornerRow, data.scatterCornerCol // ROW, COL
        );

    BaseGhostComponent* pGhostLogicRaw = ghostLogicComp.get();
    ghostObject->AddComponent(std::move(ghostLogicComp));

    // GhostSpriteAnimationComponent
    auto animComp = std::make_unique<GhostSpriteAnimationComponent>
    (
        ghostObject,
        data.normalTexturePath,
        m_FrightenedSpritePath,
        m_EatenSpritePath,
        m_DefaultSpriteFps,
        m_DefaultSpriteWidth, m_DefaultSpriteHeight,    
        m_DefaultFramesPerDir,
        m_DefaultFramesFrightened
    );
    ghostObject->AddComponent(std::move(animComp));

    // ColliderComponent
    float colliderSize = tileSpawnSize * m_DefaultGhostColliderSizeFactor;
    auto ghostCollider = std::make_unique<dae::ColliderComponent>(ghostObject, colliderSize, colliderSize);
    ghostCollider->SetTag("Ghost");

    ghostCollider->SetCollisionCallback(
        [gObj = ghostObject.get(), pacman = m_pPacmanObject.get()](dae::GameObject* pOther)
        {
            if (pOther == pacman)
            {
                auto logic = gObj->GetComponent<BaseGhostComponent>();
                if (logic && logic->GetCurrentState() == GhostState::Frightened)
                {
                    logic->WasEaten();
                    // TODO: Pacman scores points
                    auto scoreComp = pacman->GetComponent<dae::ScoreComponent>();
                    if (scoreComp) scoreComp->GainPoints(200);
                }
            }
        });
    ghostObject->AddComponent(std::move(ghostCollider));

    // Add to scene
    m_pOwningScene->Add(ghostObject);
    m_Ghosts.push_back(ghostObject);
    if (pGhostLogicRaw)
    {
        m_GhostLogicComponents.push_back(pGhostLogicRaw);
        pGhostLogicRaw->Initialize();
        pGhostLogicRaw->SpawnAt(data.startPenRow, data.startPenCol, data.initialSpawnState, data.initialSpawnDirection);
        if (data.startsActive)
        {
            pGhostLogicRaw->Activate();
        }
    }

    std::cout << "Created ghost: " << data.nameTag << std::endl;
}


void GhostManagerComponent::SetupGhostReleaseTimers()
{
    m_GhostReleaseSchedule.clear();
    m_NextGhostToReleaseIndex = 0;
    float cumulativeDelay = 0.f;
    float delayBetweenGhosts = 5.0f;

    for (BaseGhostComponent* ghostComp : m_GhostLogicComponents)
    {
        if (ghostComp)
        {
            bool startsActive = false; 

            if (ghostComp->GetGameObject()->GetTag() == "Blinky") // Blinky is special
            {
                startsActive = true;
            }

            if (!startsActive)
            { 
                m_GhostReleaseSchedule.push_back({ ghostComp, cumulativeDelay, false, false });
                cumulativeDelay += delayBetweenGhosts;
            }
            else
            {
                m_GhostReleaseSchedule.push_back({ ghostComp, 0.f, true, true });
            }
        }
    }
}


void GhostManagerComponent::Update()
{
    if (!m_IsInitialized)
    {
        Initialize();
    }

    float deltaTime = static_cast<float>(dae::Time::GetInstance().GetDeltaTime());

    // Ghost Release Logic
    if (m_NextGhostToReleaseIndex < static_cast<int>(m_GhostReleaseSchedule.size()))
    {
        GhostReleaseInfo& nextGhostInfo = m_GhostReleaseSchedule[m_NextGhostToReleaseIndex];

        if (!nextGhostInfo.hasBeenReleased && !nextGhostInfo.startsActiveOutsidePen)
        {
            nextGhostInfo.releaseTimer -= deltaTime;

            if (nextGhostInfo.releaseTimer <= 0.f)
            {
                if (nextGhostInfo.ghostComp)
                {
                    nextGhostInfo.ghostComp->Activate();
                }

                nextGhostInfo.hasBeenReleased = true;
                m_NextGhostToReleaseIndex++;
            }
        }
        else if (nextGhostInfo.startsActiveOutsidePen && !nextGhostInfo.hasBeenReleased)
        {
            nextGhostInfo.hasBeenReleased = true;
            m_NextGhostToReleaseIndex++;
        }
        else if (nextGhostInfo.hasBeenReleased)
        {
            m_NextGhostToReleaseIndex++;
        }
    }
}

void GhostManagerComponent::ResetGhostsForNewLife()
{
    m_NextGhostToReleaseIndex = 0;

    for (size_t i = 0; i < m_GhostLogicComponents.size(); ++i)
    {
        BaseGhostComponent* ghostLogic = m_GhostLogicComponents[i];
        if (ghostLogic)
        {
            std::string tag = ghostLogic->GetGameObject()->GetTag();
            if (tag == "Blinky")
            {
                ghostLogic->SpawnAt(10, 13, GhostState::ExitingPen, dae::Direction::Left);
                ghostLogic->Activate();
                if (i < m_GhostReleaseSchedule.size())
                {
                    m_GhostReleaseSchedule[i].hasBeenReleased = true;
                    m_GhostReleaseSchedule[i].releaseTimer = 0.f;
                }
            }
            else if (tag == "Pinky")
            {
                ghostLogic->SpawnAt(14, 13, GhostState::Idle, dae::Direction::Up);
                ghostLogic->Deactivate();
                if (i < m_GhostReleaseSchedule.size())
                {
                    m_GhostReleaseSchedule[i].hasBeenReleased = false;
                    m_GhostReleaseSchedule[i].releaseTimer = 0.0f;
                }
            }
            else if (tag == "Inky")
            {
                ghostLogic->SpawnAt(14, 11, GhostState::Idle, dae::Direction::Up);
                ghostLogic->Deactivate();
                if (i < m_GhostReleaseSchedule.size())
                {
                    m_GhostReleaseSchedule[i].hasBeenReleased = false;
                    m_GhostReleaseSchedule[i].releaseTimer = 4.0f;
                }
            }
            else if (tag == "Sue")
            {
                ghostLogic->SpawnAt(14, 15, GhostState::Idle, dae::Direction::Up);
                ghostLogic->Deactivate();
                if (i < m_GhostReleaseSchedule.size())
                {
                    m_GhostReleaseSchedule[i].hasBeenReleased = false;
                    m_GhostReleaseSchedule[i].releaseTimer = 4.0f;
                }
            }
        }
    }

    SetupGhostReleaseTimers();
}

void GhostManagerComponent::ResetGhostsForNewLevel()
{
    std::cout << "GhostManager: Resetting ghosts for new level." << std::endl;
    ResetGhostsForNewLife();
}

void GhostManagerComponent::ClearGhostsFromScene()
{
    if (m_pOwningScene)
    {
        for (auto& ghostGO : m_Ghosts)
        {
            if (ghostGO)
            {
                m_pOwningScene->Remove(ghostGO);
            }
        }
    }

    m_Ghosts.clear();
    m_GhostLogicComponents.clear();
    m_GhostReleaseSchedule.clear();
    m_NextGhostToReleaseIndex = 0;
}

dae::GameObject* GhostManagerComponent::GetGhostByTag(const std::string& tag) const
{
    for (const auto& ghostGO : m_Ghosts)
    {
        if (ghostGO && ghostGO->GetTag() == tag)
        {
            return ghostGO.get();
        }
    }
    return nullptr;
}
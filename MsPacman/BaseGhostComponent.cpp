#include "BaseGhostComponent.h"
#include <stdexcept>
#include <iostream>
#include <algorithm> 
#include <random> 
#include <imgui.h>

// Engine
#include "GameObject.h"          
#include "RenderComponent.h"      
#include "TimeGameEngine.h"

// Game
#include "PlayerMovementComponent.h"
#include "LevelGridComponent.h"   
#include "IGhostChaseBehaviour.h"
#include "SpriteAnimationComponent.h"

dae::Direction GetOppositeDirection(dae::Direction dir);
glm::ivec2 DirectionToVec2(dae::Direction dir);

BaseGhostComponent::BaseGhostComponent(std::shared_ptr<dae::GameObject> owner,
    std::shared_ptr<dae::GameObject> levelObject,
    std::shared_ptr<dae::GameObject> pacmanObject,
    std::unique_ptr<IGhostChaseBehaviour> chaseBehaviour,
    float normalSpeed, float frightenedSpeed, float eatenSpeed,
    int homeCornerRow, int homeCornerCol)
    : dae::BaseComponent(*owner),
    m_pLevelObject(levelObject),
    m_pPacmanObject(pacmanObject),
    m_pChaseBehaviour(std::move(chaseBehaviour)),
    m_normalSpeed(normalSpeed),
    m_frightenedSpeed(frightenedSpeed),
    m_eatenSpeed(eatenSpeed),
    m_homeCornerGridPos(homeCornerCol, homeCornerRow)
{
    if (!m_pLevelObject) throw std::runtime_error("BaseGhostComponent requires a level object.");
    if (!m_pPacmanObject) throw std::runtime_error("BaseGhostComponent requires a Pac-Man object.");
    if (!m_pChaseBehaviour) throw std::runtime_error("BaseGhostComponent requires a chase behaviour.");

    m_currentSpeed = m_normalSpeed;
}

void BaseGhostComponent::Initialize()
{
    if (m_isInitialized) return;

    m_pLevelGridCache = m_pLevelObject->GetComponent<LevelGridComponent>();
    if (!m_pLevelGridCache) 
    {
        throw std::runtime_error("Ghost's Level GameObject is missing LevelGridComponent.");
    }

    m_pPacmanMovementCache = m_pPacmanObject->GetComponent<PlayerMovementComponent>();
    if (!m_pPacmanMovementCache) 
    {
        throw std::runtime_error("Ghost's Pacman GameObject is missing PlayerMovementComponent.");
    }

    m_pRenderComponentCache = GetGameObject()->GetComponent<dae::RenderComponent>();
    if (m_pRenderComponentCache) 
    {
        m_spriteRenderWidth = m_pRenderComponentCache->GetRenderWidth();
        m_spriteRenderHeight = m_pRenderComponentCache->GetRenderHeight();
    }
    else 
    {
        throw std::runtime_error("Ghost GameObject is missing RenderComponent.");
    }

    m_pSpriteAnimationComponentCache = GetGameObject()->GetComponent<SpriteAnimationComponent>();
    if (!m_pSpriteAnimationComponentCache) 
    {
        std::cout << "BaseGhostComponent Warning: Ghost GameObject,is missing SpriteAnimationComponent.\n";
    }

    const auto initialWorldPos = GetGameObject()->GetWorldPosition().GetPosition();
    glm::ivec2 initialGridPos = WorldToGrid(initialWorldPos + glm::vec3(m_spriteRenderWidth / 2.f, m_spriteRenderHeight / 2.f, 0.f));
    m_targetWorldPos = AdjustPositionForSpriteCenter(GridToWorldCenter(initialGridPos));


    if (m_currentState == GhostState::Idle && m_currentSpeed == 0.f && !m_isMoving)
    { 
        SetState(GhostState::Scatter);
    }


    m_isInitialized = true;
}

void BaseGhostComponent::SpawnAt(int gridRow, int gridCol, GhostState initialState, dae::Direction initialDir)
{
    if (!m_isInitialized) 
    {
        Initialize(); 
    }

    glm::ivec2 spawnGridPos = { gridCol, gridRow }; 
    glm::vec2 tileCenter = GridToWorldCenter(spawnGridPos);
    m_targetWorldPos = AdjustPositionForSpriteCenter(tileCenter);
    GetGameObject()->SetLocalPosition(m_targetWorldPos.x, m_targetWorldPos.y);

    m_currentDir = initialDir;

    SetState(initialState);

    if (m_isMoving) 
    {
        glm::ivec2 dirVec = DirectionToVec2(m_currentDir);
        glm::ivec2 nextGridPos = spawnGridPos + dirVec;
        m_targetWorldPos = AdjustPositionForSpriteCenter(GridToWorldCenter(nextGridPos));
    }
}

glm::ivec2 BaseGhostComponent::GetCurrentGridPosition() const
{
    if (!m_isInitialized) return { -1,-1 };

    const auto currentTopLeft = GetGameObject()->GetWorldPosition().GetPosition();
    glm::vec3 currentSpriteCenter = { currentTopLeft.x + m_spriteRenderWidth / 2.f, currentTopLeft.y + m_spriteRenderHeight / 2.f, 0.f };

    return WorldToGrid(currentSpriteCenter);
}

void BaseGhostComponent::Update() 
{
    if (!m_isInitialized) 
    {
        Initialize();
    }

    float deltaTime = static_cast<float>(dae::Time::GetInstance().GetDeltaTime());
    if (deltaTime <= 0.f) return;

    UpdateStateTimers(deltaTime);
    HandleStateTransitions();
    UpdateMovement(deltaTime);

     if (m_pSpriteAnimationComponentCache) 
     {
         m_pSpriteAnimationComponentCache->SetDirection(m_currentDir);
         m_pSpriteAnimationComponentCache->SetIsMoving(m_isMoving);
         // To do: add frightened and eaten sprites
     }
}

void BaseGhostComponent::UpdateMovement(float deltaTime) 
{
    auto* owner = GetGameObject();
    const auto currentTopLeft = owner->GetWorldPosition().GetPosition();
    glm::ivec2 currentGridPos = GetCurrentGridPosition();

    // Determine target grid cell based on state
    switch (m_currentState) 
    {
    case GhostState::Chase:

        if (m_pChaseBehaviour) 
        {
            m_currentTargetGridPos = m_pChaseBehaviour->CalculateChaseTarget(this);
        }

        break;
    case GhostState::Scatter:
        m_currentTargetGridPos = GetScatterTarget();
        break;
    case GhostState::Frightened:
        break;
    case GhostState::Eaten:
        m_currentTargetGridPos = GetEatenTarget();
        break;
    case GhostState::Idle:
        m_isMoving = false;
        return;
    case GhostState::ExitingPen:
        m_currentTargetGridPos = GetPenExitTarget();
        break;
    }

    float distToTargetSq = glm::distance(glm::vec2(currentTopLeft.x, currentTopLeft.y), m_targetWorldPos);

    // Reached target
    if (distToTargetSq < (m_tileCenterTolerance * m_tileCenterTolerance) || !m_isMoving) 
    { 
        owner->SetLocalPosition(m_targetWorldPos.x, m_targetWorldPos.y); 
        m_isMoving = true;
        m_justReversed = false;

        currentGridPos = GetCurrentGridPosition();

        // Make decision at intersection
        dae::Direction chosenDir = dae::Direction::Idle;
        if (m_nextDir != dae::Direction::Idle) 
        { // Player AI override
            if (CanMoveTo(currentGridPos, m_nextDir))
            {
                chosenDir = m_nextDir;
            }
            m_nextDir = dae::Direction::Idle;
        }

        if (chosenDir == dae::Direction::Idle) 
        {
            chosenDir = ChooseDirectionAtIntersection(currentGridPos, m_currentDir);
        }

        if (chosenDir != dae::Direction::Idle && chosenDir != GetOppositeDirection(m_currentDir)) 
        {
            m_currentDir = chosenDir;
        }
        else if (chosenDir == GetOppositeDirection(m_currentDir) && !m_justReversed)
        {
            // Allow reversal if state changed
            m_currentDir = chosenDir;
            m_justReversed = true; 
        }
        else if (chosenDir == dae::Direction::Idle) 
        { 
            m_isMoving = false;

            if (CanMoveTo(currentGridPos, GetOppositeDirection(m_currentDir))) 
            {
                m_currentDir = GetOppositeDirection(m_currentDir);
                m_isMoving = true;
                m_justReversed = true;
            }
        }

        // Update target based on direction
        if (m_isMoving)
        {
            glm::ivec2 dirVec = DirectionToVec2(m_currentDir);
            glm::ivec2 nextGridPos = currentGridPos + dirVec;
            m_targetWorldPos = AdjustPositionForSpriteCenter(GridToWorldCenter(nextGridPos));
        }
    }

    // Actual movement
    if (m_isMoving) 
    {
        glm::vec2 moveVec = m_targetWorldPos - glm::vec2(currentTopLeft.x, currentTopLeft.y);
        float distToTarget = glm::length(moveVec);

        if (distToTarget > 0.001f) 
        {
            glm::vec2 normMoveVec = glm::normalize(moveVec);
            float moveAmount = m_currentSpeed * deltaTime;

            if (moveAmount >= distToTarget) 
            { 
                owner->SetLocalPosition(m_targetWorldPos.x, m_targetWorldPos.y);
            }
            else 
            {
                owner->SetLocalPosition(currentTopLeft.x + normMoveVec.x * moveAmount,
                    currentTopLeft.y + normMoveVec.y * moveAmount);
            }
        }
    }
}


dae::Direction BaseGhostComponent::ChooseDirectionAtIntersection(const glm::ivec2& currentGridPos, dae::Direction lastDir)
{
    std::vector<dae::Direction> possibleTurns;
    dae::Direction oppositeDir = GetOppositeDirection(lastDir);

    // Check all 4 directions
    for (auto dir : { dae::Direction::Up, dae::Direction::Down, dae::Direction::Left, dae::Direction::Right })
    {
        if (dir == oppositeDir && m_currentState != GhostState::Frightened && !m_justReversed) 
        {
            continue;
        }
        if (CanMoveTo(currentGridPos, dir))
        {
            possibleTurns.push_back(dir);
        }
    }

    if (possibleTurns.empty()) 
    {
        if (CanMoveTo(currentGridPos, oppositeDir)) return oppositeDir;
        return dae::Direction::Idle;
    }

    if (possibleTurns.size() == 1)
    {
        return possibleTurns[0]; // Forced turn
    }

    // At an intersection with choices
    if (m_currentState == GhostState::Frightened)
    {
        // Pick a random turn
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(possibleTurns.begin(), possibleTurns.end(), g);
        return possibleTurns[0];
    }
    else
    {
        dae::Direction bestDir = dae::Direction::Idle;
        float minDistanceSq = std::numeric_limits<float>::max();

        for (dae::Direction turnDir : possibleTurns)
        {
            glm::ivec2 dirVec = DirectionToVec2(turnDir);            
            glm::ivec2 nextTileGridPos = currentGridPos + dirVec; 

            float distSq = glm::distance(glm::vec2(nextTileGridPos), glm::vec2(m_currentTargetGridPos));

            if (distSq < minDistanceSq)
            {
                minDistanceSq = distSq;
                bestDir = turnDir;
            }

            // TODO: Implement Pac-Man tie-breaking (Up > Left > Down > Right priority)
        }
        return bestDir;
    }
}

bool BaseGhostComponent::CanMoveTo(const glm::ivec2& gridPos, dae::Direction dir) const
{
    if (!m_pLevelGridCache || dir == dae::Direction::Idle) return false;
    glm::ivec2 dirVec = DirectionToVec2(dir);
    glm::ivec2 nextGridPos = gridPos + dirVec;

    return !m_pLevelGridCache->IsWall(nextGridPos.y, nextGridPos.x, true);
}


void BaseGhostComponent::UpdateStateTimers(float deltaTime)
{
    if (m_currentState == GhostState::Frightened) 
    {
        m_frightenTimer -= deltaTime;
        if (m_frightenTimer <= 0.f) 
        {
            Unfrighten();
        }
    }
    else if (m_currentState == GhostState::Scatter || m_currentState == GhostState::Idle)
    {
        m_stateTimer += deltaTime;
    }
}

void BaseGhostComponent::HandleStateTransitions()
{
    glm::ivec2 currentGridPos = GetCurrentGridPosition();
    if (currentGridPos.x == -1 && currentGridPos.y == -1) return;

    switch (m_currentState)
    {
    case GhostState::Idle:

        if (m_isActiveInPen && m_stateTimer >= IDLE_DURATION_IN_PEN)
        {
            SetState(GhostState::ExitingPen);
        }

        break;

    case GhostState::ExitingPen:

        if (currentGridPos == GetPenExitTarget()) 
        {
            SetState(GhostState::Scatter);
            m_isActiveInPen = false; 
        }

        break;

    case GhostState::Scatter:

        if (m_stateTimer >= SCATTER_DURATION) 
        {
            SetState(GhostState::Chase);
        }

        break;

    case GhostState::Eaten:

        if (currentGridPos == GetEatenTarget()) 
        {
            SetState(GhostState::ExitingPen);
        }
        break;

    case GhostState::Frightened:
        // Frighten timer is handled in UpdateStateTimers
        break;

    case GhostState::Chase:
        break;
    }
}


void BaseGhostComponent::SetState(GhostState newState)
{
    if (m_currentState == newState && m_isInitialized) return;

    m_previousState = m_currentState;
    m_currentState = newState;
    m_stateTimer = 0.f; 
    m_justReversed = true;

    switch (m_currentState) 
    {
    case GhostState::Scatter:
    case GhostState::ExitingPen:
    case GhostState::Chase:

        m_currentSpeed = m_normalSpeed;
        m_isMoving = true;

        break;
    case GhostState::Frightened:

        m_currentSpeed = m_frightenedSpeed;
        m_frightenTimer = FRIGHTEN_DURATION;
        m_isMoving = true;

        if (m_previousState == GhostState::Chase || m_previousState == GhostState::Scatter)
        {
            m_currentDir = GetOppositeDirection(m_currentDir);
            glm::ivec2 currentGhostGridPos = GetCurrentGridPosition(); 

            if (currentGhostGridPos.x != -1)
            { 
                glm::ivec2 reversedDirectionOffset = DirectionToVec2(m_currentDir);
                glm::ivec2 targetTileGridCoords = currentGhostGridPos + reversedDirectionOffset;
                m_targetWorldPos = AdjustPositionForSpriteCenter(GridToWorldCenter(targetTileGridCoords));
            }
        }

        break;
    case GhostState::Eaten:

        m_currentSpeed = m_eatenSpeed;
        m_isMoving = true;

        // To do: Visuals change to eyes

        break;
    case GhostState::Idle:

        m_currentSpeed = 10; 
        m_isMoving = false;

        break;
    }

}

void BaseGhostComponent::Frighten() 
{
    if (m_currentState != GhostState::Eaten) 
    { 
        SetState(GhostState::Frightened);
    }
}

void BaseGhostComponent::Unfrighten() 
{
    if (m_currentState == GhostState::Frightened) 
    {
        if (m_previousState == GhostState::Scatter || m_previousState == GhostState::Chase) 
        {
            SetState(m_previousState);
        }
        else 
        {
            SetState(GhostState::Chase);
        }
    }
}

void BaseGhostComponent::WasEaten() 
{
    if (m_currentState == GhostState::Frightened) 
    {
        SetState(GhostState::Eaten);
        // TODO: Add score for eating ghost
        // if(m_pPacmanObject->GetComponent<ScoreComponent>()) m_pPacmanObject->GetComponent<ScoreComponent>()->GainPoints(GHOST_EAT_SCORE);
    }
}

glm::ivec2 BaseGhostComponent::GetEatenTarget() const 
{
    return { 13, 14 };
}


void BaseGhostComponent::SetNextDirection(dae::Direction direction) 
{
    const auto currentGridPos = GetCurrentGridPosition();
    if (currentGridPos.x == -1) return;

    if (CanMoveTo(currentGridPos, direction) && direction != GetOppositeDirection(m_currentDir))
    {
        m_nextDir = direction;
    }
    else if (direction == GetOppositeDirection(m_currentDir))
    {
        m_nextDir = direction;
    }
}

glm::ivec2 BaseGhostComponent::WorldToGrid(const glm::vec3& worldPos) const 
{
    if (!m_pLevelGridCache) return { -1,-1 };

    std::pair<int, int> rowColPair = m_pLevelGridCache->WorldToGridCoords(worldPos.x, worldPos.y);

    return { rowColPair.second, rowColPair.first };
}

glm::vec2 BaseGhostComponent::GridToWorldCenter(const glm::ivec2& gridPos) const
{
    if (!m_pLevelGridCache) return { 0.f, 0.f };

    glm::vec2 tileTopLeft = m_pLevelGridCache->GridToWorldCoords(gridPos.y, gridPos.x);
    float halfTile = m_pLevelGridCache->GetTileSize() / 2.0f;

    return { tileTopLeft.x + halfTile, tileTopLeft.y + halfTile };
}

glm::vec2 BaseGhostComponent::AdjustPositionForSpriteCenter(glm::vec2 targetCenterPos) const
{
    return { targetCenterPos.x - m_spriteRenderWidth / 2.0f,
             targetCenterPos.y - m_spriteRenderHeight / 2.0f };
}


void BaseGhostComponent::RenderImGui() {

    std::string name = "Ghost";

    if (ImGui::TreeNode(name.c_str())) 
    {
        ImGui::Text("State: %d", static_cast<int>(m_currentState));
        ImGui::Text("Current Dir: %d", static_cast<int>(m_currentDir));
        ImGui::Text("Is Moving: %s", m_isMoving ? "True" : "False");
        ImGui::Text("Target Grid: (%d, %d)", m_currentTargetGridPos.x, m_currentTargetGridPos.y);
        const auto pos = GetGameObject()->GetWorldPosition().GetPosition();
        ImGui::Text("World Pos TL: (%.1f, %.1f)", pos.x, pos.y);
        ImGui::TreePop();
    }
}

// Helper to get opposite direction
dae::Direction GetOppositeDirection(dae::Direction dir)
{
    switch (dir) 
    {
    case dae::Direction::Up:    return dae::Direction::Down;
    case dae::Direction::Down:  return dae::Direction::Up;
    case dae::Direction::Left:  return dae::Direction::Right;
    case dae::Direction::Right: return dae::Direction::Left;
    default: return dae::Direction::Idle;
    }
}

// Helper to convert dae::Direction to glm::ivec2
glm::ivec2 DirectionToVec2(dae::Direction dir)
{
    switch (dir) 
    {
    case dae::Direction::Up:    return { 0, -1 };
    case dae::Direction::Down:  return { 0, 1 };
    case dae::Direction::Left:  return { -1, 0 };
    case dae::Direction::Right: return { 1, 0 };
    default: return { 0,0 };
    }
}
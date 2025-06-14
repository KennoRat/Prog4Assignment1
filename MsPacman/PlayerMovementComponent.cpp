#include "PlayerMovementComponent.h"
#include <imgui.h>
#include <iostream>

// Engine
#include "GameObject.h"
#include "TimeGameEngine.h"
#include "RenderComponent.h" 
#include "ServiceLocator.h"

// Game
#include "LevelGridComponent.h"
#include "ScoreComponent.h" 
#include "PowerPelletComponent.h"

namespace Sounds 
{
    dae::SoundId PELLET_EAT_SOUND = dae::INVALID_SOUND_ID; 
    dae::SoundId POWER_PELLET_EAT_SOUND = dae::INVALID_SOUND_ID; 
}

PlayerMovementComponent::PlayerMovementComponent(std::shared_ptr<dae::GameObject> gameObject, std::shared_ptr<dae::GameObject> levelObject, float speed) :
    BaseComponent(*gameObject),
    m_pLevelObject(levelObject),
    m_speed(speed)
{
}

void PlayerMovementComponent::Initialize()
{
    if (m_initialized) return;

    m_pLevelGridCache = m_pLevelObject->GetComponent<LevelGridComponent>();
    if (!m_pLevelGridCache)
    {
        throw std::runtime_error("Level GameObject does not have a LevelGridComponent!");
    }

    m_pScoreComponentCache = GetGameObject()->GetComponent<dae::ScoreComponent>();
    if (!m_pScoreComponentCache)
    {
        throw std::runtime_error("PlayerMovementComponent owner does not have a ScoreComponent!");
    }

    // Move Player to Spawn Level Grid
    m_pRenderComponentCache = GetGameObject()->GetComponent<dae::RenderComponent>();
    if (m_pRenderComponentCache) 
    {
        m_spriteRenderWidth = m_pRenderComponentCache->GetRenderWidth();
        m_spriteRenderHeight = m_pRenderComponentCache->GetRenderHeight();
        if (m_spriteRenderWidth == 0.f || m_spriteRenderHeight == 0.f)
        {
            std::cout << "PlayerMovementComponent Warning: Sprite render dimensions are 0.\n";
        }
    } 
    else 
    {
        throw std::runtime_error("PlayerMovementComponent owner does not have a RenderComponent! Needed for sprite centering.");
    }

    const auto initialWorldPos = GetGameObject()->GetWorldPosition().GetPosition();
    const auto initialGridCoords = m_pLevelGridCache->WorldToGridCoords(initialWorldPos.x + m_spriteRenderWidth / 2.f,
                                                                      initialWorldPos.y + m_spriteRenderHeight / 2.f);
    glm::vec2 tileCenter = GetTileCenter(initialGridCoords.first, initialGridCoords.second);
    m_targetPosition = AdjustPositionForSpriteCenter(tileCenter);

    // Load Sounds 
    if (Sounds::PELLET_EAT_SOUND == dae::INVALID_SOUND_ID)
    {
        std::string pelletSoundPath = "../Data/Sounds/ms_eat_dot.wav";
        Sounds::PELLET_EAT_SOUND = dae::ServiceLocator::GetAudioService().LoadSound(pelletSoundPath);
        if (Sounds::PELLET_EAT_SOUND == dae::INVALID_SOUND_ID) 
        {
            std::cerr << "Failed to queue loading for Pellet sound.\n";
        }
        else 
        {
            std::cout << "Queued loading for Pellet sound with ID: " << Sounds::PELLET_EAT_SOUND << std::endl;
        }
    }

    if (Sounds::POWER_PELLET_EAT_SOUND == dae::INVALID_SOUND_ID)
    {
        std::string powerPelletSoundPath = "../Data/Sounds/pacman_eatfruit.wav"; 
        Sounds::POWER_PELLET_EAT_SOUND = dae::ServiceLocator::GetAudioService().LoadSound(powerPelletSoundPath);
        if (Sounds::POWER_PELLET_EAT_SOUND == dae::INVALID_SOUND_ID) 
        {
            std::cerr << "Failed to queue loading for Power Pellet sound.\n";
        }
        else 
        {
            std::cout << "Queued loading for Power Pellet sound with ID: " << Sounds::POWER_PELLET_EAT_SOUND << std::endl;
        }
    }

    m_initialized = true;
}

void PlayerMovementComponent::Update()
{
    if (!m_isActive) return;

    const float deltaTime = static_cast<float>(dae::Time::GetInstance().GetDeltaTime());
    if (deltaTime <= 0.f) return; // Avoid zero

    auto* owner = GetGameObject(); 
    const auto currentSpriteCenter = owner->GetWorldPosition().GetPosition() + glm::vec3(m_spriteRenderWidth / 2.f, m_spriteRenderHeight / 2.f, 0.f);
    const auto currentGridCoords = m_pLevelGridCache->WorldToGridCoords(currentSpriteCenter.x, currentSpriteCenter.y);

    // Handle Turning
    if (m_desiredDirection != dae::Direction::Idle)
    {
        // Check is near center of the tile
        if (IsNearTarget(m_tileCenterTolerance))
        {
            glm::vec2 currentTargetCenter = { m_targetPosition.x + m_spriteRenderWidth / 2.f, m_targetPosition.y + m_spriteRenderHeight / 2.f };
            const auto targetTileGridCoords = m_pLevelGridCache->WorldToGridCoords(currentTargetCenter.x, currentTargetCenter.y);

            // Check if player can turn
            if (CanMove(currentGridCoords.first, currentGridCoords.second, m_desiredDirection))
            {
                HandleTileReached(targetTileGridCoords.first, targetTileGridCoords.second);

                // Snap and change to new direction
                owner->SetLocalPosition(m_targetPosition.x, m_targetPosition.y);
                m_currentDirection = m_desiredDirection;
                m_desiredDirection = dae::Direction::Idle;
                m_isMoving = true;

                // Calculate target tile based on the turn
                glm::vec2 dirVec = GetDirectionVector(m_currentDirection);
                int nextRow = currentGridCoords.first + static_cast<int>(dirVec.y);
                int nextCol = currentGridCoords.second + static_cast<int>(dirVec.x);
                glm::vec2 nextTileCenter = GetTileCenter(nextRow, nextCol);
                m_targetPosition = AdjustPositionForSpriteCenter(nextTileCenter);
            }
        }
    }

    // Handle Movement
    if (m_isMoving)
    {
        const auto currentTopLeftPos = owner->GetWorldPosition().GetPosition();
        glm::vec2 moveVector = m_targetPosition - glm::vec2(currentTopLeftPos.x, currentTopLeftPos.y);
        float distanceToTarget = glm::length(moveVector);

        if (distanceToTarget > 0.01f)
        {
            glm::vec2 moveDirection = glm::normalize(moveVector);
            float moveDistance = m_speed * deltaTime;

            // Reach new target
            if (moveDistance >= distanceToTarget)
            {
                // Reached Target and snap
                owner->SetLocalPosition(m_targetPosition.x, m_targetPosition.y);

                
                glm::vec2 reachedTileCenter = { m_targetPosition.x + m_spriteRenderWidth / 2.f, m_targetPosition.y + m_spriteRenderHeight / 2.f };
                const auto reachedTileGridCoords = m_pLevelGridCache->WorldToGridCoords(reachedTileCenter.x, reachedTileCenter.y);
                HandleTileReached(reachedTileGridCoords.first, reachedTileGridCoords.second);

                const auto postReachTopLeft = owner->GetWorldPosition().GetPosition();
                const auto postReachSpriteCenter = postReachTopLeft + glm::vec3(m_spriteRenderWidth / 2.f, m_spriteRenderHeight / 2.f, 0.f);
                const auto postReachGridCoords = m_pLevelGridCache->WorldToGridCoords(postReachSpriteCenter.x, postReachSpriteCenter.y);


                // Continue moving in the same direction
                if (CanMove(postReachGridCoords.first, postReachGridCoords.second, m_currentDirection))
                {
                    // Get the next target tile
                    glm::vec2 dirVec = GetDirectionVector(m_currentDirection);
                    int nextRow = postReachGridCoords.first + static_cast<int>(dirVec.y);
                    int nextCol = postReachGridCoords.second + static_cast<int>(dirVec.x);
                    glm::vec2 nextTileCenter = GetTileCenter(nextRow, nextCol);
                    m_targetPosition = AdjustPositionForSpriteCenter(nextTileCenter);
                    m_isMoving = true;
                }
                else
                {
                    // Hit a wall
                    m_isMoving = false;
                }
            }
            else
            {
                // Move Towards Target
                glm::vec2 newPos = glm::vec2(currentTopLeftPos.x, currentTopLeftPos.y) + moveDirection * moveDistance;
                owner->SetLocalPosition(newPos.x, newPos.y);
            }
        }
        else // Already at target
        {
            owner->SetLocalPosition(m_targetPosition.x, m_targetPosition.y);

            // Continue moving in the same direction
            glm::vec2 reachedTileCenter = { m_targetPosition.x + m_spriteRenderWidth / 2.f, m_targetPosition.y + m_spriteRenderHeight / 2.f };
            const auto reachedTileGridCoords = m_pLevelGridCache->WorldToGridCoords(reachedTileCenter.x, reachedTileCenter.y);
            HandleTileReached(reachedTileGridCoords.first, reachedTileGridCoords.second);

            const auto postReachTopLeft = owner->GetWorldPosition().GetPosition();
            const auto postReachSpriteCenter = postReachTopLeft + glm::vec3(m_spriteRenderWidth / 2.f, m_spriteRenderHeight / 2.f, 0.f);
            const auto postReachGridCoords = m_pLevelGridCache->WorldToGridCoords(postReachSpriteCenter.x, postReachSpriteCenter.y);

            if (CanMove(postReachGridCoords.first, postReachGridCoords.second, m_currentDirection))
            {
                // Get the next target tile
                glm::vec2 dirVec = GetDirectionVector(m_currentDirection);
                int nextRow = postReachGridCoords.first + static_cast<int>(dirVec.y);
                int nextCol = postReachGridCoords.second + static_cast<int>(dirVec.x);
                glm::vec2 nextTileCenter = GetTileCenter(nextRow, nextCol);
                m_targetPosition = AdjustPositionForSpriteCenter(nextTileCenter);
                m_isMoving = true;
            }
            else
            {
                m_isMoving = false;
            }
        }
    }
    else if (m_desiredDirection != dae::Direction::Idle) // Check for new desired direction
    {
        // Try to start moving if possible from current snapped position
        if (CanMove(currentGridCoords.first, currentGridCoords.second, m_desiredDirection))
        {
            glm::vec2 currentTileActualCenter = GetTileCenter(currentGridCoords.first, currentGridCoords.second);
            glm::vec2 newLocalPosition = AdjustPositionForSpriteCenter(currentTileActualCenter);
            owner->SetLocalPosition(newLocalPosition.x, newLocalPosition.y);

            m_currentDirection = m_desiredDirection;
            m_desiredDirection = dae::Direction::Idle;
            m_isMoving = true;

            glm::vec2 dirVec = GetDirectionVector(m_currentDirection);
            int nextRow = currentGridCoords.first + static_cast<int>(dirVec.y);
            int nextCol = currentGridCoords.second + static_cast<int>(dirVec.x);
            glm::vec2 nextTileCenter = GetTileCenter(nextRow, nextCol);
            m_targetPosition = AdjustPositionForSpriteCenter(nextTileCenter);
        }
    }
}

void PlayerMovementComponent::Render() const
{
    // Do nothing
}

void PlayerMovementComponent::RenderImGui()
{
    if (!m_initialized || !m_pLevelGridCache) return;

    if (ImGui::TreeNode("PlayerMovementComponent Debug"))
    {
        const auto currentPos = GetGameObject()->GetWorldPosition().GetPosition();
        const auto gridCoords = m_pLevelGridCache->WorldToGridCoords(currentPos.x, currentPos.y);

        ImGui::Text("World Pos: (%.1f, %.1f)", currentPos.x, currentPos.y);
        ImGui::Text("Grid Pos: (%d, %d)", gridCoords.first, gridCoords.second);
        ImGui::Text("Target Pos: (%.1f, %.1f)", m_targetPosition.x, m_targetPosition.y);
        ImGui::Text("Is Moving: %s", m_isMoving ? "True" : "False");

        const char* currentDirStr = "None";
        switch (m_currentDirection)
        {
        case dae::Direction::Up: currentDirStr = "Up"; break;
        case dae::Direction::Down: currentDirStr = "Down"; break;
        case dae::Direction::Left: currentDirStr = "Left"; break;
        case dae::Direction::Right: currentDirStr = "Right"; break;
        }
        ImGui::Text("Current Direction: %s", currentDirStr);

        const char* desiredDirStr = "None";
        switch (m_desiredDirection)
        {
        case dae::Direction::Up: desiredDirStr = "Up"; break;
        case dae::Direction::Down: desiredDirStr = "Down"; break;
        case dae::Direction::Left: desiredDirStr = "Left"; break;
        case dae::Direction::Right: desiredDirStr = "Right"; break;
        case dae::Direction::Idle: desiredDirStr = "Idle"; break;
        }
        ImGui::Text("Desired Direction: %s", desiredDirStr);

        ImGui::TreePop();
    }
}


void PlayerMovementComponent::SetDesiredDirection(dae::Direction dir)
{
    m_desiredDirection = dir;
}

void PlayerMovementComponent::SnapToGrid(int row, int col)
{
    if(!m_initialized) 
    {
        Initialize(); 
    }
    if (!m_pLevelGridCache) return;

    if (row >= 0 && row < m_pLevelGridCache->GetRows() && col >= 0 && col < m_pLevelGridCache->GetCols())
    {
        glm::vec2 tileCenter = GetTileCenter(row, col);
        glm::vec2 targetTopLeft = AdjustPositionForSpriteCenter(tileCenter);

        GetGameObject()->SetLocalPosition(targetTopLeft.x, targetTopLeft.y);
        m_targetPosition = targetTopLeft; 
        m_isMoving = false;

        std::cout << "Snapped player to grid (" << row << ", " << col << ") at world TL (" << targetTopLeft.x << ", " << targetTopLeft.y << ")" << std::endl;
    }
}


bool PlayerMovementComponent::CanMove(int startRow, int startCol, dae::Direction dir) const
{
    if (!m_pLevelGridCache) return false;

    glm::vec2 dirVec = GetDirectionVector(dir);
    int nextRow = startRow + static_cast<int>(dirVec.y);
    int nextCol = startCol + static_cast<int>(dirVec.x);

    return !m_pLevelGridCache->IsWall(nextRow, nextCol);
}

glm::vec2 PlayerMovementComponent::GetTileCenter(int row, int col) const
{
    if (!m_pLevelGridCache) return { 0.f, 0.f };

    glm::vec2 tileTopLeft = m_pLevelGridCache->GridToWorldCoords(row, col);
    float halfTileSize = m_pLevelGridCache->GetTileSize() / 2.0f;
    return { tileTopLeft.x + halfTileSize, tileTopLeft.y + halfTileSize };
}

bool PlayerMovementComponent::IsNearTarget(float tolerance) const
{
    const auto currentPos = GetGameObject()->GetWorldPosition().GetPosition();
    float dx = m_targetPosition.x - currentPos.x;
    float dy = m_targetPosition.y - currentPos.y;

    // Using squared distance to avoid sqrt
    return (dx * dx + dy * dy) < (tolerance * tolerance);
}

glm::vec2 PlayerMovementComponent::GetDirectionVector(dae::Direction dir) const
{
    switch (dir)
    {
    case dae::Direction::Up:    return { 0.f, -1.f };
    case dae::Direction::Down:  return { 0.f,  1.f };
    case dae::Direction::Left:  return { -1.f, 0.f };
    case dae::Direction::Right: return { 1.f,  0.f };
    case dae::Direction::Idle: return { 0.f,  0.f };
    default: return { 0.f,  0.f };
    }
}

glm::vec2 PlayerMovementComponent::AdjustPositionForSpriteCenter(glm::vec2 targetCenterPos) const
{
    if (m_pRenderComponentCache) 
    {
        return 
        {
            targetCenterPos.x - m_spriteRenderWidth / 2.0f,
            targetCenterPos.y - m_spriteRenderHeight / 2.0f
        };
    }

    return targetCenterPos;
}

void PlayerMovementComponent::HandleTileReached(int tileRow, int tileCol)
{
    if (!m_pLevelGridCache) return;

    TileType currentTile = m_pLevelGridCache->GetTileType(tileRow, tileCol);

    // Check for Pellets
    if (currentTile == TileType::Pellet)
    {
        m_pLevelGridCache->ClearTileAndDecrementPellet(tileRow, tileCol);

        if (m_pScoreComponentCache) m_pScoreComponentCache->GainPoints(PELLET_SCORE);

        dae::ServiceLocator::GetAudioService().PlaySound(Sounds::PELLET_EAT_SOUND, 0.7f);
    }
    else if (currentTile == TileType::PowerPellet)
    {
        m_pLevelGridCache->ClearTileAndDecrementPellet(tileRow, tileCol);

        if (m_pScoreComponentCache) m_pScoreComponentCache->GainPoints(POWER_PELLET_SCORE);

        dae::ServiceLocator::GetAudioService().PlaySound(Sounds::POWER_PELLET_EAT_SOUND, 0.8f);

        auto powerComp = GetGameObject()->GetComponent<PowerPelletComponent>();
        if (powerComp)
        {
            powerComp->ActivatePowerUp();
        }
    }

    // Check for Tunnel Wrapping
    std::pair<int, int> destinationOpt = m_pLevelGridCache->GetTeleportDestination(currentTile);
    if (destinationOpt != std::pair(0, 0))
    {
        const auto& destination = destinationOpt;

        std::cout << "Wrapping from (" << tileRow << "," << tileCol << ") type " << static_cast<int>(currentTile)
            << " to (" << destination.first << "," << destination.second << ")" << std::endl;

        SnapToGrid(destination.first, destination.second);
    }
}
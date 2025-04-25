#include "PlayerMovementComponent.h"
#include "GameObject.h"
#include "TimeGameEngine.h"
#include "LevelGridComponent.h"
#include <imgui.h>
#include <iostream>

PlayerMovementComponent::PlayerMovementComponent(std::shared_ptr<dae::GameObject> owner, std::shared_ptr<dae::GameObject> levelObject, float speed) :
    BaseComponent(*owner), // Pass raw pointer to BaseComponent constructor
    m_pLevelObject(levelObject),
    m_speed(speed)
{
    if (!m_pLevelObject)
    {
        throw std::runtime_error("PlayerMovementComponent requires a valid level GameObject!");
    }
}

void PlayerMovementComponent::Initialize()
{
    if (m_initialized) return;

    m_pLevelGridCache = m_pLevelObject->GetComponent<LevelGridComponent>();
    if (!m_pLevelGridCache)
    {
        throw std::runtime_error("Level GameObject does not have a LevelGridComponent!");
    }

    // Move Player to Spawn Level Grid
    const auto currentPos = GetGameObject()->GetWorldPosition().GetPosition();
    const auto gridCoords = m_pLevelGridCache->WorldToGridCoords(currentPos.x, currentPos.y);
    m_targetPosition = GetTileCenter(gridCoords.first, gridCoords.second);
    GetGameObject()->SetLocalPosition(m_targetPosition.x, m_targetPosition.y);

    m_initialized = true;
}

void PlayerMovementComponent::Update()
{
    const float deltaTime = static_cast<float>(dae::Time::GetInstance().GetDeltaTime());
    if (deltaTime <= 0.f) return; // Avoid division by zero or weird behavior

    // Temp Raw pointer to keep Game Object
    auto* owner = GetGameObject(); 
    const auto currentPos = owner->GetWorldPosition().GetPosition();
    const auto currentGridCoords = m_pLevelGridCache->WorldToGridCoords(currentPos.x, currentPos.y);

    // Handle Turning
    if (m_desiredDirection != dae::Direction::Idle)
    {
        // Check if player is close enough to the center of the tile
        if (IsNearTarget(m_tileCenterTolerance))
        {
            // Check if player can turn
            if (CanMove(currentGridCoords.first, currentGridCoords.second, m_desiredDirection))
            {
                // Snap and change to new direction
                owner->SetLocalPosition(m_targetPosition.x, m_targetPosition.y);
                m_currentDirection = m_desiredDirection;
                m_desiredDirection = dae::Direction::Idle;
                m_isMoving = true;

                // Calculate the NEW target tile based on the turn
                glm::vec2 dirVec = GetDirectionVector(m_currentDirection);
                int nextRow = currentGridCoords.first + static_cast<int>(dirVec.y);
                int nextCol = currentGridCoords.second + static_cast<int>(dirVec.x);
                m_targetPosition = GetTileCenter(nextRow, nextCol);
            }
        }
    }

    // Handle Movement
    if (m_isMoving)
    {
        glm::vec2 moveVector = m_targetPosition - glm::vec2(currentPos.x, currentPos.y);
        float distanceToTarget = glm::length(moveVector);

        if (distanceToTarget > 0.01f)
        {
            glm::vec2 moveDirection = glm::normalize(moveVector);
            float moveDistance = m_speed * deltaTime;

            // Check if we reach new target
            if (moveDistance >= distanceToTarget)
            {
                // Reached Target and snap
                owner->SetLocalPosition(m_targetPosition.x, m_targetPosition.y);

                // Check if we can continue moving in the same direction
                const auto targetGridCoords = m_pLevelGridCache->WorldToGridCoords(m_targetPosition.x, m_targetPosition.y);

                if (CanMove(targetGridCoords.first, targetGridCoords.second, m_currentDirection))
                {
                    // Get the next target tile
                    glm::vec2 dirVec = GetDirectionVector(m_currentDirection);
                    int nextRow = targetGridCoords.first + static_cast<int>(dirVec.y);
                    int nextCol = targetGridCoords.second + static_cast<int>(dirVec.x);
                    m_targetPosition = GetTileCenter(nextRow, nextCol);
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
                glm::vec2 newPos = glm::vec2(currentPos.x, currentPos.y) + moveDirection * moveDistance;
                owner->SetLocalPosition(newPos.x, newPos.y);
            }
        }
        else // Already at target
        {
            owner->SetLocalPosition(m_targetPosition.x, m_targetPosition.y); // 

            // Check if we can continue moving in the same direction
            const auto targetGridCoords = m_pLevelGridCache->WorldToGridCoords(m_targetPosition.x, m_targetPosition.y);

            if (CanMove(targetGridCoords.first, targetGridCoords.second, m_currentDirection))
            {
                // Get the next target tile
                glm::vec2 dirVec = GetDirectionVector(m_currentDirection);
                int nextRow = targetGridCoords.first + static_cast<int>(dirVec.y);
                int nextCol = targetGridCoords.second + static_cast<int>(dirVec.x);
                m_targetPosition = GetTileCenter(nextRow, nextCol);
                m_isMoving = true;
            }
            else
            {
                // Hit a wall
                m_isMoving = false;
            }
        }
    }
    else if (m_desiredDirection != dae::Direction::Idle) // Not moving, check for new desired direction
    {
        // Try to start moving immediately if possible from current snapped position
        if (CanMove(currentGridCoords.first, currentGridCoords.second, m_desiredDirection))
        {
            m_currentDirection = m_desiredDirection;
            m_desiredDirection = dae::Direction::Idle;
            m_isMoving = true;

            // Get the next target tile
            glm::vec2 dirVec = GetDirectionVector(m_currentDirection);
            int nextRow = currentGridCoords.first + static_cast<int>(dirVec.y);
            int nextCol = currentGridCoords.second + static_cast<int>(dirVec.x);
            m_targetPosition = GetTileCenter(nextRow, nextCol);
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
    // Initialize first if haven't done yet
    if (!m_pLevelGridCache)
    {
        Initialize();
    }

    if (row >= 0 && row < m_pLevelGridCache->GetRows() && col >= 0 && col < m_pLevelGridCache->GetCols())
    {
        glm::vec2 centerPos = GetTileCenter(row, col);
        GetGameObject()->SetLocalPosition(centerPos.x, centerPos.y);
        m_targetPosition = centerPos; // Update target as well
        m_isMoving = false; // Stop movement when snapping
        std::cout << "Snapped player to grid (" << row << ", " << col << ") at world (" << centerPos.x << ", " << centerPos.y << ")" << std::endl;

    }
}


bool PlayerMovementComponent::CanMove(int startRow, int startCol, dae::Direction dir) const
{
    if (!m_pLevelGridCache) return false;

    glm::vec2 dirVec = GetDirectionVector(dir);
    int nextRow = startRow + static_cast<int>(dirVec.y);
    int nextCol = startCol + static_cast<int>(dirVec.x);

    // Check bounds first
    if (nextRow < 0 || nextRow >= m_pLevelGridCache->GetRows() ||
        nextCol < 0 || nextCol >= m_pLevelGridCache->GetCols())
    {
        // To do: Allow movement for tunnel wrapping
        return false;
    }

    // Check if the target tile is a wall
    return !m_pLevelGridCache->IsWall(nextRow, nextCol);
}

glm::vec2 PlayerMovementComponent::GetTileCenter(int row, int col) const
{
    if (!m_pLevelGridCache) return { 0.f, 0.f };

    glm::vec2 topLeft = m_pLevelGridCache->GridToWorldCoords(row, col);
    float halfTile = m_pLevelGridCache->GetTileSize() / 2.0f;
    return { topLeft.x + halfTile, topLeft.y + halfTile };
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
#include "InkyChaseBehaviour.h"
#include "BaseGhostComponent.h"
#include "GameObject.h"
#include "PlayerMovementComponent.h"
#include "LevelGridComponent.h"
#include "MoveCommand.h"
#include <iostream>


static glm::ivec2 GetDirectionalOffsetForInky(dae::Direction dir, int distance)
{
    switch (dir)
    {
    case dae::Direction::Up:    return { 0, -distance };
    case dae::Direction::Down:  return { 0, distance };
    case dae::Direction::Left:  return { -distance, 0 };
    case dae::Direction::Right: return { distance, 0 };
    default: return { 0, 0 };
    }
}


InkyChaseBehaviour::InkyChaseBehaviour(std::function<dae::GameObject* ()> blinkyGetter)
    : m_fnGetBlinky(std::move(blinkyGetter))
{
    if (!m_fnGetBlinky)
    {
        throw std::runtime_error("InkyChaseBehaviour requires a valid function to get Blinky.");
    }
}

glm::ivec2 InkyChaseBehaviour::CalculateChaseTarget(const BaseGhostComponent* pInkyGhostComponent) const
{
    if (!pInkyGhostComponent)
    {
        return { -1, -1 };
    }

    auto pacmanObject = pInkyGhostComponent->GetPacmanObject();
    LevelGridComponent* levelGrid = pInkyGhostComponent->GetLevelGrid();
    PlayerMovementComponent* pacmanMove = pInkyGhostComponent->GetPacmanMovement();
    dae::GameObject* blinkyObject = m_fnGetBlinky ? m_fnGetBlinky() : nullptr;

    if (!pacmanObject || !levelGrid || !pacmanMove || !blinkyObject)
    {
        return pInkyGhostComponent->GetScatterTarget();
    }

    BaseGhostComponent* blinkyLogic = blinkyObject->GetComponent<BaseGhostComponent>();
    if (!blinkyLogic)
    {
        return pInkyGhostComponent->GetScatterTarget();
    }

    glm::vec3 pacmanWorldPos_TL = pacmanObject->GetWorldPosition().GetPosition();
    float pacmanSpriteWidth = pacmanMove->GetSpriteRenderWidth();
    float pacmanSpriteHeight = pacmanMove->GetSpriteRenderHeight();
    glm::vec2 pacmanSpriteCenterWorld = {
        pacmanWorldPos_TL.x + pacmanSpriteWidth / 2.0f,
        pacmanWorldPos_TL.y + pacmanSpriteHeight / 2.0f
    };

    std::pair<int, int> pacmanGridRowCol = levelGrid->WorldToGridCoords(pacmanSpriteCenterWorld.x, pacmanSpriteCenterWorld.y);
    glm::ivec2 pacmanCurrentTile = { pacmanGridRowCol.second, pacmanGridRowCol.first }; // {COL, ROW}

    dae::Direction pacmanDirection = pacmanMove->GetCurrentDirection();
    int lookAheadDistance = 2;
    glm::ivec2 pacmanOffsetVec = GetDirectionalOffsetForInky(pacmanDirection, lookAheadDistance);
    glm::ivec2 tileTwoAheadOfPacman = pacmanCurrentTile + pacmanOffsetVec; // {COL, ROW}

    // Get Blinky
    glm::ivec2 blinkyCurrentTile = blinkyLogic->GetCurrentGridPosition();
    glm::ivec2 vectorBlinkyToPacOffset = tileTwoAheadOfPacman - blinkyCurrentTile;
    glm::ivec2 inkyTargetTile = blinkyCurrentTile + (vectorBlinkyToPacOffset * 2); // {COL, ROW}

    // Boundary checks
    inkyTargetTile.x = std::max(0, std::min(inkyTargetTile.x, levelGrid->GetCols() - 1));
    inkyTargetTile.y = std::max(0, std::min(inkyTargetTile.y, levelGrid->GetRows() - 1));

    return inkyTargetTile;
}
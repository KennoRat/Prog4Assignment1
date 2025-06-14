#include "PinkyChaseBehaviour.h"
#include "BaseGhostComponent.h"
#include "GameObject.h"
#include "PlayerMovementComponent.h"
#include "LevelGridComponent.h"
#include "MoveCommand.h"

// Helper function
static glm::ivec2 GetDirectionalOffset(dae::Direction dir, int distance)
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

glm::ivec2 PinkyChaseBehaviour::CalculateChaseTarget(const BaseGhostComponent* pGhostComponent) const
{
    if (!pGhostComponent)
    {
        return { -1, -1 }; 
    }

    auto pacmanObject = pGhostComponent->GetPacmanObject();
    LevelGridComponent* levelGrid = pGhostComponent->GetLevelGrid();
    PlayerMovementComponent* pacmanMove = pGhostComponent->GetPacmanMovement();

    if (pacmanObject && levelGrid && pacmanMove)
    {

        glm::vec3 pacmanWorldPos_TL = pacmanObject->GetWorldPosition().GetPosition();
        float pacmanSpriteWidth = pacmanMove->GetSpriteRenderWidth();
        float pacmanSpriteHeight = pacmanMove->GetSpriteRenderHeight();
        glm::vec2 pacmanSpriteCenterWorld =
        {
            pacmanWorldPos_TL.x + pacmanSpriteWidth / 2.0f,
            pacmanWorldPos_TL.y + pacmanSpriteHeight / 2.0f
        };
        std::pair<int, int> pacmanGridRowCol = levelGrid->WorldToGridCoords
        (
            pacmanSpriteCenterWorld.x,
            pacmanSpriteCenterWorld.y
        );

        glm::ivec2 pacmanCurrentTile = { pacmanGridRowCol.second, pacmanGridRowCol.first }; // {COL, ROW}


        dae::Direction pacmanDirection = pacmanMove->GetCurrentDirection();

        int lookAheadDistance = 4;
        glm::ivec2 offset = GetDirectionalOffset(pacmanDirection, lookAheadDistance);

        glm::ivec2 targetTile = pacmanCurrentTile + offset; // {COL, ROW}

        if (targetTile.x < 0 || targetTile.x >= levelGrid->GetCols() ||
            targetTile.y < 0 || targetTile.y >= levelGrid->GetRows())
        {
            return pacmanCurrentTile;
        }

        return targetTile; // {COL, ROW}
    }

    return pGhostComponent->GetScatterTarget(); // Fallback
}
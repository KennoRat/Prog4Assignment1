#include "SueChaseBehaviour.h"
#include "BaseGhostComponent.h"
#include "GameObject.h"
#include "PlayerMovementComponent.h"
#include "LevelGridComponent.h"
#include <iostream> 

glm::ivec2 SueChaseBehaviour::CalculateChaseTarget(const BaseGhostComponent* pClydeGhostComponent) const
{
    if (!pClydeGhostComponent)
    {
        return { -1, -1 };
    }

    auto pacmanObject = pClydeGhostComponent->GetPacmanObject();
    LevelGridComponent* levelGrid = pClydeGhostComponent->GetLevelGrid();
    PlayerMovementComponent* pacmanMove = pClydeGhostComponent->GetPacmanMovement();

    if (!pacmanObject || !levelGrid || !pacmanMove)
    {
        return pClydeGhostComponent->GetScatterTarget();
    }


    glm::ivec2 clydeCurrentTile = pClydeGhostComponent->GetCurrentGridPosition();
    if (clydeCurrentTile.x == -1 && clydeCurrentTile.y == -1)
    {
        return pClydeGhostComponent->GetScatterTarget();
    }



    glm::vec3 pacmanWorldPos_TL = pacmanObject->GetWorldPosition().GetPosition();
    float pacmanSpriteWidth = pacmanMove->GetSpriteRenderWidth();
    float pacmanSpriteHeight = pacmanMove->GetSpriteRenderHeight();
    glm::vec2 pacmanSpriteCenterWorld = {
        pacmanWorldPos_TL.x + pacmanSpriteWidth / 2.0f,
        pacmanWorldPos_TL.y + pacmanSpriteHeight / 2.0f
    };
    std::pair<int, int> pacmanGridRowCol = levelGrid->WorldToGridCoords(
        pacmanSpriteCenterWorld.x,
        pacmanSpriteCenterWorld.y
    );
    glm::ivec2 pacmanCurrentTile = { pacmanGridRowCol.second, pacmanGridRowCol.first }; // {COL, ROW}

    float distanceSqToPacman = glm::distance(glm::vec2(clydeCurrentTile), glm::vec2(pacmanCurrentTile));


    if (distanceSqToPacman > AGGRESSIVE_DISTANCE_THRESHOLD_SQ)
    {
        return pacmanCurrentTile;
    }
    else
    {
        return pClydeGhostComponent->GetScatterTarget();
    }
}
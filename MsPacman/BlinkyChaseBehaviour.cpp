#include "BlinkyChaseBehaviour.h"

// Game
#include "BaseGhostComponent.h"
#include "GameObject.h"          
#include "PlayerMovementComponent.h"
#include "LevelGridComponent.h"  

glm::ivec2 BlinkyChaseBehaviour::CalculateChaseTarget(const BaseGhostComponent* pGhostComponent) const
{
    if (!pGhostComponent) 
    {
        return { -1, -1 }; 
    }

    auto pacmanObject = pGhostComponent->GetPacmanObject();
    LevelGridComponent* levelGrid = pGhostComponent->GetLevelGrid(); 

    if (pacmanObject && levelGrid)
    {
        glm::vec3 pacmanWorldPos_TL = pacmanObject->GetWorldPosition().GetPosition(); 

        PlayerMovementComponent* pacmanMove = pGhostComponent->GetPacmanMovement();
        if (pacmanMove)
        {
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

            return { pacmanGridRowCol.second, pacmanGridRowCol.first }; // {COLUMN, ROW}
        }
        else
        {
            std::pair<int, int> pacmanGridRowCol = levelGrid->WorldToGridCoords(pacmanWorldPos_TL.x, pacmanWorldPos_TL.y);

            return { pacmanGridRowCol.second, pacmanGridRowCol.first };
        }
    }

    return pGhostComponent->GetScatterTarget();
}
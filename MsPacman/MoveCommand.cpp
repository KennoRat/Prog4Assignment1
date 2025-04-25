#include "MoveCommand.h"
#include "GameObject.h"
#include "PlayerMovementComponent.h"

using namespace dae;

MoveCommand::MoveCommand(std::shared_ptr<GameObject> gameObject, Direction moveDirection)
    : GameObjectCommand(gameObject.get()),
    m_direction(moveDirection)
{
}

void MoveCommand::Execute()
{
    auto* gameObject = GetGameObject();

    auto* movementComponent = gameObject->GetComponent<PlayerMovementComponent>();
    if (movementComponent)
    {
        movementComponent->SetDesiredDirection(m_direction);
    }
}


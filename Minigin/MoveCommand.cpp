#include "MoveCommand.h"
#include "GameObject.h"

using namespace dae;

MoveCommand::MoveCommand(std::shared_ptr<GameObject> gameObject, Direction moveDirection)
	: GameObjectCommand(gameObject.get()), m_direction(moveDirection), m_moveComponent(nullptr)
{
	if (GetGameObject()->CheckComponent<MovementComponent>())
	{
		m_moveComponent = GetGameObject()->GetComponent<MovementComponent>();
	}
}

void MoveCommand::Execute()
{
	if (!m_moveComponent) return;
	m_moveComponent->Move(m_direction);
}


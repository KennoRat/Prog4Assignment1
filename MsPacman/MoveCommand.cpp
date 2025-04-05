#include "MoveCommand.h"
#include "GameObject.h"
#include "TimeGameEngine.h"

using namespace dae;

MoveCommand::MoveCommand(std::shared_ptr<GameObject> gameObject, Direction moveDirection, float speed)
	: GameObjectCommand(gameObject.get()), m_direction(moveDirection), m_speed(speed)
{
}

void MoveCommand::Execute()
{
    auto deltaTime = Time::GetInstance().GetDeltaTime();
    auto pos = GetGameObject()->GetWorldPosition().GetPosition();

    switch (m_direction)
    {
    case Direction::Left:
        pos.x -= static_cast<float>(m_speed * deltaTime);
        break;
    case Direction::Right:
        pos.x += static_cast<float>(m_speed * deltaTime);
        break;
    case Direction::Up:
        pos.y -= static_cast<float>(m_speed * deltaTime);
        break;
    case Direction::Down:
        pos.y += static_cast<float>(m_speed * deltaTime);
        break;
    }

    GetGameObject()->SetLocalPosition(pos.x, pos.y);
}


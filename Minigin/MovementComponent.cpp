#include "MovementComponent.h"
#include "MoveCommand.h"
#include "GameObject.h"

using namespace dae;

MovementComponent::MovementComponent(std::shared_ptr<GameObject> gameObject, float speed)
	: BaseComponent(*gameObject.get()), m_speed{speed}
{

}

void MovementComponent::Update()
{
	//GetGameObject()->SetLocalPosition(updated x_position, y_position);
}

void MovementComponent::Render() const
{
	// Nothing to render
}

void MovementComponent::RenderImGui()
{
	// Nothing
}

void MovementComponent::Move(Direction moveDirection)
{
    auto pos = GetGameObject()->GetWorldPosition().GetPosition();

    switch (moveDirection)
    {
    case Direction::Left:
        pos.x -= m_speed;
        break;
    case Direction::Right:
        pos.x += m_speed;
        break;
    case Direction::Up:
        pos.y -= m_speed;
        break;
    case Direction::Down:
        pos.y += m_speed;
        break;
    }

    GetGameObject()->SetLocalPosition(pos.x, pos.y);
}

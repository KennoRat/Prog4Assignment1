#pragma once

#include "Command.h"
#include "MovementComponent.h"

namespace dae
{
	enum class Direction
	{
		Left,
		Right,
		Up,
		Down
	};

	class MoveCommand : public GameObjectCommand
	{
	public:
		MoveCommand(std::shared_ptr<GameObject> gameObject, Direction moveDirection);

		virtual void Execute() override;

	private:
		Direction m_direction;
		MovementComponent* m_moveComponent;
	};
}

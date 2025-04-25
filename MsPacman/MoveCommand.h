#pragma once

#include "Command.h"
#include <memory>

namespace dae
{
	enum class Direction
	{
		Left,
		Right,
		Up,
		Down,
		Idle
	};

	class MoveCommand : public GameObjectCommand
	{
	public:
		MoveCommand(std::shared_ptr<GameObject> gameObject, Direction moveDirection);

		virtual void Execute() override;

	private:
		Direction m_direction;
	};
}

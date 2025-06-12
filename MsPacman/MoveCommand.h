#pragma once

#include "Command.h"
#include <memory>

namespace dae
{
	enum class Direction
	{
		Right = 0,
		Left = 1,
		Up = 2,
		Down = 3,
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

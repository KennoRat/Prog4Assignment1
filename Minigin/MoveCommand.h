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
		Down
	};

	class MoveCommand : public GameObjectCommand
	{
	public:
		MoveCommand(std::shared_ptr<GameObject> gameObject, Direction moveDirection, float speed = 1);

		virtual void Execute() override;

	private:
		Direction m_direction;
		float m_speed;
	};
}

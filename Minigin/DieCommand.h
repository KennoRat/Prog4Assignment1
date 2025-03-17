#pragma once

#include "Command.h"
#include <memory>

namespace dae
{
	class DieCommand final : public GameObjectCommand
	{
	public:
		DieCommand(std::shared_ptr<GameObject> gameObject);

		virtual void Execute() override;
	};
}
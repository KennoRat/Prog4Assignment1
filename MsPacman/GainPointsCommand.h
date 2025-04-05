#pragma once

#include "Command.h"
#include <memory>

namespace dae
{
	class GainPointsCommand final : public GameObjectCommand
	{
	public:
		GainPointsCommand(std::shared_ptr<GameObject> gameObject);

		virtual void Execute() override;
	};
}



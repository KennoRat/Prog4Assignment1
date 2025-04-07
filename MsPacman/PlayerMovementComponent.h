#pragma once
#include "BaseComponent.h"

enum class Direction
{
	Left,
	Right,
	Up,
	Down
};

class PlayerMovementComponent final: public dae::BaseComponent
{
public:
	// Constructor
	PlayerMovementComponent(std::shared_ptr<dae::GameObject> gameObject);

	//Destructor
	virtual ~PlayerMovementComponent() = default;

	// Rule Of Five
	PlayerMovementComponent(const PlayerMovementComponent& other) = delete;
	PlayerMovementComponent(PlayerMovementComponent&& other) = delete;
	PlayerMovementComponent& operator=(const PlayerMovementComponent& other) = delete;
	PlayerMovementComponent& operator=(PlayerMovementComponent&& other) = delete;

private:

};


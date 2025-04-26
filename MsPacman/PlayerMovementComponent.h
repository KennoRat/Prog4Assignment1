#pragma once
#include "BaseComponent.h"
#include "LevelGridComponent.h" // Include necessary header
#include "MoveCommand.h"
#include <glm.hpp>
#include <memory>

// Forward declare
namespace dae { class ScoreComponent; }

class PlayerMovementComponent final: public dae::BaseComponent
{
public:
	// Scores
	static constexpr int PELLET_SCORE{ 10 };
	static constexpr int POWER_PELLET_SCORE{ 50 };

	// Constructor
	PlayerMovementComponent(std::shared_ptr<dae::GameObject> gameObject, std::shared_ptr<dae::GameObject> levelObject, float speed);

	// Destructor
	virtual ~PlayerMovementComponent() = default;

	// Rule Of Five
	PlayerMovementComponent(const PlayerMovementComponent& other) = delete;
	PlayerMovementComponent(PlayerMovementComponent&& other) = delete;
	PlayerMovementComponent& operator=(const PlayerMovementComponent& other) = delete;
	PlayerMovementComponent& operator=(PlayerMovementComponent&& other) = delete;

	virtual void Update() override;
	virtual void Render() const override;
	virtual void RenderImGui() override;

	// Called by MoveCommand
	void SetDesiredDirection(dae::Direction dir);

	// Place player at a specific grid cell
	void SnapToGrid(int row, int col);

private:

	// Methods
	void Initialize();
	void HandleTileReached(int row, int col);
	bool CanMove(int startRow, int startCol, dae:: Direction dir) const;
	bool IsNearTarget(float tolerance = 1.0f) const;
	glm::vec2 GetTileCenter(int row, int col) const;
	glm::vec2 GetDirectionVector(dae::Direction dir) const;

	// Data
	std::shared_ptr<dae::GameObject> m_pLevelObject;
	LevelGridComponent* m_pLevelGridCache{ nullptr }; // Cache for efficiency
	dae::ScoreComponent* m_pScoreComponentCache{ nullptr };
	float m_speed;
	dae::Direction m_currentDirection{ dae::Direction::Left };
	dae::Direction m_desiredDirection{ dae::Direction::Left };
	glm::vec2 m_targetPosition{};
	bool m_isMoving{ false };
	bool m_initialized{ false };

	const float m_tileCenterTolerance{ 1.5f }; // Pixels distance to allow turning
};


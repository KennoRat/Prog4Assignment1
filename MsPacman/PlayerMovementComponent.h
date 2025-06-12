#pragma once
#include "BaseComponent.h"
#include "LevelGridComponent.h"
#include "MoveCommand.h"
#include <glm.hpp>
#include <memory>

// Forward declare
namespace dae 
{
	class ScoreComponent;
	class RenderComponent;
}

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

	void SetDesiredDirection(dae::Direction dir);
	dae::Direction GetCurrentDirection() const { return m_currentDirection; }
	bool IsCurrentlyMoving() const { return m_isMoving; }
	void SnapToGrid(int row, int col);
	float GetSpriteRenderWidth() const { return m_spriteRenderWidth; }
	float GetSpriteRenderHeight() const { return m_spriteRenderHeight; }

private:

	// Methods
	void Initialize();
	void HandleTileReached(int row, int col);
	bool CanMove(int startRow, int startCol, dae:: Direction dir) const;
	bool IsNearTarget(float tolerance = 1.0f) const;
	glm::vec2 GetTileCenter(int row, int col) const;
	glm::vec2 GetDirectionVector(dae::Direction dir) const;
	glm::vec2 AdjustPositionForSpriteCenter(glm::vec2 targetCenterPos) const;

	// Data
	dae::RenderComponent* m_pRenderComponentCache{ nullptr };
	std::shared_ptr<dae::GameObject> m_pLevelObject;
	LevelGridComponent* m_pLevelGridCache{ nullptr };
	dae::ScoreComponent* m_pScoreComponentCache{ nullptr };
	dae::Direction m_currentDirection{ dae::Direction::Left };
	dae::Direction m_desiredDirection{ dae::Direction::Left };
	glm::vec2 m_targetPosition{};
	float m_speed{};
	float m_spriteRenderWidth{ 0.f };
	float m_spriteRenderHeight{ 0.f };
	bool m_isMoving{ false };
	bool m_initialized{ false };

	const float m_tileCenterTolerance{ 1.5f }; // Pixels distance to allow turning
};


#pragma once
#include <string>
#include <glm.hpp>
#include <vector>
#include "BaseComponent.h"

enum class TileType
{
    Path = 0,
    Wall = 1,
    Pellet = 2,
	PowerPellet = 3,
	GhostSpawn = 4,
	Empty = -1
};

class LevelGridComponent final : public dae::BaseComponent
{
public:
	// Constructor
	LevelGridComponent(std::shared_ptr<dae::GameObject> gameObject, float tileSize = 15.0f);

	//Destructor
	virtual ~LevelGridComponent() = default;

	// Rule Of Five
	LevelGridComponent(const LevelGridComponent& other) = delete;
	LevelGridComponent(LevelGridComponent&& other) = delete;
	LevelGridComponent& operator=(const LevelGridComponent& other) = delete;
	LevelGridComponent& operator=(LevelGridComponent&& other) = delete;

	virtual void Update() override;
	virtual void Render() const override;
	virtual void RenderImGui() override;

	void LoadLevel(const std::string& filepath);
	TileType GetTileType(int row, int col) const;
	bool IsWall(int row, int col) const;
	std::pair<int, int> WorldToGridCoords(float worldX, float worldy) const;
	glm::vec2 GridToWorldCoords(int row, int col) const;

	float GetTileSize() const { return m_tileSize; }
	int GetRows() const { return static_cast<int>(m_gridRows); }
	int GetCols() const { return static_cast<int>(m_gridCols); }

private:

	std::vector<std::vector<TileType>> m_mapGrid{};
	int m_gridCols{};
	int m_gridRows{};
	float m_tileSize{};
};

//Gridsize of a level in Ms.Pacman is 28x31
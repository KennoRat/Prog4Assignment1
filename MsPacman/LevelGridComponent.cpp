#include "LevelGridComponent.h"
#include <fstream> 
#include <sstream> 
#include <iostream> 
#include <stdexcept> 
#include "Renderer.h"
#include "GameObject.h"

LevelGridComponent::LevelGridComponent(std::shared_ptr<dae::GameObject> gameObject, float tileSize):
    dae::BaseComponent(*gameObject),
    m_gridCols(0),
    m_gridRows(0),
    m_tileSize(tileSize)
{
    if (m_tileSize <= 0) 
    {
        m_tileSize = 5.f;
    };
}

void LevelGridComponent::Update()
{
    // Do nothing
}

void LevelGridComponent::Render() const
{
    if (m_mapGrid.empty() || m_tileSize <= 0) return; // Nothing to render

    auto& renderer = dae::Renderer::GetInstance();
    SDL_Renderer* sdlRenderer = renderer.GetSDLRenderer();

    SDL_Rect tileRect;
    tileRect.w = static_cast<int>(m_tileSize);
    tileRect.h = static_cast<int>(m_tileSize);

    const SDL_Color wallColor = { 0, 0, 255, 255 };     // Blue
    const SDL_Color wallBackColor = { 42, 67, 100, 255 }; // Dark Blue
    const SDL_Color pelletColor = { 255, 255, 255, 255 };   // White
    const SDL_Color powerPelletColor = { 255, 255, 0, 255 }; // Yellow
    const SDL_Color pathColor = { 0, 0, 0, 255 }; // Black
    const SDL_Color ghostSpawnColor = { 100, 100, 100, 255 }; // Grey

    for (int row = 0; row < m_gridRows; ++row) {
        for (int col = 0; col < m_gridCols; ++col) {

            TileType type = m_mapGrid[row][col];
            if (type == TileType::Empty) continue;

            // Calculate screen position for the top-left corner of the tile
            glm::vec2 screenPos = GridToWorldCoords(row, col);
            tileRect.x = static_cast<int>(screenPos.x);
            tileRect.y = static_cast<int>(screenPos.y);

            switch (type) {
            case TileType::Wall:

                // Draw background
                SDL_SetRenderDrawColor(sdlRenderer, wallBackColor.r, wallBackColor.g, wallBackColor.b, wallBackColor.a);
                SDL_RenderFillRect(sdlRenderer, &tileRect);

                // Draw outlune
                SDL_SetRenderDrawColor(sdlRenderer, wallColor.r, wallColor.g, wallColor.b, wallColor.a);
                SDL_RenderDrawRect(sdlRenderer, &tileRect);

                break;

            case TileType::Pellet:

                // Draw background
                SDL_SetRenderDrawColor(sdlRenderer, pathColor.r, pathColor.g, pathColor.b, pathColor.a);
                SDL_RenderFillRect(sdlRenderer, &tileRect);

                SDL_SetRenderDrawColor(sdlRenderer, pelletColor.r, pelletColor.g, pelletColor.b, pelletColor.a);

                // Make pellet smaller than the tile
                SDL_Rect pelletRect;
                pelletRect.w = static_cast<int>(m_tileSize * 0.25f);
                pelletRect.h = static_cast<int>(m_tileSize * 0.25f);

                // Center it within the tile
                pelletRect.x = tileRect.x + (tileRect.w - pelletRect.w) / 2;
                pelletRect.y = tileRect.y + (tileRect.h - pelletRect.h) / 2;
                SDL_RenderFillRect(sdlRenderer, &pelletRect);
                break;

            case TileType::PowerPellet:

                // Draw background
                SDL_SetRenderDrawColor(sdlRenderer, pathColor.r, pathColor.g, pathColor.b, pathColor.a);
                SDL_RenderFillRect(sdlRenderer, &tileRect);

                SDL_SetRenderDrawColor(sdlRenderer, powerPelletColor.r, powerPelletColor.g, powerPelletColor.b, powerPelletColor.a);

                // Make power pellet larger than regular pellet
                SDL_Rect powerPelletRect;
                powerPelletRect.w = static_cast<int>(m_tileSize * 0.5f);
                powerPelletRect.h = static_cast<int>(m_tileSize * 0.5f);

                // Center it
                powerPelletRect.x = tileRect.x + (tileRect.w - powerPelletRect.w) / 2;
                powerPelletRect.y = tileRect.y + (tileRect.h - powerPelletRect.h) / 2;
                SDL_RenderFillRect(sdlRenderer, &powerPelletRect);
                break;

            case TileType::Path:

                SDL_SetRenderDrawColor(sdlRenderer, pathColor.r, pathColor.g, pathColor.b, pathColor.a);
                SDL_RenderFillRect(sdlRenderer, &tileRect);

                break;

            case TileType::GhostSpawn:

                SDL_SetRenderDrawColor(sdlRenderer, ghostSpawnColor.r, ghostSpawnColor.g, ghostSpawnColor.b, ghostSpawnColor.a);
                SDL_RenderFillRect(sdlRenderer, &tileRect);
                break;

            default:
                break;
            }
        }
    }
}

void LevelGridComponent::RenderImGui()
{
  // Do nothing
}

void LevelGridComponent::LoadLevel(const std::string& filepath)
{
    std::ifstream inputFile(filepath); // Open the CSV file

    if (!inputFile.is_open()) {
        throw std::runtime_error("Error: Could not open level file: " + filepath);
    }

    std::string line;
    int rowNum = 0;
    while (std::getline(inputFile, line)) { // Read one line at a time
        if (line.empty()) continue; // Skip empty lines

        std::vector<TileType> row;
        std::stringstream ss(line);
        std::string segment;
        int colNum = 0;

        while (std::getline(ss, segment, ';')) { 
            try {
                int tileValue = std::stoi(segment); // Convert string segment to integer
                row.push_back(static_cast<TileType>(tileValue));
            }
            catch (const std::out_of_range&) {
                std::cerr << "Warning: Number out of range '" << segment << "' at row " << rowNum + 1 << ", col " << colNum + 1 << std::endl;
                row.push_back(TileType::Empty);
            }
            colNum++;
        }

        if (rowNum == 0) {
            m_gridCols = colNum; // Set number of collumns based on the first row
        }

        m_mapGrid.push_back(row); 
        rowNum++;
    }

    
    m_gridRows = rowNum; // Total rows

    inputFile.close();

    if (m_gridCols == 0 || m_gridRows == 0) {
        throw std::runtime_error("Error: Level file is empty or invalid: " + filepath);
    }

    std::cout << "Level loaded: " << m_gridCols << "x" << m_gridRows << std::endl;
}

TileType LevelGridComponent::GetTileType(int row, int col) const
{
    // Check if inside grid<b
    if (row < 0 || row >= m_gridRows || col < 0 || col >= m_gridCols) {
        return TileType::Empty;
    }
    return m_mapGrid[row][col];
}

bool LevelGridComponent::IsWall(int row, int col) const
{
    return GetTileType(row, col) == TileType::Wall;
}

std::pair<int, int> LevelGridComponent::WorldToGridCoords(float worldX, float worldY) const
{
    const auto& pos = GetGameObject()->GetWorldPosition().GetPosition();

    float relativeX = worldX - pos.x;
    float relativeY = worldY - pos.y;

    int col = static_cast<int>(std::floor(relativeX / m_tileSize));
    int row = static_cast<int>(std::floor(relativeY / m_tileSize));

    return { row, col };
}

glm::vec2 LevelGridComponent::GridToWorldCoords(int row, int col) const
{
    const auto& pos = GetGameObject()->GetWorldPosition().GetPosition();

    float worldX = pos.x + col * m_tileSize;
    float worldY = pos.y + row * m_tileSize;

    return { worldX, worldY };
}



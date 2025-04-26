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

                // Make pellet and center
                SDL_Rect pelletRect;
                pelletRect.w = static_cast<int>(m_tileSize * 0.25f);
                pelletRect.h = static_cast<int>(m_tileSize * 0.25f);
                pelletRect.x = tileRect.x + (tileRect.w - pelletRect.w) / 2;
                pelletRect.y = tileRect.y + (tileRect.h - pelletRect.h) / 2;
                SDL_RenderFillRect(sdlRenderer, &pelletRect);
                break;

            case TileType::PowerPellet:

                // Draw background
                SDL_SetRenderDrawColor(sdlRenderer, pathColor.r, pathColor.g, pathColor.b, pathColor.a);
                SDL_RenderFillRect(sdlRenderer, &tileRect);

                SDL_SetRenderDrawColor(sdlRenderer, powerPelletColor.r, powerPelletColor.g, powerPelletColor.b, powerPelletColor.a);

                // Make power pellet and center it
                SDL_Rect powerPelletRect;
                powerPelletRect.w = static_cast<int>(m_tileSize * 0.5f);
                powerPelletRect.h = static_cast<int>(m_tileSize * 0.5f);
                powerPelletRect.x = tileRect.x + (tileRect.w - powerPelletRect.w) / 2;
                powerPelletRect.y = tileRect.y + (tileRect.h - powerPelletRect.h) / 2;
                SDL_RenderFillRect(sdlRenderer, &powerPelletRect);
                break;

            case TileType::GhostSpawn:

                SDL_SetRenderDrawColor(sdlRenderer, ghostSpawnColor.r, ghostSpawnColor.g, ghostSpawnColor.b, ghostSpawnColor.a);
                SDL_RenderFillRect(sdlRenderer, &tileRect);
                break;

            case TileType::Path:
            case TileType::TP1_Left:
            case TileType::TP1_Right:
            case TileType::TP2_Left:
            case TileType::TP2_Right:
            default:

                SDL_SetRenderDrawColor(sdlRenderer, pathColor.r, pathColor.g, pathColor.b, pathColor.a);
                SDL_RenderFillRect(sdlRenderer, &tileRect);

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
    // Clear everything
    m_mapGrid.clear();
    m_gridCols = 0;
    m_gridRows = 0;
    m_tp1LeftCoord = std::pair(0, 0);
    m_tp1RightCoord = std::pair(0, 0);
    m_tp2LeftCoord = std::pair(0, 0);
    m_tp2RightCoord = std::pair(0, 0);

    // Open the CSV file
    std::ifstream inputFile(filepath);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Error: Could not open level file: " + filepath);
    }

    std::string line;
    int rowNum = 0;
    while (std::getline(inputFile, line)) {
        // Skip empty lines
        if (line.empty()) continue; 

        std::vector<TileType> rowVector;
        std::stringstream ss(line);
        std::string segment;
        int colNum = 0;

        while (std::getline(ss, segment, ';')) { 
            // Convert string segment to integer
            try {
                int tileValue = std::stoi(segment); 
                rowVector.push_back(static_cast<TileType>(tileValue));
            }
            catch (const std::out_of_range&) {
                std::cerr << "Warning: Number out of range '" << segment << "' at row " << rowNum + 1 << ", col " << colNum + 1 << std::endl;
                rowVector.push_back(TileType::Empty);
            }
            colNum++;
        }

        // Set number of collumns based on the first row
        if (rowNum == 0) {
            m_gridCols = colNum; 
        }

        m_mapGrid.push_back(rowVector);
        rowNum++;
    }

    
    m_gridRows = rowNum; // Total rows

    inputFile.close();

    if (m_gridCols == 0 || m_gridRows == 0) {
        throw std::runtime_error("Error: Level file is empty or invalid: " + filepath);
    }

    FindAndStoreTunnelEndpoints();

    std::cout << "Level loaded: " << m_gridCols << "x" << m_gridRows << std::endl;
}

void LevelGridComponent::ClearTile(int row, int col)
{
    // Check if Pacman eats pellet
    if (row >= 0 && row < m_gridRows && col >= 0 && col < m_gridCols)
    {
        if (m_mapGrid[row][col] == TileType::Pellet || m_mapGrid[row][col] == TileType::PowerPellet)
        {
            m_mapGrid[row][col] = TileType::Path;
        }
    }
}

TileType LevelGridComponent::GetTileType(int row, int col) const
{
    // Check if inside grid<b
    if (row < 0 || row >= m_gridRows || col < 0 || col >= m_gridCols) 
    {
        if (row >= 0 && row < m_gridRows) // Check row is valid
        {
            // Check wrapping
            if (col == 0 && m_tp1LeftCoord.first && row == m_tp1LeftCoord.second) return TileType::TP1_Right; 
            else if (col == m_gridCols - 1 && m_tp1RightCoord.first && row == m_tp1RightCoord.second) return TileType::TP1_Left; 
            else if (col == 0 && m_tp2LeftCoord.first && row == m_tp2LeftCoord.second) return TileType::TP2_Right;
            else if (col == m_gridCols - 1 && m_tp2RightCoord.first && row == m_tp2RightCoord.second) return TileType::TP2_Left;
        }
        return TileType::Empty;
    }
    return m_mapGrid[row][col];
}

bool LevelGridComponent::IsWall(int row, int col) const
{
    TileType type = GetTileType(row, col);
    return type == TileType::Wall || type == TileType::Empty || type == TileType::GhostSpawn;
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

std::pair<int, int> LevelGridComponent::GetTeleportDestination(TileType entranceType) const
{
    if(entranceType == TileType::TP1_Left)
    {
        return m_tp1RightCoord;
    }
    else if (entranceType == TileType::TP1_Right)
    {
        return m_tp1LeftCoord;
    }
    else if (entranceType == TileType::TP2_Left)
    {
        return m_tp2RightCoord;
    }
    else if (entranceType == TileType::TP2_Right)
    {
        return m_tp2LeftCoord;
    }

    return std::pair(0, 0);
}

glm::vec2 LevelGridComponent::GridToWorldCoords(int row, int col) const
{
    const auto& pos = GetGameObject()->GetWorldPosition().GetPosition();

    float worldX = pos.x + col * m_tileSize;
    float worldY = pos.y + row * m_tileSize;

    return { worldX, worldY };
}

void LevelGridComponent::FindAndStoreTunnelEndpoints()
{
    for (int r = 0; r < m_gridRows; ++r)
    {
        for (int c = 0; c < m_gridCols; ++c)
        {
            switch (m_mapGrid[r][c])
            {
            case TileType::TP1_Left:  m_tp1LeftCoord = std::make_pair(r, c); break;
            case TileType::TP1_Right: m_tp1RightCoord = std::make_pair(r, c); break;
            case TileType::TP2_Left:  m_tp2LeftCoord = std::make_pair(r, c); break;
            case TileType::TP2_Right: m_tp2RightCoord = std::make_pair(r, c); break;
            default: break;
            }
        }
    }
}



#pragma once

#include "GameState.h"
#include "Observer.h" 
#include <memory>
#include <string>
#include <vector>

namespace dae
{
    class Scene;
    class GameObject;
}

class LevelGridComponent;

class PlayingState final : public dae::GameState, public dae::Observer
{
public:
    // Constructor and Destructor
    PlayingState(int initialLevelIndex = 0);
    virtual ~PlayingState() override;

    void OnEnter(dae::GameStateMachine* pStateMachine) override;
    void OnExit(dae::GameStateMachine* pStateMachine) override;
    void ResetKeybindings() override;

    dae::StateTransition HandleInput() override;
    dae::StateTransition Update(float deltaTime) override;
    void Render() const override;
    void SignalGoNextLevel() { m_goNextLevel = true; }

    virtual void Notify(const dae::GameObject& gameObject, const dae::Event& event) override;

private:

    std::shared_ptr<dae::Scene> m_pGameScene; 
    std::shared_ptr<dae::GameObject> m_pMissPacman;
    std::shared_ptr<dae::GameObject> m_pGhostManager;
    std::shared_ptr<dae::GameObject> m_pLevelObject;
    LevelGridComponent* m_pLevelGridComponentCache{ nullptr };

    bool m_gameSceneLoaded{ false };
    bool m_isPlayerRespawning{ false };
    bool m_goNextLevel{ false };
    float m_playerRespawnTimer{ 0.f };
    static constexpr float PLAYER_RESPAWN_DELAY = 2.0f;

    int m_currentLevelIndex{ 0 };
    std::vector<std::string> m_levelFilePaths;

    void InitializeLevelFilePaths();
    void LoadGameSceneForLevel(int levelIndex);
    void UnloadCurrentGameScene();
    void ResetPlayerForRespawn();
    void ResetForNewLevel();
};
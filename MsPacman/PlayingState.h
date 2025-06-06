#pragma once

#include "GameState.h"
#include <memory>

namespace dae
{
    class Scene;
    class GameObject;
}

class PlayingState final : public dae::GameState
{
public:
    // Constructor and Destructor
    PlayingState();
    virtual ~PlayingState() override;

    void OnEnter(dae::GameStateMachine* pStateMachine) override;
    void OnExit(dae::GameStateMachine* pStateMachine) override;

    dae::StateTransition HandleInput() override;
    dae::StateTransition Update(float deltaTime) override;
    void Render() const override;

private:

    std::shared_ptr<dae::Scene> m_pGameScene; 
    bool m_GameSceneLoaded{ false };

    void LoadGameScene(); 
    void UnloadGameScene(); 
};
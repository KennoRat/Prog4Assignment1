#pragma once

#include "GameState.h"
#include <memory>   
#include <iostream> 
#include <vector>

namespace dae
{
    class SceneManager;
    class InputManager;

    class GameStateMachine final
    {
    public:
        // Constructor and Destructor
        GameStateMachine();
        ~GameStateMachine();

        // Rule of 5
        GameStateMachine(const GameStateMachine&) = delete;
        GameStateMachine(GameStateMachine&&) = delete;
        GameStateMachine& operator=(const GameStateMachine&) = delete;
        GameStateMachine& operator=(GameStateMachine&&) = delete;

        void ProcessTransition(StateTransition& transition);
        void SetState(std::unique_ptr<GameState> newState);
        void PushState(std::unique_ptr<GameState> newState);
        void PopState();

        void HandleInput();
        void Update(float deltaTime);
        void Render() const;

        GameState* GetCurrentState() const 
        { 
            if (m_States.empty()) return nullptr;
            return m_States.back().get();
        }
        void RequestQuit() { m_WantsToQuit = true; }
        bool WantsToQuit() const { return m_WantsToQuit; }
        bool IsStackEmpty() const { return m_States.empty(); }

    private:

        std::vector<std::unique_ptr<GameState>> m_States;
        bool m_WantsToQuit{ false };
    };
}
#pragma once

#include <memory> 

namespace dae
{
    class GameStateMachine;
    class GameState;

    enum class TransitionType
    {
        None,   // No transition
        Set,    // Replace the entire state stack
        Push,   // Push the new state on top
        Pop     // Pop the current state
    };

    struct StateTransition
    {
        TransitionType type = TransitionType::None;
        std::unique_ptr<GameState> nextState = nullptr; 

        // Helper constructors
        StateTransition() = default;
        explicit StateTransition(TransitionType transition) : type(transition) {}
        StateTransition(TransitionType transition, std::unique_ptr<GameState> state) : type(transition), nextState(std::move(state)) {}
    };


    class GameState
    {
    public:

        virtual ~GameState() = default;

        virtual void OnEnter(GameStateMachine* pStateMachine) = 0;
        virtual void OnExit(GameStateMachine* pStateMachine) = 0;
        virtual void ResetKeybindings() = 0;

        virtual StateTransition HandleInput() = 0;
        virtual StateTransition Update(float deltaTime) = 0;
        virtual void Render() const = 0;

    protected:

        GameStateMachine* GetStateMachine() const { return m_pStateMachine; }

    private:

        GameStateMachine* m_pStateMachine{ nullptr };
        friend class GameStateMachine;
    };
}
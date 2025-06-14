#include "GameStateMachine.h"
#include "CollisionManager.h"
#include <cassert> 

namespace dae
{
    GameStateMachine::GameStateMachine()
    {
        std::cout << "GameStateMachine created." << std::endl;
    }

    GameStateMachine::~GameStateMachine()
    {
        std::cout << "GameStateMachine destroyed. Clearing states..." << std::endl;

        while (!m_States.empty()) 
        {
            m_States.back()->OnExit(this);
            m_States.pop_back();
        }
    }

    void GameStateMachine::SetState(std::unique_ptr<GameState> newState)
    {
        // Exit all current states
        while (!m_States.empty()) 
        {
            m_States.back()->OnExit(this);
            m_States.pop_back();
        }

        if (newState)
        {
            newState->m_pStateMachine = this; 
            newState->OnEnter(this);
            m_States.push_back(std::move(newState));
            std::cout << "GameStateMachine: Set new state." << std::endl;
        }
    }

    void GameStateMachine::PushState(std::unique_ptr<GameState> newState)
    {
        if (newState)
        {
            newState->m_pStateMachine = this;
            newState->OnEnter(this);
            m_States.push_back(std::move(newState));
            std::cout << "GameStateMachine: Pushed new state." << std::endl;
        }
    }

    void GameStateMachine::PopState()
    {
        if (!m_States.empty())
        {
            m_States.back()->OnExit(this);
            m_States.pop_back();
            m_States.back()->ResetKeybindings();
            std::cout << "GameStateMachine: Popped state." << std::endl;
        }
    }

    void GameStateMachine::HandleInput()
    {
        if (m_States.empty()) return;

        StateTransition transition = m_States.back()->HandleInput();
        ProcessTransition(transition);
    }

    void GameStateMachine::Update(float deltaTime)
    {
        if (m_States.empty()) return;

        StateTransition transition = m_States.back()->Update(deltaTime);
        ProcessTransition(transition);
    }

    void GameStateMachine::Render() const
    {
        if (m_States.empty()) return;
        
        m_States.back()->Render();
    }

    void GameStateMachine::ProcessTransition(StateTransition& transition)
    {
        switch (transition.type)
        {
        case TransitionType::Set:

            SetState(std::move(transition.nextState));

            break;
        case TransitionType::Push:

            PushState(std::move(transition.nextState));

            break;
        case TransitionType::Pop:

            PopState();

            break;
        case TransitionType::None:
        default:
 
            break;
        }
    }
}
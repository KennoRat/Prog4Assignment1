#pragma once
#include "Command.h" 

// Forward declarations
namespace dae 
{ 
    class GameStateMachine;
    class GameState;
}

class UICommandBase : public dae::Command
{
public:

    UICommandBase(dae::GameStateMachine* gsm) : m_pGameStateMachine(gsm) {}
    virtual ~UICommandBase() = default;

protected:

    dae::GameStateMachine* GetGameStateMachine() const { return m_pGameStateMachine; }
    dae::GameState* GetCurrentGameState() const;

private:

    dae::GameStateMachine* m_pGameStateMachine;
};
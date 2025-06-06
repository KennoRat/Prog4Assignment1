#include "UICommandBase.h"
#include "GameStateMachine.h"
#include "GameState.h"

dae::GameState* UICommandBase::GetCurrentGameState() const
{
    if (m_pGameStateMachine && !m_pGameStateMachine->IsStackEmpty())
    {
        return m_pGameStateMachine->GetCurrentState();
    }
    return nullptr;
}

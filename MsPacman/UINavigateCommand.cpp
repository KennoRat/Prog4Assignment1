#include "UINavigateCommand.h"
#include "GameState.h"    
#include <iostream>

// Game States
#include "MenuState.h"    
#include "PausedState.h"

UINavigateCommand::UINavigateCommand(dae::GameStateMachine* gsm, NavigationDirection direction)
    : UICommandBase(gsm), m_Direction(direction)
{
}

void UINavigateCommand::Execute()
{
    dae::GameState* currentState = GetCurrentGameState();
    if (!currentState) return;

    if (auto* menuState = dynamic_cast<MenuState*>(currentState)) 
    {
        if (m_Direction == NavigationDirection::Down) menuState->SelectNextOption();
        else if (m_Direction == NavigationDirection::Up) menuState->SelectPreviousOption();
        std::cout << "UINavigateCommand executed for MenuState" << std::endl;
    }
    else if (auto* pausedState = dynamic_cast<PausedState*>(currentState))
    {
        if (m_Direction == NavigationDirection::Down) pausedState->SelectNextOption();
        else if (m_Direction == NavigationDirection::Up) pausedState->SelectPreviousOption();
        std::cout << "UINavigateCommand executed for PausedState" << std::endl;
    }
}
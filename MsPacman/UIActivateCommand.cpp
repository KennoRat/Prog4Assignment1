#include "UIActivateCommand.h"
#include "GameState.h"
#include "GameStateMachine.h"
#include "SceneManager.h"
#include <iostream>

// Game States
#include "MenuState.h"
#include "PausedState.h"
#include "PlayingState.h"

UIActivateCommand::UIActivateCommand(dae::GameStateMachine* gsm)
    : UICommandBase(gsm)
{
}

void UIActivateCommand::Execute()
{
    dae::GameState* currentState = GetCurrentGameState();
    if (!currentState) return;

    dae::GameStateMachine* gsm = GetGameStateMachine();
    if (!gsm) return;

    if (auto* menuState = dynamic_cast<MenuState*>(currentState)) 
    {
        MenuButtons selectedOption = menuState->GetSelectedOptionIndex(); 
        if (selectedOption == MenuButtons::PLAY_BUTTON_INDEX) 
        {
            gsm->SetState(std::make_unique<PlayingState>()); 
        }
        else if (selectedOption == MenuButtons::QUIT_BUTTON_INDEX) {
            gsm->RequestQuit();
        }
    }
    else if (auto* pausedState = dynamic_cast<PausedState*>(currentState)) 
    {
        PausedButtons selectedOption = pausedState->GetSelectedOptionIndex();
        if (selectedOption == PausedButtons::RESUME_BUTTON_INDEX)
        {
            gsm->PopState();
            dae::SceneManager::GetInstance().PauseActiveScene(false);
        }
        else if (selectedOption == PausedButtons::MENU_BUTTON_INDEX)
        {
            gsm->SetState(std::make_unique<MenuState>());
            dae::SceneManager::GetInstance().PauseActiveScene(false);
        }
    }
}
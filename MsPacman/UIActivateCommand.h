#pragma once
#include "UICommandBase.h"

class MenuState; 
class PausedState; 

class UIActivateCommand final : public UICommandBase
{
public:
    UIActivateCommand(dae::GameStateMachine* gsm);
    virtual void Execute() override;
};
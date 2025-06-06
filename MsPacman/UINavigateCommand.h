#pragma once
#include "UICommandBase.h"

enum class NavigationDirection
{
    Up,
    Down,
    Left,
    Right 
};

class MenuState;
class PausedState;

class UINavigateCommand final : public UICommandBase
{
public:

    UINavigateCommand(dae::GameStateMachine* gsm, NavigationDirection direction);
    virtual void Execute() override;

private:

    NavigationDirection m_Direction;
};
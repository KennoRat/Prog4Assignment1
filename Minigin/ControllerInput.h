#pragma once

#include "Command.h"
#include <memory>


namespace dae
{
    using WORD = unsigned short; // WORD represents a button (e.g., XINPUT_GAMEPAD_A)

    enum class ControllerState
    {
        Pressed,
        Down,
        Up
    };

    struct ControllerCommand
    {
        WORD Button; 
        ControllerState State;
        std::unique_ptr<Command> command;
    };

    class ControllerInput final
    {
    public:
        ControllerInput();
        ~ControllerInput();

        void ProcessInput();
        void BindButton(WORD button, ControllerState state, std::unique_ptr<Command> command);
        void UnbindButton(WORD button);

    private:
        class ControllerImpl;
        std::unique_ptr<ControllerImpl> m_pControllerImpl;
    };
}

#include "ControllerInput.h"
#include <windows.h>
#include <Xinput.h>
#include <vector>


//Based on the code from: https://docs.microsoft.com/en-us/windows/win32/xinput/getting-started-with-xinput

using namespace dae;

class ControllerInput::ControllerImpl
{
public:
    ControllerImpl() : m_controllerIndex(0), buttonsPressedThisFrame(0), buttonsReleasedThisFrame(0)
    {
        ZeroMemory(&currentState, sizeof(XINPUT_STATE));
        ZeroMemory(&previousState, sizeof(XINPUT_STATE));
    }

    void ProcessInput();
    void UpdateKeyStates();
    void BindButton(WORD button, ControllerState state, std::unique_ptr<Command> command);
    void UnbindButton(WORD button);

private:
    XINPUT_STATE previousState{};
    XINPUT_STATE currentState{};
    int m_controllerIndex;
    WORD buttonsPressedThisFrame;
    WORD buttonsReleasedThisFrame;

    std::vector<std::unique_ptr<ControllerCommand>> m_pKeyBindings;
};

// Base class for all commands
ControllerInput::ControllerInput() : m_pControllerImpl(std::make_unique<ControllerImpl>()) {}

ControllerInput::~ControllerInput() = default;

void ControllerInput::ProcessInput()
{
    m_pControllerImpl->ProcessInput();
}

void ControllerInput::BindButton(WORD button, ControllerState state, std::unique_ptr<Command> command)
{
    m_pControllerImpl->BindButton(button, state, std::move(command));
}

void ControllerInput::UnbindButton(WORD button)
{
    m_pControllerImpl->UnbindButton(button);
}

// Implementation of the ControllerImpl class
void ControllerInput::ControllerImpl::ProcessInput()
{
    UpdateKeyStates();

    for (auto& keyCmd : m_pKeyBindings)
    {
        bool isPressed = currentState.Gamepad.wButtons & keyCmd->Button;
        bool wasPressed = previousState.Gamepad.wButtons & keyCmd->Button;

        switch (keyCmd->State)
        {
        case ControllerState::Pressed:
            if (isPressed)
                keyCmd->command->Execute();
            break;

        case ControllerState::Down:
            if (isPressed && !wasPressed)
                keyCmd->command->Execute();
            break;

        case ControllerState::Up:
            if (!isPressed && wasPressed)
                keyCmd->command->Execute();
            break;
        }
    }
}

void ControllerInput::ControllerImpl::UpdateKeyStates()
{
    // Save the previous state before updating
    CopyMemory(&previousState, &currentState, sizeof(XINPUT_STATE));
    ZeroMemory(&currentState, sizeof(XINPUT_STATE));

	// Get the current state of the controller
    if (XInputGetState(m_controllerIndex, &currentState) == ERROR_SUCCESS)
    {
        auto buttonChanges = currentState.Gamepad.wButtons ^ previousState.Gamepad.wButtons;
        buttonsPressedThisFrame = buttonChanges & currentState.Gamepad.wButtons;
        buttonsReleasedThisFrame = buttonChanges & (~currentState.Gamepad.wButtons);
    }
    else
    {
        buttonsPressedThisFrame = 0;
        buttonsReleasedThisFrame = 0;
    }
}

void ControllerInput::ControllerImpl::BindButton(WORD button, ControllerState state, std::unique_ptr<Command> command)
{
    m_pKeyBindings.emplace_back(std::make_unique<ControllerCommand>(ControllerCommand{ button, state, std::move(command) }));
}

void ControllerInput::ControllerImpl::UnbindButton(WORD button)
{
    // Find the button binding
    auto iterator = std::remove_if(m_pKeyBindings.begin(), m_pKeyBindings.end(),
        [button](const std::unique_ptr<ControllerCommand>& keyCmd)
        {
            return keyCmd->Button == button;
        });

    // Remove from the vector
    if (iterator != m_pKeyBindings.end())
    {
        m_pKeyBindings.erase(iterator, m_pKeyBindings.end());
    }
}

#include "InputManager.h"
#include "imgui.h"
#include <backends/imgui_impl_sdl2.h>
#include <iostream>

using namespace dae;

bool InputManager::ProcessInput()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            return false;
        }

        // Process events for ImGui
        ImGui_ImplSDL2_ProcessEvent(&e);
    }

	// Keyboard input
    UpdateKeyStates();

    for (auto& keyCmd : m_pKeyBindings)
    {
        bool isKeyPressed = m_CurrentKeyStates[keyCmd->Scancode];
        bool wasKeyPressed = m_PreviousKeyStates[keyCmd->Scancode];

        switch (keyCmd->State)
        {
		case KeyState::Pressed:
			if (isKeyPressed)
				keyCmd->command->Execute();
			break;
		case KeyState::Down:
			if (isKeyPressed && !wasKeyPressed)
				keyCmd->command->Execute();
			break;
		case KeyState::Up:
			if (!isKeyPressed && wasKeyPressed)
				keyCmd->command->Execute();
			break;
        }
    }

	//Controller input
	for (auto& controller : m_pControllerInput)
	{
		controller->ProcessInput();
	}

    // Store previous state
    memcpy(m_PreviousKeyStates, m_CurrentKeyStates, sizeof(m_PreviousKeyStates));

    return true;
}

void InputManager::UpdateKeyStates()
{
    // Get new keyboard state
    if (!m_CurrentKeyStates)
    {
        m_CurrentKeyStates = SDL_GetKeyboardState(NULL);
    }
}

void InputManager::BindKey(SDL_Scancode scancode, KeyState state, std::unique_ptr<Command> command)
{
    m_pKeyBindings.emplace_back(std::make_unique<KeyCommand>(KeyCommand{ scancode, state, std::move(command) }));
}

void InputManager::UnbindKey(SDL_Scancode scancode)
{
    // Find the key binding
    auto iterator = std::remove_if(m_pKeyBindings.begin(), m_pKeyBindings.end(),
        [scancode](const std::unique_ptr<KeyCommand>& keyCmd)
        {
            return keyCmd->Scancode == scancode;
        });

    // Remove from the vector
    if (iterator != m_pKeyBindings.end())
    {
        m_pKeyBindings.erase(iterator, m_pKeyBindings.end());
    }
}

void InputManager::AddController(std::unique_ptr<ControllerInput> controller)
{
	m_pControllerInput.emplace_back(std::move(controller));
}

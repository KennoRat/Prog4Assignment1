#pragma once
#include "Singleton.h"
#include "Command.h"
#include <SDL.h>
#include <memory>
#include <vector>
#include "ControllerInput.h"

namespace dae
{
    enum class KeyState
    {
        Pressed,
        Down,
        Up
    };

    struct KeyCommand
    {
        SDL_Scancode Scancode;
        KeyState State;
        std::unique_ptr<Command> command;
    };

    class InputManager final : public Singleton<InputManager>
    {
    public:
        bool ProcessInput();
        void UpdateKeyStates();

        void BindKey(SDL_Scancode scancode, KeyState state, std::unique_ptr<Command> command);
        void UnbindKey(SDL_Scancode scancode);

		void AddController(std::unique_ptr<ControllerInput> controller);

    private:
        friend class Singleton<InputManager>;
        InputManager() = default;

        const Uint8* m_CurrentKeyStates{ nullptr };
        Uint8 m_PreviousKeyStates[SDL_NUM_SCANCODES]{ 0 };

        std::vector<std::unique_ptr<KeyCommand>> m_pKeyBindings;
        std::vector<std::unique_ptr <ControllerInput>> m_pControllerInput;
    };
}

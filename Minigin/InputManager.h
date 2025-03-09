#pragma once
#include "Singleton.h"
#include "Command.h"
#include <unordered_map>
#include <SDL.h>
#include <memory>

namespace dae
{
	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();

		// Methods to bind commands
		void BindKey(SDL_Keycode key, std::unique_ptr<Command> command);

	private:
		friend class Singleton<InputManager>;
		InputManager() = default;

		std::unordered_map<SDL_Keycode, std::unique_ptr<Command>> m_KeyBindings;
	};

}

#include <SDL.h>
#include "InputManager.h"
#include "imgui.h"
#include <backends/imgui_impl_sdl2.h>
#include <iostream>


using namespace dae;

bool InputManager::ProcessInput()
{
	std::vector<SDL_Keycode> m_pressedKeys;

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			return false;
		}
		if (e.type == SDL_KEYDOWN) {
			auto iterator = m_KeyBindings.find(e.key.keysym.sym);
			if (iterator != m_KeyBindings.end()) // If key is bound to a command
			{
				//std::cout << "Key Pressed: " << SDL_GetKeyName(e.key.keysym.sym) << std::endl;
				m_pressedKeys.push_back(e.key.keysym.sym);
			}
		}
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			
		}

		// Now execute all collected key commands
		for (auto key : m_pressedKeys) {
			auto it = m_KeyBindings.find(key);
			if (it != m_KeyBindings.end()) {
				it->second->Execute();
			}
		}

		//Process events for ImGui
		ImGui_ImplSDL2_ProcessEvent(&e);
	}

	return true;
}

void InputManager::BindKey(SDL_Keycode key, std::unique_ptr<Command> command)
{
	m_KeyBindings[key] = std::move(command);
}

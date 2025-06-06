#pragma once

#include "GameState.h"
#include <memory>
#include <SDL_scancode.h>

namespace dae 
{
    class Scene;
    class GameObject;
    class TextComponent;
}

// Define button indices
enum class MenuButtons
{
    PLAY_BUTTON_INDEX = 0,
    QUIT_BUTTON_INDEX = 1,
    TOTAL_MENU_BUTTONS = 2,
};


class MenuState final : public dae::GameState
{
public:
    // Constructor and Destructor
    MenuState();
    virtual ~MenuState() override;

    void OnEnter(dae::GameStateMachine* pStateMachine) override;
    void OnExit(dae::GameStateMachine* pStateMachine) override;

    dae::StateTransition HandleInput() override;
    dae::StateTransition Update(float deltaTime) override;
    void Render() const override;

    // Command methods
    void SelectNextOption();
    void SelectPreviousOption();
    dae::StateTransition ActivateSelectedOption();
    MenuButtons GetSelectedOptionIndex() const { return m_SelectedButtonIndex; }

private:
    std::shared_ptr<dae::Scene> m_pMenuScene;
    std::shared_ptr<dae::GameObject> m_pPlayButtonObject;
    std::shared_ptr<dae::GameObject> m_pQuitButtonObject;
    dae::TextComponent* m_pPlayTextComponent = nullptr;
    dae::TextComponent* m_pQuitTextComponent = nullptr;
    MenuButtons m_SelectedButtonIndex = MenuButtons::PLAY_BUTTON_INDEX;

    // Key code for binding/unbinding
    SDL_Scancode m_KeyUpBinding = SDL_SCANCODE_UP;
    SDL_Scancode m_KeyDownBinding = SDL_SCANCODE_DOWN;
    SDL_Scancode m_KeyActivateBinding = SDL_SCANCODE_RETURN;

    void CreateUI();
    void CleanupUI();
    void HighlightSelectedButton();
};
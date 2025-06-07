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
enum class PausedButtons
{
    RESUME_BUTTON_INDEX = 0,
    MENU_BUTTON_INDEX = 1,
    TOTAL_PAUSED_BUTTONS = 2
};

class PausedState final : public dae::GameState
{
public:
    // Constructor and Destructor
    PausedState();
    virtual ~PausedState() override;

    virtual void OnEnter(dae::GameStateMachine* pStateMachine) override;
    virtual void OnExit(dae::GameStateMachine* pStateMachine) override;

    dae::StateTransition HandleInput() override;
    dae::StateTransition Update(float deltaTime) override;
    void ResetKeybindings() override;
    void Render() const override;

    // Command methods
    void SelectNextOption();
    void SelectPreviousOption();
    dae::StateTransition ActivateSelectedOption();
    PausedButtons GetSelectedOptionIndex() const { return m_SelectedButtonIndex; }

private:
    
    bool m_IsUICreated = false;
    std::shared_ptr<dae::Scene> m_pPauseScene;
    std::shared_ptr<dae::GameObject> m_pResumeButtonObject;
    std::shared_ptr<dae::GameObject> m_pMenuButtonObject;
    dae::TextComponent* m_pResumeTextComponent = nullptr;
    dae::TextComponent* m_pMenuTextComponent = nullptr;
    PausedButtons m_SelectedButtonIndex = PausedButtons::RESUME_BUTTON_INDEX;

    // Key code for binding/unbinding
    SDL_Scancode m_KeyUpBinding = SDL_SCANCODE_W;
    SDL_Scancode m_KeyDownBinding = SDL_SCANCODE_S;
    SDL_Scancode m_KeyActivateBinding = SDL_SCANCODE_RETURN;
    
    void CreateUI();
    void CleanupUI();
    void HighlightSelectedButton();
};
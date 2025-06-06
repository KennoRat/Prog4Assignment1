#pragma once

#include "GameState.h"
#include <memory>

namespace dae 
{
    class GameObject;
    class TextComponent;
    class Scene;
}

// Define button indices
enum class PausedButtons
{
    RESUME_BUTTON_INDEX = 0,
    MENU_BUTTON_INDEX = 1,
    TOTAL_PAUSED_BUTTONS = 2,
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
    void Render() const override;

    // Command methods
    void SelectNextOption();
    void SelectPreviousOption();
    dae::StateTransition ActivateSelectedOption();
    PausedButtons GetSelectedOptionIndex() const { return m_SelectedButtonIndex; }

private:

    std::shared_ptr<dae::Scene> m_pPauseScene;
    std::shared_ptr<dae::GameObject> m_pResumeButtonObject;
    std::shared_ptr<dae::GameObject> m_pMenuButtonObject;
    dae::TextComponent* m_pResumeTextComponent = nullptr;
    dae::TextComponent* m_pMenuTextComponent = nullptr;
    PausedButtons m_SelectedButtonIndex = PausedButtons::RESUME_BUTTON_INDEX;
    
    void CreateUI();
    void CleanupUI();
    void HighlightSelectedButton();
};
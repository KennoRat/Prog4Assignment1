#include "MenuState.h"
#include <iostream>

// Engine Specific
#include "GameStateMachine.h" 
#include "InputManager.h"   
#include "SceneManager.h"  
#include "ResourceManager.h"
#include "GameObject.h"     
#include "Scene.h"
#include "TextComponent.h"    

// Game Specific
#include "PlayingState.h" 
#include "UINavigateCommand.h"
#include "UIActivateCommand.h"

MenuState::MenuState()
{
    std::cout << "MenuState Constructed" << std::endl;
}

MenuState::~MenuState()
{
    std::cout << "MenuState Destroyed" << std::endl;
    CleanupUI();
}

void MenuState::OnEnter(dae::GameStateMachine* /*pStateMachine*/)
{
    std::cout << "Entering MenuState" << std::endl;
    m_pMenuScene = dae::SceneManager::GetInstance().GetSceneByName("MenuScene");

    if (!m_pMenuScene) 
    {
        m_pMenuScene = dae::SceneManager::GetInstance().CreateScene("MenuScene");
    }

    if (m_pMenuScene) 
    {
        dae::SceneManager::GetInstance().SetActiveScene("MenuScene");
        CreateUI();
        m_SelectedButtonIndex = MenuButtons::PLAY_BUTTON_INDEX;
        HighlightSelectedButton();

        // Bind UI commands
        auto& input = dae::InputManager::GetInstance();

        input.BindKey(m_KeyUpBinding, dae::KeyState::Down, std::make_unique<UINavigateCommand>(GetStateMachine(), NavigationDirection::Up));
        input.BindKey(m_KeyDownBinding, dae::KeyState::Down, std::make_unique<UINavigateCommand>(GetStateMachine(), NavigationDirection::Down));
        input.BindKey(m_KeyActivateBinding, dae::KeyState::Down, std::make_unique<UIActivateCommand>(GetStateMachine()));
        //input.BindKey(SDL_SCANCODE_KP_ENTER, dae::KeyState::Down, std::make_unique<UIActivateCommand>(GetStateMachine()));
    }
}

void MenuState::OnExit(dae::GameStateMachine* /*pStateMachine*/)
{
    std::cout << "Exiting MenuState" << std::endl;
    CleanupUI();

    if (m_pMenuScene) 
    {
        dae::SceneManager::GetInstance().RemoveScene("MenuScene");
        m_pMenuScene.reset();
    }

    // Unbind UI commands
    auto& input = dae::InputManager::GetInstance();
    input.UnbindKey(m_KeyUpBinding);
    input.UnbindKey(m_KeyDownBinding);
    input.UnbindKey(m_KeyActivateBinding);
    //input.UnbindKey(SDL_SCANCODE_KP_ENTER);
}

dae::StateTransition MenuState::HandleInput()
{
    //auto& input = dae::InputManager::GetInstance();

    //if (input.IsKeyDownThisFrame(SDL_SCANCODE_W)) 
    //{ // Assuming IsKeyDownThisFrame exists
    //    m_SelectedButtonIndex = (m_SelectedButtonIndex + 1) % TOTAL_MENU_BUTTONS;
    //    HighlightSelectedButton();
    //    std::cout << "Menu: Down pressed, selected: " << m_SelectedButtonIndex << std::endl;
    //}
    //else if (input.IsKeyDownThisFrame(SDL_SCANCODE_S))
    //{
    //    m_SelectedButtonIndex = (m_SelectedButtonIndex - 1 + TOTAL_MENU_BUTTONS) % TOTAL_MENU_BUTTONS;
    //    HighlightSelectedButton();
    //    std::cout << "Menu: Up pressed, selected: " << m_SelectedButtonIndex << std::endl;
    //}
    //else if (input.IsKeyDownThisFrame(SDL_SCANCODE_RETURN) || input.IsKeyDownThisFrame(SDL_SCANCODE_KP_ENTER)) 
    //{
    //    std::cout << "Menu: Enter pressed, action for: " << m_SelectedButtonIndex << std::endl;
    //    if (m_SelectedButtonIndex == PLAY_BUTTON_INDEX) {
    //        return dae::StateTransition(dae::TransitionType::Set, std::make_unique<PlayingState>());
    //    }
    //    else if (m_SelectedButtonIndex == QUIT_BUTTON_INDEX) {
    //        GetStateMachine()->RequestQuit();
    //        return dae::StateTransition(dae::TransitionType::None); 
    //    }
    //}

    return dae::StateTransition(dae::TransitionType::None);
}

dae::StateTransition MenuState::Update(float /*deltaTime*/)
{
    return dae::StateTransition(dae::TransitionType::None);
}

void MenuState::ResetKeybindings()
{
    // Nothing to reset
}

void MenuState::Render() const
{
    // SceneManager handles rendering for this state
}

void MenuState::CreateUI()
{
    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
    float buttonYStart = 300.0f;
    float buttonSpacing = 60.0f;

    // Play Button
    m_pPlayButtonObject = std::make_shared<dae::GameObject>();
    auto playText = std::make_unique<dae::TextComponent>(m_pPlayButtonObject, "PLAY", font);
    m_pPlayTextComponent = playText.get();
    m_pPlayButtonObject->AddComponent(std::move(playText));
    m_pPlayButtonObject->SetLocalPosition(550, buttonYStart);
    if (m_pMenuScene) m_pMenuScene->Add(m_pPlayButtonObject);

    // Quit Button
    m_pQuitButtonObject = std::make_shared<dae::GameObject>();
    auto quitText = std::make_unique<dae::TextComponent>(m_pQuitButtonObject, "QUIT", font);
    m_pQuitTextComponent = quitText.get();
    m_pQuitButtonObject->AddComponent(std::move(quitText));
    m_pQuitButtonObject->SetLocalPosition(550, buttonYStart + buttonSpacing);
    if (m_pMenuScene) m_pMenuScene->Add(m_pQuitButtonObject);

    // Title
    auto titleObject = std::make_shared<dae::GameObject>();
    auto titleFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 72);
    auto titleText = std::make_unique<dae::TextComponent>(titleObject, "Ms. Pac-Man", titleFont);
    titleObject->AddComponent(std::move(titleText));
    titleObject->SetLocalPosition(400, 100);
    if (m_pMenuScene) m_pMenuScene->Add(titleObject);

}

void MenuState::CleanupUI()
{
    m_pPlayButtonObject.reset();
    m_pQuitButtonObject.reset();
    m_pPlayTextComponent = nullptr;
    m_pQuitTextComponent = nullptr;
}

void MenuState::HighlightSelectedButton()
{
    if (!m_pPlayTextComponent || !m_pQuitTextComponent) return;

    m_pPlayTextComponent->SetText("PLAY");
    m_pQuitTextComponent->SetText("QUIT");

    if (m_SelectedButtonIndex == MenuButtons::PLAY_BUTTON_INDEX) {
        m_pPlayTextComponent->SetText("> PLAY");
    }
    else if (m_SelectedButtonIndex == MenuButtons::QUIT_BUTTON_INDEX) {
        m_pQuitTextComponent->SetText("> QUIT");
    }
}


void MenuState::SelectNextOption() 
{
    m_SelectedButtonIndex = static_cast<MenuButtons>((static_cast<int>(m_SelectedButtonIndex) + 1) 
        % static_cast<int>(MenuButtons::TOTAL_MENU_BUTTONS));

    HighlightSelectedButton();
    std::cout << "Menu: Selected next option: " << static_cast<int>(m_SelectedButtonIndex) << std::endl;
}

void MenuState::SelectPreviousOption() 
{
    m_SelectedButtonIndex = static_cast<MenuButtons>((static_cast<int>(m_SelectedButtonIndex) - 1 + static_cast<int>(MenuButtons::TOTAL_MENU_BUTTONS)) 
        % static_cast<int>(MenuButtons::TOTAL_MENU_BUTTONS));

    HighlightSelectedButton();
    std::cout << "Menu: Selected previous option: " << static_cast<int>(m_SelectedButtonIndex) << std::endl;
}

dae::StateTransition MenuState::ActivateSelectedOption() 
{
    std::cout << "Menu: Activating selected option: " << static_cast<int>(m_SelectedButtonIndex) << std::endl;

    if (m_SelectedButtonIndex == MenuButtons::PLAY_BUTTON_INDEX) 
    {
        return dae::StateTransition(dae::TransitionType::Set, std::make_unique<PlayingState>());
    }
    else if (m_SelectedButtonIndex == MenuButtons::QUIT_BUTTON_INDEX) 
    {
        GetStateMachine()->RequestQuit();
        return dae::StateTransition();
    }

    return dae::StateTransition();
}
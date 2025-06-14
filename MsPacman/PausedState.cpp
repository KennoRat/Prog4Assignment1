#include "PausedState.h"
#include <SDL_scancode.h>
#include <iostream>

// Engine Specific
#include "GameStateMachine.h" 
#include "InputManager.h"   
#include "SceneManager.h"
#include "Renderer.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "GameObject.h"     
#include "TextComponent.h"   

// Game Specific
#include "MenuState.h"    
#include "PlayingState.h" 
#include "UINavigateCommand.h"
#include "UIActivateCommand.h"
#include "DimOverlayComponent.h"

PausedState::PausedState()
{
    std::cout << "PausedState Constructed" << std::endl;
}

PausedState::~PausedState()
{
    std::cout << "PausedState Destroyed" << std::endl;
    CleanupUI();
}

void PausedState::OnEnter(dae::GameStateMachine* /*pStateMachine*/)
{
    std::cout << "Entering PausedState" << std::endl;
    m_pPauseScene = dae::SceneManager::GetInstance().GetSceneByName("PauseScene");

    if (!m_pPauseScene)
    {
        m_pPauseScene = dae::SceneManager::GetInstance().CreateScene("PauseScene");
    }

    if (m_pPauseScene)
    {
        //dae::SceneManager::GetInstance().SetActiveScene("PauseScene");
        CreateUI();
        m_SelectedButtonIndex = PausedButtons::RESUME_BUTTON_INDEX;
        HighlightSelectedButton();

        // Bind UI commands
        auto& input = dae::InputManager::GetInstance();

        input.BindKey(m_KeyUpBinding, dae::KeyState::Down, std::make_unique<UINavigateCommand>(GetStateMachine(), NavigationDirection::Up));
        input.BindKey(m_KeyDownBinding, dae::KeyState::Down, std::make_unique<UINavigateCommand>(GetStateMachine(), NavigationDirection::Down));
        input.BindKey(m_KeyActivateBinding, dae::KeyState::Down, std::make_unique<UIActivateCommand>(GetStateMachine()));
    }    
}

void PausedState::OnExit(dae::GameStateMachine* /*pStateMachine*/)
{
    std::cout << "Exiting PausedState" << std::endl;
    CleanupUI();

    if (m_pPauseScene) 
    {
        dae::SceneManager::GetInstance().RemoveScene("PauseScene");
        m_pPauseScene.reset();
    }

    // Unbind UI commands
    auto& input = dae::InputManager::GetInstance();
    input.UnbindKey(m_KeyUpBinding);
    input.UnbindKey(m_KeyDownBinding);
    input.UnbindKey(m_KeyActivateBinding);
    input.ClearBindings();
}

dae::StateTransition PausedState::HandleInput()
{
    //auto& input = dae::InputManager::GetInstance();

    //if (input.IsKeyDownThisFrame(SDL_SCANCODE_ESCAPE)) {
    //    return dae::StateTransition(dae::TransitionType::Pop); // Pop self (PausedState)
    //}
    //else if (input.IsKeyDownThisFrame(SDL_SCANCODE_DOWN)) {
    //    m_SelectedButtonIndex = (m_SelectedButtonIndex + 1) % TOTAL_PAUSE_BUTTONS;
    //    HighlightSelectedButton();
    //}
    //else if (input.IsKeyDownThisFrame(SDL_SCANCODE_UP)) {
    //    m_SelectedButtonIndex = (m_SelectedButtonIndex - 1 + TOTAL_PAUSE_BUTTONS) % TOTAL_PAUSE_BUTTONS;
    //    HighlightSelectedButton();
    //}
    //else if (input.IsKeyDownThisFrame(SDL_SCANCODE_RETURN) || input.IsKeyDownThisFrame(SDL_SCANCODE_KP_ENTER)) {
    //    if (m_SelectedButtonIndex == RESUME_BUTTON_INDEX_PAUSE) {
    //        return dae::StateTransition(dae::TransitionType::Pop); // Pop self
    //    }
    //    else if (m_SelectedButtonIndex == MENU_BUTTON_INDEX_PAUSE) {
    //        // This will pop PausedState, then pop PlayingState (if it's below), then set MenuState.
    //        // If you want to go directly Menu -> Pop Playing -> Pop Paused -> Set Menu.
    //        // This requires the GameStateMachine to handle multiple pops before a set, or a specific transition type.
    //        // For now, this sets the MenuState directly, replacing the whole stack.
    //        return dae::StateTransition(dae::TransitionType::Set, std::make_unique<MenuState>());
    //    }
    //}

    return dae::StateTransition(dae::TransitionType::None);
}

dae::StateTransition PausedState::Update(float /*deltaTime*/)
{
    if (m_pPauseScene && m_IsUICreated) 
    {
        const auto& gameObjectsInPauseScene = m_pPauseScene->GetAllGameObjects();
        for (const auto& go : gameObjectsInPauseScene) 
        {
            if (go) 
            {
                go->Update();
            }
        }
    }

    return dae::StateTransition(dae::TransitionType::None);
}

void PausedState::ResetKeybindings()
{
    // Nothing to reset
}

void PausedState::Render() const
{
    //  overlay render
   if (m_pPauseScene && m_IsUICreated)
    {
        const auto& gameObjectsInPauseScene = m_pPauseScene->GetAllGameObjects(); 
        for (const auto& go : gameObjectsInPauseScene) 
        {
            if (go) 
            { 
                go->Render(); 
            }
        }
   }
}

void PausedState::CreateUI()
{
    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
    float buttonYStart = 300.0f;
    float buttonSpacing = 60.0f;

    // Dimmed Background Overlay
    auto dimOverlayObject = std::make_shared<dae::GameObject>();
    SDL_Color dimColor = { 0, 0, 0, 150 }; // 60% opacity
    int screenWidth = 0;
    int screenHeight = 0;
    dae::Renderer::GetInstance().GetWindowDimensions(screenWidth, screenHeight);
    auto dimComponent = std::make_unique<DimOverlayComponent>
    (
        dimOverlayObject,
        dimColor,
        static_cast<float>(screenWidth),
        static_cast<float>(screenHeight)
    );
    dimOverlayObject->AddComponent(std::move(dimComponent));
    m_pPauseScene->Add(dimOverlayObject);

    // Resume Button
    m_pResumeButtonObject = std::make_shared<dae::GameObject>();
    auto resumeText = std::make_unique<dae::TextComponent>(m_pResumeButtonObject, "RESUME", font);
    m_pResumeTextComponent = resumeText.get();
    m_pResumeButtonObject->AddComponent(std::move(resumeText));
    m_pResumeButtonObject->SetLocalPosition(550, buttonYStart);
    if (m_pPauseScene) m_pPauseScene->Add(m_pResumeButtonObject);

    // Menu Button
    m_pMenuButtonObject = std::make_shared<dae::GameObject>();
    auto menuText = std::make_unique<dae::TextComponent>(m_pMenuButtonObject, "MAIN MENU", font);
    m_pMenuTextComponent = menuText.get();
    m_pMenuButtonObject->AddComponent(std::move(menuText));
    m_pMenuButtonObject->SetLocalPosition(520, buttonYStart + buttonSpacing);
    if (m_pPauseScene) m_pPauseScene->Add(m_pMenuButtonObject);

    // Paused Title
    auto titleObject = std::make_shared<dae::GameObject>();
    auto titleFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 72);
    auto titleText = std::make_unique<dae::TextComponent>(titleObject, "PAUSED", titleFont);
    titleObject->AddComponent(std::move(titleText));
    titleObject->SetLocalPosition(500, 100);
    if (m_pPauseScene) m_pPauseScene->Add(titleObject);

    m_IsUICreated = true;
}

void PausedState::CleanupUI()
{
    m_pResumeButtonObject.reset();
    m_pMenuButtonObject.reset();
    m_pResumeTextComponent = nullptr;
    m_pMenuTextComponent = nullptr;

    m_IsUICreated = false;
}

void PausedState::HighlightSelectedButton()
{
    if (!m_pResumeTextComponent || !m_pMenuTextComponent) return;

    m_pResumeTextComponent->SetText("RESUME");
    m_pMenuTextComponent->SetText("MAIN MENU");

    if (m_SelectedButtonIndex == PausedButtons::RESUME_BUTTON_INDEX) 
    {
        m_pResumeTextComponent->SetText("> RESUME");
    }
    else if (m_SelectedButtonIndex == PausedButtons::MENU_BUTTON_INDEX) 
    {
        m_pMenuTextComponent->SetText("> MAIN MENU");
    }
}

void PausedState::SelectNextOption()
{
    m_SelectedButtonIndex = static_cast<PausedButtons>((static_cast<int>(m_SelectedButtonIndex) + 1)
        % static_cast<int>(PausedButtons::TOTAL_PAUSED_BUTTONS));

    HighlightSelectedButton();
    std::cout << "Paused: Selected next option: " << static_cast<int>(m_SelectedButtonIndex) << std::endl;
}

void PausedState::SelectPreviousOption()
{
    m_SelectedButtonIndex = static_cast<PausedButtons>((static_cast<int>(m_SelectedButtonIndex) - 1 + static_cast<int>(PausedButtons::TOTAL_PAUSED_BUTTONS))
        % static_cast<int>(PausedButtons::TOTAL_PAUSED_BUTTONS));

    HighlightSelectedButton();
    std::cout << "Paused: Selected previous option: " << static_cast<int>(m_SelectedButtonIndex) << std::endl;
}

dae::StateTransition PausedState::ActivateSelectedOption()
{
    std::cout << "Paused: Activating selected option: " << static_cast<int>(m_SelectedButtonIndex) << std::endl;

    if (m_SelectedButtonIndex == PausedButtons::RESUME_BUTTON_INDEX)
    {
        return dae::StateTransition(dae::TransitionType::Set, std::make_unique<PlayingState>());
    }
    else if (m_SelectedButtonIndex == PausedButtons::MENU_BUTTON_INDEX)
    {
        GetStateMachine()->RequestQuit();
        return dae::StateTransition();
    }

    return dae::StateTransition();
}
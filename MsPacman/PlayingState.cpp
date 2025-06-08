#include "PlayingState.h"
#include <SDL_scancode.h>
#include <iostream>

// Engine Specific
#include "GameStateMachine.h" 
#include "InputManager.h"   
#include "SceneManager.h"   
#include "Scene.h"
#include "ResourceManager.h"

// Game Specific
#include "GameObject.h"
// Components
#include "TextComponent.h"
#include "RenderComponent.h"
#include "LivesDisplayComponent.h"
#include "ScoreComponent.h"
#include "ScoreDisplayComponent.h"
#include "LivesComponent.h"
#include "LevelGridComponent.h"
#include "PlayerMovementComponent.h"
#include "SpriteAnimationComponent.h"
// Commands
#include "MoveCommand.h"
#include "DieCommand.h"
#include "GainPointsCommand.h"
// Observer
#include "Subject.h"
// States
#include "PausedState.h"   


PlayingState::PlayingState() : m_pGameScene(nullptr), m_GameSceneLoaded(false)
{
    std::cout << "PlayingState Constructed" << std::endl;
}

PlayingState::~PlayingState()
{
    std::cout << "PlayingState Destroyed" << std::endl;
}

void PlayingState::OnEnter(dae::GameStateMachine* /*pStateMachine*/)
{
    std::cout << "Entering PlayingState" << std::endl;

    // Load only if not already loaded 
    if (!m_GameSceneLoaded) 
    {
        LoadGameScene();
        m_GameSceneLoaded = true;
    }

    if (m_pGameScene) 
    {
        dae::SceneManager::GetInstance().SetActiveScene("GameScene");
    }

    // Bind commands
    auto& input = dae::InputManager::GetInstance();
    input.ClearBindings();

    input.BindKey(SDL_SCANCODE_W, dae::KeyState::Down, std::make_unique<dae::MoveCommand>(m_pMissPacman, dae::Direction::Up));
    input.BindKey(SDL_SCANCODE_A, dae::KeyState::Down, std::make_unique<dae::MoveCommand>(m_pMissPacman, dae::Direction::Left));
    input.BindKey(SDL_SCANCODE_D, dae::KeyState::Down, std::make_unique<dae::MoveCommand>(m_pMissPacman, dae::Direction::Right));
    input.BindKey(SDL_SCANCODE_S, dae::KeyState::Down, std::make_unique<dae::MoveCommand>(m_pMissPacman, dae::Direction::Down));
}

void PlayingState::OnExit(dae::GameStateMachine* /*pStateMachine*/)
{
    std::cout << "Exiting PlayingState" << std::endl;

    UnloadGameScene();

    // Unbind UI commands
    auto& input = dae::InputManager::GetInstance();
    input.UnbindKey(SDL_SCANCODE_W);
    input.UnbindKey(SDL_SCANCODE_A);
    input.UnbindKey(SDL_SCANCODE_D);
    input.UnbindKey(SDL_SCANCODE_S);
}

void PlayingState::ResetKeybindings()
{
    // Bind commands
    auto& input = dae::InputManager::GetInstance();
    input.ClearBindings();

    input.BindKey(SDL_SCANCODE_W, dae::KeyState::Down, std::make_unique<dae::MoveCommand>(m_pMissPacman, dae::Direction::Up));
    input.BindKey(SDL_SCANCODE_A, dae::KeyState::Down, std::make_unique<dae::MoveCommand>(m_pMissPacman, dae::Direction::Left));
    input.BindKey(SDL_SCANCODE_D, dae::KeyState::Down, std::make_unique<dae::MoveCommand>(m_pMissPacman, dae::Direction::Right));
    input.BindKey(SDL_SCANCODE_S, dae::KeyState::Down, std::make_unique<dae::MoveCommand>(m_pMissPacman, dae::Direction::Down));
}

dae::StateTransition PlayingState::HandleInput()
{
    auto& input = dae::InputManager::GetInstance();

    if (input.IsKeyDownThisFrame(SDL_SCANCODE_ESCAPE)) 
    {
        dae::SceneManager::GetInstance().PauseActiveScene(true);
        return dae::StateTransition(dae::TransitionType::Push, std::make_unique<PausedState>());
    }

    return dae::StateTransition(dae::TransitionType::None);
}

dae::StateTransition PlayingState::Update(float /*deltaTime*/)
{
    return dae::StateTransition(dae::TransitionType::None);
}

void PlayingState::Render() const
{
    // SceneManager handles rendering for this state
}

void PlayingState::LoadGameScene()
{
    std::cout << "Loading GameScene..." << std::endl;

    m_pGameScene = dae::SceneManager::GetInstance().CreateScene("GameScene");

    //Make Background
    auto backgroundObject = std::make_shared<dae::GameObject>();
    auto textureComponent = std::make_unique<dae::RenderComponent>(backgroundObject, 1280.f, 720.f);
    textureComponent->SetTexture("background.tga"); 
    backgroundObject->AddComponent(std::move(textureComponent));
    m_pGameScene->Add(backgroundObject);

    // Test Level
    auto levelObject = std::make_shared<dae::GameObject>();
    float desiredTileSize = 20.0f;
    auto levelGridComponent = std::make_unique<LevelGridComponent>(levelObject, desiredTileSize);
    levelGridComponent->LoadLevel("../Data/MsPacman_Level1.csv"); 
    levelObject->SetLocalPosition(370.f, 50.f);
    levelObject->AddComponent(std::move(levelGridComponent));
    m_pGameScene->Add(levelObject);

    // Initialize Player
    //Make UI Display for Player 1 (Lives & Score)
    auto displayLivesPlayer1Object = std::make_shared<dae::GameObject>();
    auto displayScorePlayer1Object = std::make_shared<dae::GameObject>();

    // Create subjects for events
    auto player1DieEvent = std::make_unique<dae::Subject>();
    auto player1GainPointsEvent = std::make_unique<dae::Subject>();

    auto p1LivesDisplayComp = std::make_unique<dae::LivesDisplayComponent>(displayLivesPlayer1Object);
    player1DieEvent->AddObserver(p1LivesDisplayComp.get());
    auto p1ScoreDisplayComp = std::make_unique<dae::ScoreDisplayComponent>(displayScorePlayer1Object);
    player1GainPointsEvent->AddObserver(p1ScoreDisplayComp.get());

    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
    auto p1LivesText = std::make_unique<dae::TextComponent>(displayLivesPlayer1Object, "# Lives P1: 3", font); 
    displayLivesPlayer1Object->AddComponent(std::move(p1LivesText));
    displayLivesPlayer1Object->AddComponent(std::move(p1LivesDisplayComp));
    displayLivesPlayer1Object->SetLocalPosition(10, 50);
    m_pGameScene->Add(displayLivesPlayer1Object);

    auto p1ScoreText = std::make_unique<dae::TextComponent>(displayScorePlayer1Object, "Score P1: 0", font);
    displayScorePlayer1Object->AddComponent(std::move(p1ScoreText));
    displayScorePlayer1Object->AddComponent(std::move(p1ScoreDisplayComp));
    displayScorePlayer1Object->SetLocalPosition(10, 80);
    m_pGameScene->Add(displayScorePlayer1Object);

    //Make MsPacMan
    m_pMissPacman = std::make_shared<dae::GameObject>();
    auto pacTexture = std::make_unique<dae::RenderComponent>(m_pMissPacman);
    pacTexture->SetTexture("MissPacMan.png"); 
    // Pass the player-specific subjects to player components
    auto livesComponent = std::make_unique<dae::LivesComponent>(m_pMissPacman, std::move(player1DieEvent), 3);
    auto scoreComponent = std::make_unique<dae::ScoreComponent>(m_pMissPacman, std::move(player1GainPointsEvent));
    const float movementSpeedPlayer1 = 100.f;
    auto movementComponent = std::make_unique<PlayerMovementComponent>(m_pMissPacman, levelObject, movementSpeedPlayer1);

    m_pMissPacman->AddComponent(std::move(pacTexture));
    m_pMissPacman->AddComponent(std::move(livesComponent));
    m_pMissPacman->AddComponent(std::move(scoreComponent));
    m_pMissPacman->AddComponent(std::move(movementComponent));

    // SpriteAnimationComponent
    float fps = 10.0f;
    int spriteWidth = 15;  
    int spriteHeight = 15;
    int framesPerDir = 3;
    auto spriteAnimComponent = std::make_unique<SpriteAnimationComponent>
    (
        m_pMissPacman, fps, spriteWidth, spriteHeight, framesPerDir
    );
    
    m_pMissPacman->AddComponent(std::move(spriteAnimComponent));

    auto* pMoveComp = m_pMissPacman->GetComponent<PlayerMovementComponent>();
    int startRow = 23; int startCol = 13; // Ms PacMan start
    if (pMoveComp) pMoveComp->SnapToGrid(startRow, startCol);

    m_pGameScene->Add(m_pMissPacman);

}

void PlayingState::UnloadGameScene()
{
    std::cout << "Unloading GameScene..." << std::endl;

    if (m_pGameScene) 
    {
        dae::InputManager::GetInstance().ClearBindings(); 
        dae::SceneManager::GetInstance().RemoveScene("GameScene");
        m_pGameScene.reset();
    }
}
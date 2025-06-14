#include "PlayingState.h"
#include <SDL_scancode.h>
#include <iostream>

// Engine Specific
#include "GameStateMachine.h" 
#include "InputManager.h"   
#include "SceneManager.h"   
#include "Scene.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "RenderComponent.h"
#include "ColliderComponent.h"
#include "ServiceLocator.h"

// Game Specific
#include "GameObject.h"
#include "GameEvents.h"
// Components
#include "LivesDisplayComponent.h"
#include "ScoreComponent.h"
#include "ScoreDisplayComponent.h"
#include "LivesComponent.h"
#include "LevelGridComponent.h"
#include "PlayerMovementComponent.h"
#include "SpriteAnimationComponent.h"
#include "GhostSpriteAnimationComponent.h"
#include "BaseGhostComponent.h"
#include "PowerPelletComponent.h"
#include "GhostManagerComponent.h"
// Commands
#include "MoveCommand.h"
#include "DieCommand.h"
#include "GainPointsCommand.h"
#include "SetGoNextLevelCommand.h"
// Observer
#include "Subject.h"
// States
#include "PausedState.h"
#include "MenuState.h"
// Chase
#include "BlinkyChaseBehaviour.h"
#include "PinkyChaseBehaviour.h"


namespace Sounds
{
    dae::SoundId START_GAME = dae::INVALID_SOUND_ID;
    dae::SoundId MISS_PACMAN_DEATH = dae::INVALID_SOUND_ID;
}


PlayingState::PlayingState(int initialLevelIndex)
    : m_pGameScene(nullptr), m_gameSceneLoaded(false),
    m_isPlayerRespawning(false), m_playerRespawnTimer(0.f),
    m_currentLevelIndex(initialLevelIndex)
{
    std::cout << "PlayingState Constructed for level: " << m_currentLevelIndex << std::endl;
    InitializeLevelFilePaths();
}

PlayingState::~PlayingState()
{
    std::cout << "PlayingState Destroyed" << std::endl;
}

void PlayingState::OnEnter(dae::GameStateMachine* /*pStateMachine*/)
{
    std::cout << "Entering PlayingState" << std::endl;

    // Load only if not already loaded 
    if (!m_gameSceneLoaded) 
    {
        LoadGameSceneForLevel(m_currentLevelIndex);
        m_gameSceneLoaded = true;
    }

    if (m_pGameScene) 
    {
        dae::SceneManager::GetInstance().SetActiveScene("GameScene");
    }

    if (m_pLevelObject)
    {
        m_pLevelGridComponentCache = m_pLevelObject->GetComponent<LevelGridComponent>();
    }

    // Load Sounds 
    if (Sounds::START_GAME == dae::INVALID_SOUND_ID)
    {
        std::string startSoundPath = "../Data/Sounds/ms_start.wav";
        Sounds::START_GAME = dae::ServiceLocator::GetAudioService().LoadSound(startSoundPath);

        if (Sounds::START_GAME == dae::INVALID_SOUND_ID)
        {
            std::cerr << "Failed to queue loading for Start sound.\n";
        }

        dae::ServiceLocator::GetAudioService().PlaySound(Sounds::START_GAME, 0.7f);
    }

    if (Sounds::MISS_PACMAN_DEATH == dae::INVALID_SOUND_ID)
    {
        std::string deathSoundPath = "../Data/Sounds/ms_death.wav";
        Sounds::MISS_PACMAN_DEATH = dae::ServiceLocator::GetAudioService().LoadSound(deathSoundPath);

        if (Sounds::MISS_PACMAN_DEATH == dae::INVALID_SOUND_ID)
        {
            std::cerr << "Failed to queue loading for Death sound.\n";
        }
    }
}

void PlayingState::OnExit(dae::GameStateMachine* /*pStateMachine*/)
{
    std::cout << "Exiting PlayingState" << std::endl;

    if (m_pMissPacman)
    {
        auto* livesComp = m_pMissPacman->GetComponent<dae::LivesComponent>();

        if (livesComp && livesComp->GetDeathSubject())
        { 
            livesComp->GetDeathSubject()->RemoveObserver(this);
        }
    }

    //UnloadGameScene();

    // Unbind UI commands
    auto& input = dae::InputManager::GetInstance();
    input.UnbindKey(SDL_SCANCODE_W);
    input.UnbindKey(SDL_SCANCODE_A);
    input.UnbindKey(SDL_SCANCODE_D);
    input.UnbindKey(SDL_SCANCODE_S);
    input.ClearBindings();
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
    input.BindKey(SDL_SCANCODE_F1, dae::KeyState::Down, std::make_unique<SetGoNextLevelCommand>(this));
}

dae::StateTransition PlayingState::HandleInput()
{
    auto& input = dae::InputManager::GetInstance();

    if (input.IsKeyDownThisFrame(SDL_SCANCODE_ESCAPE) && !m_isPlayerRespawning)
    {
        dae::SceneManager::GetInstance().PauseActiveScene(true);
        return dae::StateTransition(dae::TransitionType::Push, std::make_unique<PausedState>());
    }

    return dae::StateTransition(dae::TransitionType::None);
}

dae::StateTransition PlayingState::Update(float deltaTime)
{
    if (m_isPlayerRespawning)
    {
        m_playerRespawnTimer -= deltaTime;

        if (m_playerRespawnTimer <= 0.f)
        {
            ResetPlayerForRespawn();
        }
    }

    // Check for Game Over 
    if (m_pMissPacman && !m_isPlayerRespawning)
    {
        auto livesComp = m_pMissPacman->GetComponent<dae::LivesComponent>();

        if (livesComp && livesComp->GetLives() <= 0)
        {
            std::cout << "PlayingState: Game Over" << std::endl;
            return dae::StateTransition(dae::TransitionType::Set, std::make_unique<MenuState>());
        }
    }

    if ((m_pLevelGridComponentCache && m_pLevelGridComponentCache->AreAllPelletsEaten() && !m_isPlayerRespawning) || m_goNextLevel)
    {
        m_currentLevelIndex++;
        m_goNextLevel = false;

        if (m_currentLevelIndex < (static_cast<int>(m_levelFilePaths.size())))
        {
            std::cout << "PlayingState: Loading next level: " << m_currentLevelIndex << std::endl;
            UnloadCurrentGameScene();
            LoadGameSceneForLevel(m_currentLevelIndex);
            ResetForNewLevel();
            dae::SceneManager::GetInstance().SetActiveScene("GameScene");

            if (m_pLevelObject) m_pLevelGridComponentCache = m_pLevelObject->GetComponent<LevelGridComponent>();

            if (Sounds::START_GAME != dae::INVALID_SOUND_ID)
            {
                dae::ServiceLocator::GetAudioService().PlaySound(Sounds::START_GAME, 0.7f);
            }

        }
        else
        {
            UnloadCurrentGameScene();
            return dae::StateTransition(dae::TransitionType::Set, std::make_unique<MenuState>());
        }
        return dae::StateTransition();
    }

    return dae::StateTransition(dae::TransitionType::None);
}

void PlayingState::Render() const
{
    // SceneManager handles rendering for this state
}

void PlayingState::Notify(const dae::GameObject& /*gameObject*/, const dae::Event& event)
{
    if (event.id == EVENT_PLAYER_DIED) 
    {
        if (!m_isPlayerRespawning)
        {
            dae::ServiceLocator::GetAudioService().PlaySound(Sounds::MISS_PACMAN_DEATH, 0.7f);
            m_isPlayerRespawning = true;
            m_playerRespawnTimer = PLAYER_RESPAWN_DELAY;

            if (m_pMissPacman)
            {
                auto renderComp = m_pMissPacman->GetComponent<dae::RenderComponent>();
                if (renderComp)
                {
                    renderComp->SetVisible(true);
                }

                auto movementComp = m_pMissPacman->GetComponent<PlayerMovementComponent>();
                if (movementComp)
                {
                    movementComp->SetIsActive(false);
                }
            }
        }
    }
}

void PlayingState::LoadGameSceneForLevel(int levelIndex)
{
    std::cout << "Loading GameScene for level index: " << levelIndex << std::endl;

    if (levelIndex < 0 || levelIndex >= static_cast<int>(m_levelFilePaths.size()))
    {
        GetStateMachine()->SetState(std::make_unique<MenuState>());
        return;
    }

    const std::string& currentLevelPath = m_levelFilePaths[levelIndex];

    std::cout << "Loading map: " << currentLevelPath << std::endl;

    m_pGameScene = dae::SceneManager::GetInstance().CreateScene("GameScene");

    // Make Background
    auto backgroundObject = std::make_shared<dae::GameObject>();
    auto textureComponent = std::make_unique<dae::RenderComponent>(backgroundObject, 1280.f, 720.f);
    textureComponent->SetTexture("background.tga");
    backgroundObject->AddComponent(std::move(textureComponent));
    m_pGameScene->Add(backgroundObject);

    // Make Level
    m_pLevelObject = std::make_shared<dae::GameObject>();
    m_pLevelObject->SetTag("Level");
    float desiredTileSize = 20.0f;
    auto levelGridComponent = std::make_unique<LevelGridComponent>(m_pLevelObject, desiredTileSize);
    levelGridComponent->LoadLevel(currentLevelPath);
    m_pLevelObject->SetLocalPosition(370.f, 50.f);
    m_pLevelObject->AddComponent(std::move(levelGridComponent));
    m_pGameScene->Add(m_pLevelObject);
    m_pLevelGridComponentCache = m_pLevelObject->GetComponent<LevelGridComponent>(); 

    // Make UI Display for Player 1 (Lives & Score)
    auto displayLivesPlayer1Object = std::make_shared<dae::GameObject>();
    auto displayScorePlayer1Object = std::make_shared<dae::GameObject>();

    // Create subjects for events
    auto player1DieEvent = std::make_unique<dae::Subject>();
    auto player1RespawnEvent = std::make_unique<dae::Subject>();
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

    // Initialize Player
    
    bool playerNeedsFullSetup = false;

    if (!m_pMissPacman)
    {
        m_pMissPacman = std::make_shared<dae::GameObject>();
        m_pMissPacman->SetTag("Player");
        playerNeedsFullSetup = true;
    }
     
    if (playerNeedsFullSetup) 
    {
        auto pacTexture = std::make_unique<dae::RenderComponent>(m_pMissPacman);
        pacTexture->SetTexture("MissPacMan.png");

        // Add the specific MsPacMan Components
        const int LIVES = 4;
        auto livesComponent = std::make_unique<dae::LivesComponent>(m_pMissPacman, std::move(player1DieEvent), std::move(player1RespawnEvent), LIVES);
        auto scoreComponent = std::make_unique<dae::ScoreComponent>(m_pMissPacman, std::move(player1GainPointsEvent));
        auto powerPelletComp = std::make_unique<PowerPelletComponent>(m_pMissPacman);


        m_pMissPacman->AddComponent(std::move(pacTexture));
        {
            dae::LivesComponent* pLivesCompRaw = livesComponent.get();
            m_pMissPacman->AddComponent(std::move(livesComponent));
            if (pLivesCompRaw && pLivesCompRaw->GetDeathSubject())
            {
                pLivesCompRaw->GetDeathSubject()->AddObserver(this);
            }
        }
        m_pMissPacman->AddComponent(std::move(scoreComponent));
        m_pMissPacman->AddComponent(std::move(powerPelletComp));

        // SpriteAnimationComponent MsPacMan
        float fps = 10.0f;
        int spriteWidth = 15;
        int spriteHeight = 15;
        int framesPerDir = 3;
        auto spriteAnimComponent = std::make_unique<SpriteAnimationComponent>
            (
                m_pMissPacman, fps, spriteWidth, spriteHeight, framesPerDir
            );

        m_pMissPacman->AddComponent(std::move(spriteAnimComponent));

        // ColliderComponent MsPacMan
        float pacmanColliderSize = desiredTileSize * 0.7f;
        auto pacmanCollider = std::make_unique<dae::ColliderComponent>(m_pMissPacman, pacmanColliderSize, pacmanColliderSize);
        pacmanCollider->SetTag("Player");

        pacmanCollider->SetCollisionCallback([playerObject = m_pMissPacman.get()](dae::GameObject* pOther)
        {
                if (pOther && pOther->GetComponent<BaseGhostComponent>())
                {
                    BaseGhostComponent* ghostComp = pOther->GetComponent<BaseGhostComponent>();
                    if (ghostComp &&
                        ghostComp->GetCurrentState() != GhostState::Frightened &&
                        ghostComp->GetCurrentState() != GhostState::Eaten)
                    {
                        std::cout << "Player collided with an active Ghost!" << std::endl;
                        auto livesComp = playerObject->GetComponent<dae::LivesComponent>();
                        if (livesComp)
                        {
                            livesComp->Die();
                        }
                    }
                }
        });

        m_pMissPacman->AddComponent(std::move(pacmanCollider));
    }

    if (m_pMissPacman->CheckComponent<PlayerMovementComponent>())
    { 
        m_pMissPacman->RemoveComponent<PlayerMovementComponent>(); 
    }

    const float movementSpeedPlayer1 = 100.f;
    auto movementComponent = std::make_unique<PlayerMovementComponent>(m_pMissPacman, m_pLevelObject, movementSpeedPlayer1);
    auto* pMoveCompRaw = movementComponent.get();
    m_pMissPacman->AddComponent(std::move(movementComponent));

    int startRow = 23; int startCol = 13; // Ms PacMan start
    if (pMoveCompRaw) pMoveCompRaw->SnapToGrid(startRow, startCol);

    m_pGameScene->Add(m_pMissPacman);


    // Ghost manager
    m_pGhostManager = std::make_shared<dae::GameObject>();
    m_pGhostManager->SetTag("GhostManager");
    auto ghostManagerComp = std::make_unique<GhostManagerComponent>
    (
        m_pGhostManager, m_pLevelObject, m_pMissPacman, m_pGameScene.get()
    );
    GhostManagerComponent* pGhostManagerRaw = ghostManagerComp.get();
    m_pGhostManager->AddComponent(std::move(ghostManagerComp));
    m_pGameScene->Add(m_pGhostManager);

    if (pGhostManagerRaw)
    {
        pGhostManagerRaw->Initialize(); 
    }

    ResetKeybindings();
}

void PlayingState::UnloadCurrentGameScene()
{
    std::cout << "Unloading GameScene..." << std::endl;

    if (m_pMissPacman)
    {
        auto* livesComp = m_pMissPacman->GetComponent<dae::LivesComponent>();

        if (livesComp && livesComp->GetDeathSubject())
        {
            livesComp->GetDeathSubject()->RemoveObserver(this);
        }
    }

    if (m_pGhostManager && m_pGhostManager->CheckComponent<GhostManagerComponent>())
    {
        m_pGhostManager->GetComponent<GhostManagerComponent>()->ClearGhostsFromScene();
        m_pGameScene->Remove(m_pGhostManager);
    }

    m_pMissPacman.reset();

    if (m_pGameScene) 
    {
        dae::InputManager::GetInstance().ClearBindings(); 
        dae::SceneManager::GetInstance().RemoveScene("GameScene");
        m_pGameScene.reset();
    }

    m_pLevelGridComponentCache = nullptr;
    m_gameSceneLoaded = false;
}

void PlayingState::InitializeLevelFilePaths()
{
    m_levelFilePaths.push_back("../Data/MsPacman_Level1.csv");
    m_levelFilePaths.push_back("../Data/MsPacman_Level2.csv");
}

void PlayingState::ResetPlayerForRespawn()
{
    m_isPlayerRespawning = false;

    if (m_pMissPacman)
    {
        std::cout << "PlayingState: Resetting player for respawn." << std::endl;
        dae::ServiceLocator::GetAudioService().PlaySound(Sounds::START_GAME, 0.7f);

        auto moveComp = m_pMissPacman->GetComponent<PlayerMovementComponent>();
        if (moveComp)
        {
            int pacmanStartRow = 23; int pacmanStartCol = 13;
            moveComp->SnapToGrid(pacmanStartRow, pacmanStartCol);
            moveComp->SetIsActive(true);
        }

        auto renderComp = m_pMissPacman->GetComponent<dae::RenderComponent>();
        if (renderComp)
        {
            renderComp->SetVisible(true);
        }

        auto livesComp = m_pMissPacman->GetComponent<dae::LivesComponent>();
        if (livesComp)
        {
            livesComp->Respawn();
        }
    }

    if (m_pGameScene && m_pGhostManager)
    {
        GhostManagerComponent* ghostManager = m_pGhostManager->GetComponent<GhostManagerComponent>();
        if (ghostManager)
        {
            ghostManager->ResetGhostsForNewLife();
        }
    }
}

void PlayingState::ResetForNewLevel()
{
    if (m_pMissPacman)
    {
        auto moveComp = m_pMissPacman->GetComponent<PlayerMovementComponent>();
        if (moveComp)
        {
            int pacmanStartRow = 23; int pacmanStartCol = 13;
            moveComp->SnapToGrid(pacmanStartRow, pacmanStartCol);
            moveComp->SetIsActive(true);
        }

        auto renderComp = m_pMissPacman->GetComponent<dae::RenderComponent>();
        if (renderComp) renderComp->SetVisible(true);
    }


    if (m_pGhostManager)
    {
        GhostManagerComponent* ghostManager = m_pGhostManager->GetComponent<GhostManagerComponent>();
        if (ghostManager)
        {
            ghostManager->ResetGhostsForNewLevel();
        }
    }

    m_isPlayerRespawning = false;
    m_playerRespawnTimer = 0.f;
}

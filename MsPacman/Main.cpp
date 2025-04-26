
#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

//Windows
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

//GameEngine
#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Scene.h"

//Components
#include "TextComponent.h"
#include "RenderComponent.h"
#include "FpsComponent.h"
#include "LivesDisplayComponent.h"
#include "ScoreComponent.h"
#include "ScoreDisplayComponent.h"
#include "LivesComponent.h"
#include "LevelGridComponent.h"
#include "PlayerMovementComponent.h"

//Commands
#include "MoveCommand.h"
#include "Xinput.h"
#include "DieCommand.h"
#include "GainPointsCommand.h"

//Observers
#include "Subject.h"
#include <ControllerInput.h>
#include <SDL_scancode.h>


void InitializePlayer(dae::Scene& scene, std::shared_ptr<dae::GameObject> levelObject);
void load();

int main(int, char*)
{
	dae::Minigin engine("../Data/");
	engine.Run(load);
	return 0;
}

void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Assignment3");

	//Make Background
	auto backgroundObject = std::make_shared<dae::GameObject>();
	auto textureComponent = std::make_unique<dae::RenderComponent>(backgroundObject, 1280.f, 720.f);
	textureComponent->SetTexture("background.tga");
	backgroundObject->AddComponent(std::move(textureComponent));
	scene.Add(backgroundObject);

	//Make Logo DAE
	auto logoObject = std::make_shared<dae::GameObject>();
	textureComponent = std::make_unique<dae::RenderComponent>(logoObject);
	textureComponent->SetTexture("logo.tga");
	logoObject->SetLocalPosition(1000.f, 620.f);
	logoObject->AddComponent(std::move(textureComponent));
	scene.Add(logoObject);

	//Make Text
	auto textObject = std::make_shared<dae::GameObject>();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto textComponent = std::make_unique<dae::TextComponent>(textObject, "Programming 4 Assignment", std::move(font));
	textObject->SetLocalPosition(420.f, 5.f);
	textObject->AddComponent(std::move(textComponent));
	scene.Add(textObject);

	// Test Level
	auto levelObject = std::make_shared<dae::GameObject>();
	float desiredTileSize = 20.0f; // Size in pixels
	auto levelGridComponent = std::make_unique<LevelGridComponent>(levelObject, desiredTileSize);
	levelGridComponent->LoadLevel("../Data/MsPacman_Level1.csv");

	levelObject->SetLocalPosition(370.f, 50.f);
	levelObject->AddComponent(std::move(levelGridComponent));
	scene.Add(levelObject);

	InitializePlayer(scene, levelObject);
}

void InitializePlayer(dae::Scene& scene, std::shared_ptr<dae::GameObject> levelObject)
{
	//Make UI Display
	auto displayLivesPlayer1Object = std::make_shared<dae::GameObject>();
	auto displayScorePlayer1Object = std::make_shared<dae::GameObject>();

	//Add Observer to Subject
	auto playerDieEvent = std::make_unique<dae::Subject>();
	auto displayLivesPlayer1Component = std::make_unique<dae::LivesDisplayComponent>(displayLivesPlayer1Object);
	playerDieEvent->AddObserver(displayLivesPlayer1Component.get());

	auto textComponent = std::make_unique<dae::TextComponent>(displayLivesPlayer1Object, "# Lives Player1", dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20));
	displayLivesPlayer1Object->AddComponent(std::move(textComponent));
	displayLivesPlayer1Object->AddComponent(std::move(displayLivesPlayer1Component));
	displayLivesPlayer1Object->SetLocalPosition(10, 200);
	scene.Add(displayLivesPlayer1Object);

	auto playerGainPointsEvent = std::make_unique<dae::Subject>();
	auto displayScorePlayer1Component = std::make_unique<dae::ScoreDisplayComponent>(displayScorePlayer1Object);
	playerGainPointsEvent->AddObserver(displayScorePlayer1Component.get());

	textComponent = std::make_unique<dae::TextComponent>(displayScorePlayer1Object, "Score: 0", dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20));
	displayScorePlayer1Object->AddComponent(std::move(textComponent));
	displayScorePlayer1Object->AddComponent(std::move(displayScorePlayer1Component));
	displayScorePlayer1Object->SetLocalPosition(10, 300);
	scene.Add(displayScorePlayer1Object);


	//Make MissPacMan
	auto missPacManObject = std::make_shared<dae::GameObject>();
	auto textureComponent = std::make_unique<dae::RenderComponent>(missPacManObject);
	auto livesComponent = std::make_unique<dae::LivesComponent>(missPacManObject, std::move(playerDieEvent), 3);
	auto scoreComponent = std::make_unique<dae::ScoreComponent>(missPacManObject, std::move(playerGainPointsEvent));

	const float movementSpeedPlayer1 = 80.f;
	auto movementComponent = std::make_unique<PlayerMovementComponent>(missPacManObject, levelObject, movementSpeedPlayer1);

	textureComponent->SetTexture("MissPacMan.png");
	missPacManObject->AddComponent(std::move(textureComponent));
	missPacManObject->AddComponent(std::move(livesComponent));
	missPacManObject->AddComponent(std::move(scoreComponent));
	missPacManObject->AddComponent(std::move(movementComponent));

	// Set initial grid position
	auto* pMoveComp = missPacManObject->GetComponent<PlayerMovementComponent>();
	int startRow = 23;
	int startCol = 13;
	if (pMoveComp) {
		pMoveComp->SnapToGrid(startRow, startCol); 
	}

	scene.Add(missPacManObject);

	//Set Keybinds
	auto& input = dae::InputManager::GetInstance();
	auto MoveUpCommand = std::make_unique<dae::MoveCommand>(missPacManObject, dae::Direction::Up);
	auto MoveLeftCommand = std::make_unique<dae::MoveCommand>(missPacManObject, dae::Direction::Left);
	auto MoveRightCommand = std::make_unique<dae::MoveCommand>(missPacManObject, dae::Direction::Right);
	auto MoveDownCommand = std::make_unique<dae::MoveCommand>(missPacManObject, dae::Direction::Down);

	input.BindKey(SDL_SCANCODE_W, dae::KeyState::Down, std::move(MoveUpCommand));
	input.BindKey(SDL_SCANCODE_A, dae::KeyState::Down, std::move(MoveLeftCommand));
	input.BindKey(SDL_SCANCODE_D, dae::KeyState::Down, std::move(MoveRightCommand));
	input.BindKey(SDL_SCANCODE_S, dae::KeyState::Down, std::move(MoveDownCommand));
	//input.UnbindKey(SDL_SCANCODE_W);

	auto DieCommandPlayer1 = std::make_unique<dae::DieCommand>(missPacManObject);
	auto GainPointsCommandPlayer1 = std::make_unique<dae::GainPointsCommand>(missPacManObject);

	input.BindKey(SDL_SCANCODE_Z, dae::KeyState::Up, std::move(DieCommandPlayer1));
	input.BindKey(SDL_SCANCODE_X, dae::KeyState::Up, std::move(GainPointsCommandPlayer1));

}
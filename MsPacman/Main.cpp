
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <profileapi.h> 


#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "GameObject.h"

//Commands
#include "MoveCommand.h"
#include "DieCommand.h"
#include "GainPointsCommand.h"
#include "ControllerInput.h"
#include <Xinput.h>

//Components
#include "TextComponent.h"
#include "RenderComponent.h"
#include "FpsComponent.h"
#include "Scene.h"
#include "RotatorComponent.h"
#include "LivesComponent.h"
#include "LivesDisplayComponent.h"
#include "ScoreComponent.h"
#include "ScoreDisplayComponent.h"

//Observers
#include "Subject.h"

// Define functions
void InitializePlayer(dae::Scene& scene);

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
	logoObject->SetLocalPosition(550, 180);
	logoObject->AddComponent(std::move(textureComponent));
	scene.Add(logoObject);

	//Make Text
	auto textObject = std::make_shared<dae::GameObject>();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto textComponent = std::make_unique<dae::TextComponent>(textObject, "Programming 4 Assignment", std::move(font));
	textObject->SetLocalPosition(420, 2);
	textObject->AddComponent(std::move(textComponent));
	scene.Add(textObject);

	//Make FPS
	auto fpsObject = std::make_shared<dae::GameObject>();
	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 50);
	auto fpsComponent = std::make_unique<dae::FpsComponent>(std::move(font), fpsObject);
	fpsObject->SetLocalPosition(550, 70);
	fpsObject->AddComponent(std::move(fpsComponent));
	scene.Add(fpsObject);

	//Make Player 1 Instructions
	textObject = std::make_shared<dae::GameObject>();
	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
	textComponent = std::make_unique<dae::TextComponent>(textObject, "Use the D-Pad to move Blue Ghost, X to lose a life, Y to gain points", std::move(font));
	textObject->SetLocalPosition(20, 680);
	textObject->AddComponent(std::move(textComponent));
	scene.Add(textObject);

	//Make Player 2 Instructions
	textObject = std::make_shared<dae::GameObject>();
	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
	textComponent = std::make_unique<dae::TextComponent>(textObject, "Use WASD to move Ms Pacman, Z to lose a life, X to gain points", std::move(font));
	textObject->SetLocalPosition(20, 660);
	textObject->AddComponent(std::move(textComponent));
	scene.Add(textObject);

	InitializePlayer(scene);
}

int main(int, char* []) {
	dae::Minigin engine("../Data/");
	engine.Run(load);
	return 0;
}

void InitializePlayer(dae::Scene& scene)
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
	textureComponent->SetTexture("MissPacMan.png");
	missPacManObject->SetLocalPosition(600, 500);
	missPacManObject->AddComponent(std::move(textureComponent));
	missPacManObject->AddComponent(std::move(livesComponent));
	missPacManObject->AddComponent(std::move(scoreComponent));
	scene.Add(missPacManObject);

	//Make UI Display
	auto displayLivesPlayer2Object = std::make_shared<dae::GameObject>();
	auto displayScorePlayer2Object = std::make_shared<dae::GameObject>();

	//Add Observer to Subject
	playerDieEvent = std::make_unique<dae::Subject>();
	auto displayLivesPlayer2Component = std::make_unique<dae::LivesDisplayComponent>(displayLivesPlayer2Object);
	playerDieEvent->AddObserver(displayLivesPlayer2Component.get());

	textComponent = std::make_unique<dae::TextComponent>(displayLivesPlayer2Object, "# Lives Player2", dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20));
	displayLivesPlayer2Object->AddComponent(std::move(textComponent));
	displayLivesPlayer2Object->AddComponent(std::move(displayLivesPlayer2Component));
	displayLivesPlayer2Object->SetLocalPosition(10, 230);
	scene.Add(displayLivesPlayer2Object);

	playerGainPointsEvent = std::make_unique<dae::Subject>();
	auto displayScorePlayer2Component = std::make_unique<dae::ScoreDisplayComponent>(displayScorePlayer2Object);
	playerGainPointsEvent->AddObserver(displayScorePlayer2Component.get());

	textComponent = std::make_unique<dae::TextComponent>(displayScorePlayer2Object, "Score: 0", dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20));
	displayScorePlayer2Object->AddComponent(std::move(textComponent));
	displayScorePlayer2Object->AddComponent(std::move(displayScorePlayer2Component));
	displayScorePlayer2Object->SetLocalPosition(10, 330);
	scene.Add(displayScorePlayer2Object);

	//Make Ghost
	auto blueGhostObject = std::make_shared<dae::GameObject>();
	textureComponent = std::make_unique<dae::RenderComponent>(blueGhostObject);
	textureComponent->SetTexture("ScaredGhost.png");
	livesComponent = std::make_unique<dae::LivesComponent>(blueGhostObject, std::move(playerDieEvent), 3);
	scoreComponent = std::make_unique<dae::ScoreComponent>(blueGhostObject, std::move(playerGainPointsEvent));
	blueGhostObject->AddComponent(std::move(textureComponent));
	blueGhostObject->AddComponent(std::move(livesComponent));
	blueGhostObject->AddComponent(std::move(scoreComponent));
	blueGhostObject->SetLocalPosition(700, 500);
	scene.Add(blueGhostObject);

	//Set Keybinds
	auto& input = dae::InputManager::GetInstance();
	const float movementSpeedPlayer1 = 250.f;
	auto MoveUpCommand = std::make_unique<dae::MoveCommand>(missPacManObject, dae::Direction::Up, movementSpeedPlayer1);
	auto MoveLeftCommand = std::make_unique<dae::MoveCommand>(missPacManObject, dae::Direction::Left, movementSpeedPlayer1);
	auto MoveRightCommand = std::make_unique<dae::MoveCommand>(missPacManObject, dae::Direction::Right, movementSpeedPlayer1);
	auto MoveDownCommand = std::make_unique<dae::MoveCommand>(missPacManObject, dae::Direction::Down, movementSpeedPlayer1);

	input.BindKey(SDL_SCANCODE_W, dae::KeyState::Pressed, std::move(MoveUpCommand));
	input.BindKey(SDL_SCANCODE_A, dae::KeyState::Pressed, std::move(MoveLeftCommand));
	input.BindKey(SDL_SCANCODE_D, dae::KeyState::Pressed, std::move(MoveRightCommand));
	input.BindKey(SDL_SCANCODE_S, dae::KeyState::Pressed, std::move(MoveDownCommand));
	//input.UnbindKey(SDL_SCANCODE_W);

	auto DieCommandPlayer1 = std::make_unique<dae::DieCommand>(missPacManObject);
	auto GainPointsCommandPlayer1 = std::make_unique<dae::GainPointsCommand>(missPacManObject);

	input.BindKey(SDL_SCANCODE_Z, dae::KeyState::Up, std::move(DieCommandPlayer1));
	input.BindKey(SDL_SCANCODE_X, dae::KeyState::Up, std::move(GainPointsCommandPlayer1));

	//Set Controller Keybinds
	auto controllerInput = std::make_unique<dae::ControllerInput>();
	const float movementSpeedPlayer2 = 125.f;

	auto ControllerMoveUpCommand = std::make_unique<dae::MoveCommand>(blueGhostObject, dae::Direction::Up, movementSpeedPlayer2);
	auto ControllerMoveLeftCommand = std::make_unique<dae::MoveCommand>(blueGhostObject, dae::Direction::Left, movementSpeedPlayer2);
	auto ControllerMoveRightCommand = std::make_unique<dae::MoveCommand>(blueGhostObject, dae::Direction::Right, movementSpeedPlayer2);
	auto ControllerMoveDownCommand = std::make_unique<dae::MoveCommand>(blueGhostObject, dae::Direction::Down, movementSpeedPlayer2);

	controllerInput->BindButton(XINPUT_GAMEPAD_DPAD_UP, dae::ControllerState::Pressed, std::move(ControllerMoveUpCommand));
	controllerInput->BindButton(XINPUT_GAMEPAD_DPAD_LEFT, dae::ControllerState::Pressed, std::move(ControllerMoveLeftCommand));
	controllerInput->BindButton(XINPUT_GAMEPAD_DPAD_RIGHT, dae::ControllerState::Pressed, std::move(ControllerMoveRightCommand));
	controllerInput->BindButton(XINPUT_GAMEPAD_DPAD_DOWN, dae::ControllerState::Pressed, std::move(ControllerMoveDownCommand));

	auto DieCommandPlayer2 = std::make_unique<dae::DieCommand>(blueGhostObject);
	auto GainPointsCommandPlayer2 = std::make_unique<dae::GainPointsCommand>(blueGhostObject);

	controllerInput->BindButton(XINPUT_GAMEPAD_X, dae::ControllerState::Down, std::move(DieCommandPlayer2));
	controllerInput->BindButton(XINPUT_GAMEPAD_Y, dae::ControllerState::Down, std::move(GainPointsCommandPlayer2));

	input.AddController(std::move(controllerInput));
}
#define WIN32_LEAN_AND_MEAN 
#define SDL_MAIN_HANDLED
#include <windows.h>
#include <profileapi.h> 

#include <stdexcept>
#include <chrono>
#include <execution>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "Minigin.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TimeGameEngine.h"
#include "ServiceLocator.h" 
#include "SDLMixerAudioService.h"
#include "GameStateMachine.h" 
#include "backends/imgui_impl_sdl2.h"

SDL_Window* g_window{};

void PrintSDLVersion()
{
	SDL_version version{};
	SDL_VERSION(&version);
	printf("We compiled against SDL version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	SDL_GetVersion(&version);
	printf("We are linking against SDL version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_IMAGE_VERSION(&version);
	printf("We compiled against SDL_image version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *IMG_Linked_Version();
	printf("We are linking against SDL_image version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_TTF_VERSION(&version)
		printf("We compiled against SDL_ttf version %u.%u.%u ...\n",
			version.major, version.minor, version.patch);

	version = *TTF_Linked_Version();
	printf("We are linking against SDL_ttf version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_MIXER_VERSION(&version);
	printf("We compiled against SDL_mixer version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *Mix_Linked_Version();
	printf("We are linking against SDL_mixer version %u.%u.%u.\n",
		version.major, version.minor, version.patch);
}

dae::Minigin::Minigin(const std::string& dataPath)
{
	PrintSDLVersion();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	// Initialize SDL_mixer
	bool mixerInitialized = true;

	// Open the audio device 
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0) 
	{
		printf("SDL_mixer could not initialize main audio device! SDL_mixer Error: %s\n", Mix_GetError());
		mixerInitialized = false; 
	}

	// Initialize Service Locator 
	ServiceLocator::Initialize();

	// Register Real Audio Service 
	if (mixerInitialized)
	{
		std::cout << "Registering SDLMixerAudioService...\n";
		ServiceLocator::RegisterAudioService(std::make_unique<SDLMixerAudioService>());
	}
	else
	{
		std::cerr << "Audio initialization failed. Using NullAudioService.\n";
	}

	m_pGameStateMachine = std::make_unique<GameStateMachine>();

	g_window = SDL_CreateWindow
	(
		"Programming 4 assignment",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280, //original: 640x480
		720,
		SDL_WINDOW_OPENGL
	);

	if (g_window == nullptr)
	{
		if (mixerInitialized) Mix_CloseAudio();
		ServiceLocator::Shutdown();
		SDL_Quit();
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(g_window);
	ResourceManager::GetInstance().Init(dataPath);
}

dae::Minigin::~Minigin()
{

	m_pGameStateMachine.reset();

	ServiceLocator::Shutdown();
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_window);

	if (g_window)
	{
		SDL_DestroyWindow(g_window);
		g_window = nullptr;
		std::cout << "SDL Window destroyed.\n";
	}

	Mix_CloseAudio();
	while (Mix_Init(0)) Mix_Quit();

	SDL_Quit();
}

void dae::Minigin::Run(const std::function<void()>& load)
{
	load();

	if (!m_pGameStateMachine || m_pGameStateMachine->IsStackEmpty())
	{
		std::cerr << "No initial game state set!" << std::endl;
		return;
	}

	auto& sceneManager = SceneManager::GetInstance();
    auto& renderer = Renderer::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& time = Time::GetInstance();

	bool doContinue = true; // Quits Game

	while (doContinue)
	{
		time.Update(); // Update delta time

		// SDL event polling
		SDL_Event e;
		bool sdlWantsQuit = false;

		while (SDL_PollEvent(&e)) 
		{
			if (e.type == SDL_QUIT) 
			{
				sdlWantsQuit = true;
			}
			// For ImGui
			ImGui_ImplSDL2_ProcessEvent(&e); 
		}

		if (sdlWantsQuit || (m_pGameStateMachine && m_pGameStateMachine->WantsToQuit())) 
		{
			doContinue = false;
			continue;
		}

		// Handle input and update
		if (m_pGameStateMachine) 
		{
			m_pGameStateMachine->HandleInput();
			m_pGameStateMachine->Update(static_cast<float>(time.GetDeltaTime()));
		}
		input.ProcessInput();
		sceneManager.Update();
		sceneManager.LateUpdate();

		// Handle render
		renderer.BeginFrame();
		if (m_pGameStateMachine)
		{
			m_pGameStateMachine->Render();
		}
		renderer.EndFrame();

		const double ms_per_frame = 16.67; // 60 FPS
		const auto sleepTime = std::chrono::milliseconds(static_cast<int>(ms_per_frame)) -
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time.GetInstance().GetLastTime());

		if (sleepTime.count() > 0)
			std::this_thread::sleep_for(sleepTime);
	}
}

void dae::Minigin::SetInitialGameState(std::unique_ptr<GameState> state)
{
	if (m_pGameStateMachine && state)
	{
		m_pGameStateMachine->SetState(std::move(state));
	}
}

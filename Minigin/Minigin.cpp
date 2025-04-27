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
	int flags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG;
	int initted = Mix_Init(flags);
	bool mixerInitialized = true;

	if ((initted & flags) != flags) 
	{
		printf("Mix_Init: Failed to init required audio format support! Mix_Init: %s\n", Mix_GetError());
		mixerInitialized = false; 
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0)
	{
		printf("SDL_mixer could not initialize main audio device! SDL_mixer Error: %s\n", Mix_GetError());
		mixerInitialized = false; 
		if (initted != 0) Mix_Quit();
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

	g_window = SDL_CreateWindow(
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
		if (initted != 0) Mix_Quit();
		SDL_Quit();
		ServiceLocator::Shutdown(); 
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(g_window);
	ResourceManager::GetInstance().Init(dataPath);
}

dae::Minigin::~Minigin()
{
	ServiceLocator::Shutdown();

	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	Mix_CloseAudio();
	while (Mix_Init(0)) Mix_Quit();
	SDL_Quit();
}

void dae::Minigin::Run(const std::function<void()>& load)
{
	load();

	auto& renderer = Renderer::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& time = Time::GetInstance();

	bool doContinue = true; // Quits Game
	const double ms_per_frame = 16.67; // 60 FPS

	while (doContinue)
	{
		time.Update(); // Update delta time

		doContinue = input.ProcessInput();

		sceneManager.Update();
		sceneManager.LateUpdate();

		renderer.Render();

		const auto sleepTime = std::chrono::milliseconds(static_cast<int>(ms_per_frame)) -
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time.GetInstance().GetLastTime());

		if (sleepTime.count() > 0)
			std::this_thread::sleep_for(sleepTime);
	}
}

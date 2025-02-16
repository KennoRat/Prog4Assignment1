#include <stdexcept>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <chrono>
#include <execution>

#include "Minigin.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Time.h"

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
}

dae::Minigin::Minigin(const std::string &dataPath)
{
	PrintSDLVersion();
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	g_window = SDL_CreateWindow(
		"Programming 4 assignment",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	if (g_window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(g_window);

	ResourceManager::GetInstance().Init(dataPath);
}

dae::Minigin::~Minigin()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	SDL_Quit();
}

void dae::Minigin::Run(const std::function<void()>& load)
{
	load();

	auto& renderer = Renderer::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& input = InputManager::GetInstance();
	auto& time = Time::GetInstance();

	// todo: this update loop could use some work.

    bool doContinue = true; // Quits Game
	const double ms_per_frame = 16.67; // 60 FPS

    while (doContinue)
    {
		time.Update(); // Update delta time

        doContinue = input.ProcessInput();
		
		sceneManager.Update();
        
        renderer.Render();

		const auto sleepTime = std::chrono::milliseconds(static_cast<int>(ms_per_frame)) -
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time.GetInstance().GetLastTime());

		if (sleepTime.count() > 0)
			std::this_thread::sleep_for(sleepTime);
    }
}

// Update loop with fixed time step
//----
//bool doContinue = true; // Quits Game
//auto lastTime = std::chrono::high_resolution_clock::now();
//double lag = 0.0;
//const double ms_per_frame = 16.67; // 60 FPS
//
//while (doContinue)
//{
//	const auto currentTime = std::chrono::high_resolution_clock::now();
//	const std::chrono::duration<double> deltaTime = currentTime - lastTime;
//	lastTime = currentTime;
//	lag += deltaTime.count() * 1000.0; // Convert to milliseconds
//
//	doContinue = input.ProcessInput();
//
//	while (lag >= ms_per_frame) // Fixed time step for updating
//	{
//		sceneManager.Update();
//		lag -= ms_per_frame;
//	}
//
//	renderer.Render(); // Pass "lag / ms_per_second" for smooth motion if needed
//
//	const auto sleepTime = currentTime + std::chrono::milliseconds(static_cast<int>(ms_per_frame)) - std::chrono::high_resolution_clock::now();
//	std::this_thread::sleep_for(sleepTime); // Clamping the frame rate to 60 fps
//}

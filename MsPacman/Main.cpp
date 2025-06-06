
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
#include "GameStateMachine.h"
#include "MenuState.h"

#include <memory>

void LoadGameAndSetState(dae::Minigin& engine)
{
    // Set the initial state of the game
    if (engine.GetGameStateMachine())
    {
        engine.GetGameStateMachine()->SetState(std::make_unique<MenuState>());
        std::cout << "Initial state (MenuState) set." << std::endl;
    }
}

int main(int, char* [])
{
    dae::Minigin engine("../Data/");

    engine.Run([&engine]() 
        {
        LoadGameAndSetState(engine);
        });

    return 0;
}
#include "SetGoNextLevelCommand.h"
#include "PlayingState.h" 
#include <iostream> 

SetGoNextLevelCommand::SetGoNextLevelCommand(PlayingState* pPlayingState)
    : m_pPlayingState(pPlayingState)
{
}

void SetGoNextLevelCommand::Execute()
{
    if (m_pPlayingState)
    {
        std::cout << "SetGoNextLevelCommand: Setting GoNextLevel to true." << std::endl;
        m_pPlayingState->SignalGoNextLevel(); 
    }
}
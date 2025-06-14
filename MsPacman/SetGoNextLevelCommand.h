#pragma once
#include "Command.h" 


class PlayingState;

class SetGoNextLevelCommand final : public dae::Command
{
public:

    // Constructor
    explicit SetGoNextLevelCommand(PlayingState* pPlayingState);
    virtual ~SetGoNextLevelCommand() override = default;

    virtual void Execute() override;

private:

    PlayingState* m_pPlayingState;
};
#pragma once

#include "IAudioService.h"

namespace dae
{
    class NullAudioService final : public IAudioService
    {
    public:
        virtual ~NullAudioService() override = default;

        virtual void PlaySound(SoundId, float) override {}
        virtual SoundId LoadSound(const std::string&) override { return INVALID_SOUND_ID; }
        virtual void StopAllSounds() override {}
    };
}
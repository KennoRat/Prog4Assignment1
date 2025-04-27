#pragma once

#include <string>
#include <cstdint> 

namespace dae
{
    using SoundId = uint16_t;
    constexpr SoundId INVALID_SOUND_ID = 0; 

    class IAudioService
    {
    public:
        virtual ~IAudioService() = default;

        // Request to play a pre-loaded sound
        virtual void PlaySound(SoundId id, float volume = 1.0f) = 0;

        // Load a sound file and associate it with an ID
        virtual SoundId LoadSound(const std::string& filePath) = 0;

        virtual void StopAllSounds() = 0;

        // To do: add background music
        // virtual void PlayMusic(SoundId id, bool loop = true) = 0;
        // virtual void StopMusic() = 0;
        // virtual SoundId LoadMusic(const std::string& filePath) = 0;
    };
}
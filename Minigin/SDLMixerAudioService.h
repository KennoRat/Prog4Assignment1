#pragma once

#include "IAudioService.h"
#include <memory> 

namespace dae
{
    class SDLMixerAudioService final : public IAudioService
    {
    public:
        // Constructor
        SDLMixerAudioService();

        // Destructor
        virtual ~SDLMixerAudioService() override;

        // Rule of 5
        SDLMixerAudioService(const SDLMixerAudioService&) = delete;
        SDLMixerAudioService& operator=(const SDLMixerAudioService&) = delete;
        SDLMixerAudioService(SDLMixerAudioService&& other) noexcept;
        SDLMixerAudioService& operator=(SDLMixerAudioService&& other) noexcept;

        // Interface implementation
        virtual void PlaySound(SoundId id, float volume = 1.0f) override;
        virtual SoundId LoadSound(const std::string& filePath) override;
        virtual void StopAllSounds() override;

    private:
        class SDLMixerAudioServiceImpl;
        std::unique_ptr<SDLMixerAudioServiceImpl> m_pImpl;
    };
}
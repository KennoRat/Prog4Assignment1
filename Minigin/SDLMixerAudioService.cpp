#include "SDLMixerAudioService.h"
#include <SDL_mixer.h>
#include <SDL.h>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <map>
#include <atomic>
#include <iostream> 

namespace dae
{
    //Sound Request Structure
    enum class AudioRequestType
    {
        LOAD_SOUND,
        PLAY_SOUND,
        STOP_ALL
    };

    struct SoundRequest
    {
        AudioRequestType type = AudioRequestType::STOP_ALL;
        std::string filePath = ""; // load
        SoundId id = INVALID_SOUND_ID; // play
        float volume = 1.0f;       
    };

    // Implementation of SDLMixerAudioServiceImpl Class
    class SDLMixerAudioService::SDLMixerAudioServiceImpl
    {

    public:

        // Constructor
        SDLMixerAudioServiceImpl()
        {
            // Initialize SDL_mixer 
            if (SDL_WasInit(SDL_INIT_AUDIO) == 0)
            {
                std::cerr << "SDL_mixer Error: SDL Audio not initialized before creating service!\n";
                throw std::runtime_error("SDL Audio not initialized");
            }
            if (Mix_AllocateChannels(MAX_CHANNELS) != MAX_CHANNELS)
            {
                std::cerr << "SDL_mixer Warning: Could not allocate requested channels: " << Mix_GetError() << std::endl;
            }


            // Start the audio processing thread
            m_AudioThread = std::thread(&SDLMixerAudioServiceImpl::AudioThreadLoop, this);
            std::cout << "Audio Service: Thread started.\n";
        }

        // Destructor
        ~SDLMixerAudioServiceImpl()
        {
            m_StopThread.store(true);
            m_QueueCondition.notify_one();

            // Wait for the thread to finish
            if (m_AudioThread.joinable())
            {
                m_AudioThread.join();
            }
            std::cout << "Audio Service: Thread joined.\n";


            // Cleanup 
            std::cout << "Audio Service: Cleaning up loaded sounds...\n";
            for (auto const& [id, chunk] : m_LoadedSounds)
            {
                if (chunk) Mix_FreeChunk(chunk);
                std::cout << "Freed chunk for ID: " << id << std::endl;
            }
            m_LoadedSounds.clear();
            m_SoundPathToId.clear();
        }

        // Rule of 5
        SDLMixerAudioServiceImpl(const SDLMixerAudioServiceImpl&) = delete;
        SDLMixerAudioServiceImpl& operator=(const SDLMixerAudioServiceImpl&) = delete;
        SDLMixerAudioServiceImpl(SDLMixerAudioServiceImpl&&) = delete;
        SDLMixerAudioServiceImpl& operator=(SDLMixerAudioServiceImpl&&) = delete;

        // Public Methods

        void EnqueueRequest(SoundRequest&& request)
        {
            // Prevent adding requests if stopping
            if (m_StopThread.load()) return;
            { 
               std::lock_guard<std::mutex> lock(m_QueueMutex);
               m_RequestQueue.push(std::move(request));
            } 
            m_QueueCondition.notify_one();
        }

        SoundId GenerateNewSoundId()
        {
            // Basic ID generation
            if (m_LoadedSounds.size() >= MAX_SOUNDS)
            {
                std::cerr << "Audio Service Error: Maximum number of sounds loaded (" << MAX_SOUNDS << ")\n";
                return INVALID_SOUND_ID;
            }
            return m_NextSoundId++;
        }

    public:
        // Track file path to avoid duplicates
        std::map<std::string, SoundId> m_SoundPathToId;
    
    private:
        static constexpr size_t MAX_SOUNDS = 256;
        static constexpr int MAX_CHANNELS = 16;

        std::thread m_AudioThread;
        std::mutex m_QueueMutex;
        std::condition_variable m_QueueCondition;
        std::queue<SoundRequest> m_RequestQueue;
        std::atomic<bool> m_StopThread{ false };

        // Store loaded sounds (chunks)
        std::map<SoundId, Mix_Chunk*> m_LoadedSounds;
        SoundId m_NextSoundId{ 1 };

    private:
        // Audio Thread Logic
        void AudioThreadLoop()
        {
            while (!m_StopThread.load())
            {
                SoundRequest currentRequest;
                bool requestAvailable = false;

                std::unique_lock<std::mutex> lock(m_QueueMutex);

                // Wait for queue request
                m_QueueCondition.wait(lock, [this] { return !m_RequestQueue.empty() || m_StopThread.load(); });

                if (m_StopThread.load()) break;

                if (!m_RequestQueue.empty())
                {
                    currentRequest = std::move(m_RequestQueue.front());
                    m_RequestQueue.pop();
                    requestAvailable = true;
                }

                // Process the request
                if (requestAvailable)
                {
                    ProcessRequest(currentRequest);
                }
            }
            std::cout << "Audio Service: Thread loop exiting.\n";
        }

        void ProcessRequest(const SoundRequest& request)
        {
            switch (request.type)
            {
            case AudioRequestType::LOAD_SOUND:
                HandleLoadSound(request.filePath, request.id);
                break;
            case AudioRequestType::PLAY_SOUND:
                HandlePlaySound(request.id, request.volume);
                break;
            case AudioRequestType::STOP_ALL:
                HandleStopAllSounds();
                break;
            }
        }

        // Hnadle Sound Logic
        void HandleLoadSound(const std::string& filePath, SoundId id)
        {
            if (id == INVALID_SOUND_ID) return;
             // Load the sound chunk
            Mix_Chunk* pChunk = Mix_LoadWAV(filePath.c_str());
            if (!pChunk)
            {
                std::cerr << "SDL_mixer Error: Failed to load sound '" << filePath << "': " << Mix_GetError() << std::endl;
                m_LoadedSounds[id] = nullptr; 
            }
            else
            {
                std::cout << "Audio Service: Loaded sound '" << filePath << "' with ID " << id << std::endl;
                m_LoadedSounds[id] = pChunk;
            }
        }

        void HandlePlaySound(SoundId id, float volume)
        {
            if (m_LoadedSounds.count(id))
            {
                Mix_Chunk* pChunk = m_LoadedSounds[id];
                if (pChunk)
                {
                    // Set chunk volume
                    int mixVolume = static_cast<int>(volume * MIX_MAX_VOLUME); // MIX_MAX_VOLUME is 128
                    Mix_VolumeChunk(pChunk, mixVolume);

                    // Play on the first available channel
                    int channel = Mix_PlayChannel(-1, pChunk, 0); // 0 loops means play once
                    if (channel == -1)
                    {
                        std::cerr << "SDL_mixer Error: Could not play sound ID " << id << " (no free channels?): " << Mix_GetError() << std::endl;
                    }
                    else {
                        // std::cout << "Playing sound ID " << id << " on channel " << channel << std::endl;
                    }
                }
                else
                {
                    std::cerr << "Audio Service Warning: Attempted to play sound ID " << id << " which failed to load.\n";
                }
            }
            else
            {
                std::cerr << "Audio Service Warning: Attempted to play unloaded sound ID " << id << std::endl;
            }
        }

        void HandleStopAllSounds()
        {
            Mix_HaltChannel(-1);
            std::cout << "Audio Service: Stopped all sounds.\n";
        }
    };


    // SDLMixerAudioService Method Implementations 

    SDLMixerAudioService::SDLMixerAudioService()
        : m_pImpl{ std::make_unique<SDLMixerAudioServiceImpl>() }
    {
        std::cout << "SDLMixerAudioService created.\n";
    }

    SDLMixerAudioService::~SDLMixerAudioService()
    {
        std::cout << "SDLMixerAudioService destroyed.\n";
    }

    // Move Operations
    SDLMixerAudioService::SDLMixerAudioService(SDLMixerAudioService&& other) noexcept = default;
    SDLMixerAudioService& SDLMixerAudioService::operator=(SDLMixerAudioService&& other) noexcept = default;


    // Interface Method Implementations 

    void SDLMixerAudioService::PlaySound(SoundId id, float volume)
    {
        if (id != INVALID_SOUND_ID)
        {
            SoundRequest request;
            request.type = AudioRequestType::PLAY_SOUND;
            request.id = id;
            request.volume = std::max(0.0f, std::min(1.0f, volume)); // Clamp volume just in case
            m_pImpl->EnqueueRequest(std::move(request));
        }
    }

    SoundId SDLMixerAudioService::LoadSound(const std::string& filePath)
    {
        // Basic check if path already loaded 
        if (m_pImpl->m_SoundPathToId.count(filePath)) {
            return m_pImpl->m_SoundPathToId[filePath];
        }

        SoundId newId = m_pImpl->GenerateNewSoundId();
        if (newId != INVALID_SOUND_ID)
        {
            SoundRequest request;
            request.type = AudioRequestType::LOAD_SOUND;
            request.filePath = filePath;
            request.id = newId;
            m_pImpl->EnqueueRequest(std::move(request));
            m_pImpl->m_SoundPathToId[filePath] = newId;

            return newId;
        }
        return INVALID_SOUND_ID; 
    }

    void SDLMixerAudioService::StopAllSounds()
    {
        SoundRequest request;
        request.type = AudioRequestType::STOP_ALL;
        m_pImpl->EnqueueRequest(std::move(request));
    }

} 
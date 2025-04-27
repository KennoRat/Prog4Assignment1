#pragma once

#include "IAudioService.h"
#include "NullAudioService.h" 
#include <memory> 
#include <iostream>

namespace dae
{
    class ServiceLocator final
    {
    public:
        // Delete for static class
        ServiceLocator() = delete;
        ~ServiceLocator() = delete;
        ServiceLocator(const ServiceLocator&) = delete;
        ServiceLocator(ServiceLocator&&) = delete;
        ServiceLocator& operator=(const ServiceLocator&) = delete;
        ServiceLocator& operator=(ServiceLocator&&) = delete;

        // Call this once at initialization
        static void Initialize()
        {
            m_pAudioServiceInstance = std::make_unique<NullAudioService>();
            std::cout << "ServiceLocator Initialized with NullAudioService.\n";
        }

        static void Shutdown()
        {
            if (m_pAudioServiceInstance) {
                std::cout << "ServiceLocator Shutting Down Audio Service...\n";
            }
            m_pAudioServiceInstance.reset(); // Explicitly destroy service
            std::cout << "ServiceLocator Shutdown Complete.\n";
        }

        static IAudioService& GetAudioService()
        {
            if (!m_pAudioServiceInstance) {
                std::cerr << "ServiceLocator Error: Audio service accessed before initialization!\n";
                Initialize();
            }
            return *m_pAudioServiceInstance;
        }

        static void RegisterAudioService(std::unique_ptr<IAudioService>&& pService)
        {
            if (pService == nullptr)
            {
                m_pAudioServiceInstance = std::make_unique<NullAudioService>();
                std::cout << "ServiceLocator Registered NullAudioService (null provided).\n";
            }
            else
            {
                std::cout << "ServiceLocator Replacing Audio Service...\n";
                m_pAudioServiceInstance = std::move(pService);
                std::cout << "ServiceLocator Audio Service Registered.\n";
            }
        }

    private:

        // Static members for service storage
        static std::unique_ptr<IAudioService> m_pAudioServiceInstance;
    };
} 
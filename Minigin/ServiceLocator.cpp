#include "ServiceLocator.h"

namespace dae
{
    // initialize here for safety
    std::unique_ptr<IAudioService> ServiceLocator::m_pAudioServiceInstance = std::make_unique<NullAudioService>();

} 
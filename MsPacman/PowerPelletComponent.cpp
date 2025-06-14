#include "PowerPelletComponent.h"
#include <iostream>
#include <stdexcept>

#include "GameObject.h"    
#include "Event.h"        
#include "GameEvents.h"
#include "TimeGameEngine.h"

PowerPelletComponent::PowerPelletComponent(std::shared_ptr<dae::GameObject> owner)
: dae::BaseComponent( *owner)
{
    m_pPowerUpActivatedSubject = std::make_unique<dae::Subject>();
}

void PowerPelletComponent::ActivatePowerUp()
{
    if (m_pPowerUpActivatedSubject)
    {
        m_pPowerUpActivatedSubject->NotifyObservers(*GetGameObject(), dae::Event{ EVENT_POWER_UP_ACTIVATED });
    }
}

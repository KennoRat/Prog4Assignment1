#include "LivesComponent.h"
#include "Subject.h"
#include "Event.h"
#include "GameEvents.h"

using namespace dae;

LivesComponent::LivesComponent(std::shared_ptr<GameObject> gameObject, std::unique_ptr<Subject> deathSubject, std::unique_ptr<Subject> respawnSubject, int startingLives)
	:BaseComponent(*gameObject.get()),
	m_pObjectDiedEvent(std::move(deathSubject)),
	m_pObjectRespawnedEvent(std::move(respawnSubject)),
	m_lives(startingLives)
{
}

void LivesComponent::Update()
{
	//Do nothing
}

void LivesComponent::Render() const
{
	//Do nothing
}

void LivesComponent::RenderImGui()
{
	//Do nothing
}

void LivesComponent::Die()
{
	if (m_lives <= 0) return;

	--m_lives;

	if (m_pObjectDiedEvent)
	{
		m_pObjectDiedEvent->NotifyObservers(*GetGameObject(), Event{ EVENT_PLAYER_DIED });
	}
}

void LivesComponent::Respawn()
{
	if (m_pObjectRespawnedEvent)
	{
		m_pObjectRespawnedEvent->NotifyObservers(*GetGameObject(), Event{ EVENT_PLAYER_RESPAWNED });
	}
}

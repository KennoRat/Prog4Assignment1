#include "LivesComponent.h"
#include "Subject.h"
#include "Event.h"

using namespace dae;

LivesComponent::LivesComponent(std::shared_ptr<GameObject> gameObject, std::unique_ptr<Subject> subject, int startingLives)
	:BaseComponent(*gameObject.get()), m_pObjectDiedEvent(std::move(subject)), m_lives(startingLives)
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
	--m_lives;

	m_pObjectDiedEvent->NotifyObservers(*GetGameObject(), Event{ make_sdbm_hash("PlayerDied") });
}

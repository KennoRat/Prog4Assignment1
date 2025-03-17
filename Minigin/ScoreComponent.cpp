#include "ScoreComponent.h"
#include "Subject.h"
#include "Event.h"

using namespace dae;

ScoreComponent::ScoreComponent(std::shared_ptr<GameObject> gameObject, std::unique_ptr<Subject> subject)
	:BaseComponent(*gameObject.get()), m_pScoreEvent(std::move(subject)), m_points(0)
{
}

void ScoreComponent::Update()
{
	//Do nothing
}

void ScoreComponent::Render() const
{
	//Do nothing
}

void ScoreComponent::RenderImGui()
{
	//Do nothing
}

void ScoreComponent::GainPoints(int amount)
{
	m_points += amount;

	m_pScoreEvent->NotifyObservers(*GetGameObject(), Event{ make_sdbm_hash("PointsGained") });
}

#include "ScoreDisplayComponent.h"
#include "GameObject.h"
#include "Event.h"
#include "ScoreComponent.h"

using namespace dae;

ScoreDisplayComponent::ScoreDisplayComponent(std::shared_ptr<GameObject> gameObject)
	:BaseComponent(*gameObject.get())
{
}

void ScoreDisplayComponent::Update()
{
	//Do nothing
}

void ScoreDisplayComponent::Render() const
{
	//Do nothing
}

void ScoreDisplayComponent::RenderImGui()
{
	//Do nothing
}

void ScoreDisplayComponent::Notify(const GameObject& game_object, const Event& event)
{
	if (event.id == make_sdbm_hash("PointsGained"))
	{
		auto textComponent = GetGameObject()->GetComponent<TextComponent>();
		auto scoreComponent = game_object.GetComponent<ScoreComponent>();
		textComponent->SetText("Score: " + std::to_string(scoreComponent->GetPoints()));
	}
}

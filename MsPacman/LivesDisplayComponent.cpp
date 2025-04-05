#include "LivesDisplayComponent.h"
#include "GameObject.h"
#include "Event.h"
#include "LivesComponent.h"

using namespace dae;

LivesDisplayComponent::LivesDisplayComponent(std::shared_ptr<GameObject> gameObject)
	:BaseComponent(*gameObject.get())
{
}

void LivesDisplayComponent::Update()
{
	//Do nothing
}

void LivesDisplayComponent::Render() const
{
	//Do nothing
}

void LivesDisplayComponent::RenderImGui()
{
	//Do nothing
}

void LivesDisplayComponent::Notify(const GameObject& game_object, const Event& event)
{
	if (event.id == make_sdbm_hash("PlayerDied"))
	{
		auto textComponent = GetGameObject()->GetComponent<TextComponent>();
		auto livesComponent = game_object.GetComponent<LivesComponent>();
		textComponent->SetText("# Lives: " + std::to_string(livesComponent->GetLives()));
	}
}

#include <format>

#include "FpsComponent.h"
#include "TimeGameEngine.h"

using namespace dae;

FpsComponent::FpsComponent(std::shared_ptr<Font> font, std::shared_ptr<GameObject> gameObject)
	: BaseComponent(*gameObject.get())
{
	//m_textComponent = new TextComponent("FPS: 0", font, gameObject);
	m_textComponent = std::make_unique<TextComponent>(gameObject, "FPS: 0", font);
}

void FpsComponent::Update()
{
	m_textComponent->Update(); //don't call update in update this should be done in gameObject

	double fps = 1.0 / Time::GetInstance().GetDeltaTime();
	m_textComponent->SetText(std::format("FPS: {:.1f}", fps));
}

void FpsComponent::Render() const
{
	m_textComponent->Render(); //don't call render in render
}

void FpsComponent::RenderImGui()
{
	//Do nothing
}

#include <format>

#include "FpsComponent.h"
#include "Time.h"

using namespace dae;

FpsComponent::FpsComponent(std::shared_ptr<Font> font, std::shared_ptr<GameObject> gameObject)
	: BaseComponent(*gameObject.get())
{
	//m_textComponent = new TextComponent("FPS: 0", font, gameObject);
	m_textComponent = std::make_unique<TextComponent>("FPS: 0", font, gameObject);
}

void FpsComponent::Update()
{
	m_textComponent->Update();

	double fps = 1.0 / Time::GetInstance().GetDeltaTime();
	m_textComponent->SetText(std::format("FPS: {:.1f}", fps));
}

void dae::FpsComponent::Render() const
{
	m_textComponent->Render();
}

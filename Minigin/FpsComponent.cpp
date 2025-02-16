#include "FpsComponent.h"
#include "Time.h"
#include <format>


using namespace dae;

FpsComponent::FpsComponent(const std::string& text, std::shared_ptr<Font> font, std::shared_ptr<GameObject> gameObjrct)
	: TextComponent(text, font, gameObjrct)
{
}

void FpsComponent::Update()
{
	TextComponent::Update();

	double fps = 1.0 / Time::GetInstance().GetDeltaTime();
	TextComponent::SetText(std::format("FPS: {:.1f}", fps));
}

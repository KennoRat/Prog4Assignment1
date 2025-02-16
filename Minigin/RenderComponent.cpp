#include "RenderComponent.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"

using namespace dae;

RenderComponent::RenderComponent(std::shared_ptr<GameObject> gameObject):BaseComponent(gameObject)
{
}

dae::RenderComponent::~RenderComponent()
{
	m_gameObject.reset();
	m_texture.reset();
}

void dae::RenderComponent::Update()
{
	// Not used
}

void RenderComponent::Render() const
{
	if (!m_texture)
	{
		return;
	}

	if (auto gameObject = m_gameObject.lock())
	{
		const auto& pos = gameObject->GetTransform().GetPosition();
		Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
	}
}
	

void RenderComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

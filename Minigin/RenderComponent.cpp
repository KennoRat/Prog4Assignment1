#include "RenderComponent.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"

using namespace dae;

RenderComponent::RenderComponent(std::shared_ptr<GameObject> gameObject):BaseComponent(*gameObject.get())
{
}

RenderComponent::~RenderComponent()
{
	m_texture.reset();
}

void RenderComponent::Update()
{
	// Not used
}

void RenderComponent::Render() const
{
	if (!m_texture)
	{
		return;
	}

	const auto& pos = GetGameObject()->GetWorldPosition().GetPosition();
	Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}
	
void RenderComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

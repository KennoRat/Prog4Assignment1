#include "RenderComponent.h"
#include "Texture2D.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"

using namespace dae;

RenderComponent::RenderComponent(std::shared_ptr<GameObject> gameObject)
	:RenderComponent(gameObject, 0, 0)
{
}

RenderComponent::RenderComponent(std::shared_ptr<GameObject> gameObject, float width, float height)
	:BaseComponent(gameObject ? *gameObject : throw std::runtime_error("Null GameObject!"))
	, m_width{width}, m_height{height}
{
	if (!gameObject)
	{
		throw std::runtime_error("RenderComponent received a nullptr GameObject!");
	}
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
	Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y, m_width, m_height);
}

void RenderComponent::RenderImGui()
{
	//Do nothing
}
	
void RenderComponent::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
	if (m_width == 0.f && m_height == 0.f)
	{
		m_width = static_cast<float>(m_texture->GetSize().x);
		m_height = static_cast<float>(m_texture->GetSize().y);
	}
}

#include "RenderComponent.h"
#include "Texture2D.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"

using namespace dae;

RenderComponent::RenderComponent(std::shared_ptr<GameObject> gameObject, float renderWidth, float renderHeight)
	: BaseComponent(*gameObject),
	m_renderWidth(renderWidth),
	m_renderHeight(renderHeight)
{
}

RenderComponent::~RenderComponent()
{
	m_pTexture.reset();
}

void RenderComponent::Update()
{
	// Not used
}

void RenderComponent::Render() const
{
    if (!m_pTexture || !m_isVisible)
	{
		return;
	}

    const auto& pos = GetGameObject()->GetWorldPosition().GetPosition();
    auto& renderer = Renderer::GetInstance();

    if (m_useSourceRect)
    {
        if (m_renderWidth > 0.f && m_renderHeight > 0.f) 
        {
            renderer.RenderTextureSpriteSheet(*m_pTexture, pos.x, pos.y, m_renderWidth, m_renderHeight, m_sourceRect);
        }
        else 
        {
            renderer.RenderTextureSpriteSheet(*m_pTexture, pos.x, pos.y, m_sourceRect);
        }
    }
    else
    {
        if (m_renderWidth > 0.f && m_renderHeight > 0.f) {
            renderer.RenderTexture(*m_pTexture, pos.x, pos.y, m_renderWidth, m_renderHeight);
        }
        else {
            renderer.RenderTexture(*m_pTexture, pos.x, pos.y);
        }
    }
}

void RenderComponent::RenderImGui()
{
	//Do nothing
}
	
void RenderComponent::SetTexture(const std::string& filename)
{
	m_pTexture = ResourceManager::GetInstance().LoadTexture(filename);

    if (m_renderWidth == 0.f && m_renderHeight == 0.f && !m_useSourceRect)
    {
        const auto textureSize = m_pTexture->GetSize();
        m_renderWidth = static_cast<float>(textureSize.x);
        m_renderHeight = static_cast<float>(textureSize.y);
    }
}

void RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
    m_pTexture = texture;
    
    if (m_renderWidth == 0.f && m_renderHeight == 0.f && !m_useSourceRect)
    {
        const auto textureSize = m_pTexture->GetSize();
        m_renderWidth = static_cast<float>(textureSize.x);
        m_renderHeight = static_cast<float>(textureSize.y);
    }
}

void RenderComponent::SetSourceRect(const SDL_Rect& srcRect)
{
    m_sourceRect = srcRect;
    m_useSourceRect = true;

    if (m_renderWidth == 0.f && m_renderHeight == 0.f) 
    {
        m_renderWidth = static_cast<float>(m_sourceRect.w);
        m_renderHeight = static_cast<float>(m_sourceRect.h);
    }
}

void RenderComponent::SetSourceRect(int x, int y, int w, int h)
{
    SetSourceRect({ x, y, w, h });
}

void RenderComponent::SetRenderDimensions(float width, float height)
{
    m_renderWidth = width;
    m_renderHeight = height;
}

void RenderComponent::SetVisible(bool visible)
{
    m_isVisible = visible;
}

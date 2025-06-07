#include "DimOverlayComponent.h"
#include "GameObject.h"   
#include "Renderer.h"      

using namespace dae;

DimOverlayComponent::DimOverlayComponent(std::shared_ptr<dae::GameObject> owner, SDL_Color color, float screenWidth, float screenHeight)
    : BaseComponent(*owner),
    m_Color(color),
    m_ScreenWidth(screenWidth),
    m_ScreenHeight(screenHeight)
{
}

void DimOverlayComponent::Update()
{
    // Nothing
}

void DimOverlayComponent::Render() const
{
    dae::Renderer::GetInstance().RenderFilledRect(0.f, 0.f, m_ScreenWidth, m_ScreenHeight, m_Color);
}

void DimOverlayComponent::RenderImGui()
{
    // Nothing
}
#pragma once
#include "BaseComponent.h" 
#include <SDL_pixels.h>

namespace dae
{
    class GameObject;
}

class DimOverlayComponent final : public dae::BaseComponent
{
public:

    DimOverlayComponent(std::shared_ptr<dae::GameObject> owner, SDL_Color color, float screenWidth, float screenHeight);
    virtual ~DimOverlayComponent() override = default;

    virtual void Update() override; 
    virtual void Render() const override;
    virtual void RenderImGui() override;

    void SetColor(SDL_Color color) { m_Color = color; }

private:

    SDL_Color m_Color;
    float m_ScreenWidth;
    float m_ScreenHeight;
};
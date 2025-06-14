#pragma once

#include "BaseComponent.h" 
#include "Subject.h"       
#include <memory>         

namespace dae
{
    class GameObject;
}

class PowerPelletComponent final : public dae::BaseComponent
{
public:
    // Constructor
    explicit PowerPelletComponent(std::shared_ptr<dae::GameObject> owner);
    //Destructor
    virtual ~PowerPelletComponent() override = default;

    // Rule of Five
    PowerPelletComponent(const PowerPelletComponent&) = delete;
    PowerPelletComponent(PowerPelletComponent&&) = delete;
    PowerPelletComponent& operator=(const PowerPelletComponent&) = delete;
    PowerPelletComponent& operator=(PowerPelletComponent&&) = delete;

    virtual void Update() override {};
    virtual void Render() const override {}
    virtual void RenderImGui() override {}

    void ActivatePowerUp();

    dae::Subject* GetPowerUpSubject() { return m_pPowerUpActivatedSubject.get(); }

private:

    std::unique_ptr<dae::Subject> m_pPowerUpActivatedSubject;
    //float m_PowerUpDuration{ 0.f };
    //bool m_IsPowerUpActive{ false };
    //static constexpr float POWER_UP_DURATION = 8.0f;
};


#pragma once
#include "IGhostChaseBehaviour.h"
#include <functional>

namespace dae 
{
    class GameObject; 
}

class InkyChaseBehaviour final : public IGhostChaseBehaviour
{
public:

    explicit InkyChaseBehaviour(std::function<dae::GameObject* ()> blinkyGetter);

    virtual ~InkyChaseBehaviour() override = default;
    virtual glm::ivec2 CalculateChaseTarget(const BaseGhostComponent* pInkyGhostComponent) const override;

private:

    std::function<dae::GameObject* ()> m_fnGetBlinky;
};
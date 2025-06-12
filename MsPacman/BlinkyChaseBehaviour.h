#pragma once
#include "IGhostChaseBehaviour.h"

class BlinkyChaseBehaviour final : public IGhostChaseBehaviour
{
public:

    virtual ~BlinkyChaseBehaviour() override = default;

    virtual glm::ivec2 CalculateChaseTarget(const BaseGhostComponent* pGhostComponent) const override;
};
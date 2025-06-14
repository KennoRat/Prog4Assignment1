#pragma once
#include "IGhostChaseBehaviour.h"

class PinkyChaseBehaviour final : public IGhostChaseBehaviour
{
public:
    virtual ~PinkyChaseBehaviour() override = default;

    virtual glm::ivec2 CalculateChaseTarget(const BaseGhostComponent* pGhostComponent) const override;

};
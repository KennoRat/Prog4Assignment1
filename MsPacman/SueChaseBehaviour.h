#pragma once
#include "IGhostChaseBehaviour.h"

class SueChaseBehaviour final : public IGhostChaseBehaviour
{
public:

    virtual ~SueChaseBehaviour() override = default;
    virtual glm::ivec2 CalculateChaseTarget(const BaseGhostComponent* pClydeGhostComponent) const override;

private:

    static constexpr float AGGRESSIVE_DISTANCE_THRESHOLD_SQ = 64.0f; // (8 tiles)^2
};
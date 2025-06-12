#pragma once
#include <glm.hpp>

class BaseGhostComponent;

class IGhostChaseBehaviour
{
public:
    virtual ~IGhostChaseBehaviour() = default;

    virtual glm::ivec2 CalculateChaseTarget(const BaseGhostComponent* pGhostComponent) const = 0;
};
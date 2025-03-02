#include "RotatorComponent.h"
#include "GameObject.h"
#include "Time.h"

using namespace dae;

RotatorComponent::RotatorComponent(float speed, float radius, std::shared_ptr<GameObject> gameObject) :
	BaseComponent(*gameObject.get()),
	m_speed(speed),
	m_radius(radius)
{}

void RotatorComponent::Update()
{
    m_angle += static_cast<float>(m_speed * Time::GetInstance().GetDeltaTime());

    float x = cos(m_angle) * m_radius;
    float y = sin(m_angle) * m_radius;

    GetGameObject()->SetLocalPosition(x, y);
}

void RotatorComponent::Render() const
{
	//nothing to render
}

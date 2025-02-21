#include "RotatorComponent.h"
#include "GameObject.h"
#include "Time.h"

using namespace dae;

RotatorComponent::RotatorComponent(float speed, float radius, std::shared_ptr<GameObject> gameObject) :
    RotatorComponent(speed, radius, false, gameObject){}

RotatorComponent::RotatorComponent(float speed, float radius, bool rotateAtWorldPosition, std::shared_ptr<GameObject> gameObject) :
	BaseComponent(*gameObject.get()),
	m_speed(speed),
	m_radius(radius)
{
	if (rotateAtWorldPosition)
	{
		m_worldPosition = GetGameObject()->GetWorldPosition();
	}
}

void dae::RotatorComponent::Update()
{
	auto pos = m_worldPosition.GetPosition();
    m_angle += static_cast<float>(m_speed * Time::GetInstance().GetDeltaTime());

    float x = cos(m_angle) * m_radius;
    float y = sin(m_angle) * m_radius;

    GetGameObject()->SetLocalPosition(pos.x + x, pos.y + y);
}

void dae::RotatorComponent::Render() const
{
	//nothing to render
}

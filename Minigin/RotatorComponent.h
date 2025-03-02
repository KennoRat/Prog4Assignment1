#pragma once
#include <memory>

#include "BaseComponent.h"
#include "Transform.h"

namespace dae
{
	class GameObject;

	class RotatorComponent final : public BaseComponent
	{
	public:
		//Constructor
		RotatorComponent(float speed, float radius, std::shared_ptr<GameObject> gameObject);

		//Destructor
		virtual ~RotatorComponent() override = default;

		//Rule of 5
		RotatorComponent(const RotatorComponent& other) = delete;
		RotatorComponent(RotatorComponent&& other) = delete;
		RotatorComponent& operator=(const RotatorComponent& other) = delete;
		RotatorComponent& operator=(RotatorComponent&& other) = delete;

		virtual void Update() override;
		virtual void Render() const override;

	private:
		float m_angle{0.f};
		float m_speed;
		float m_radius;
	};
}

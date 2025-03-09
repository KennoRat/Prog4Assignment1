#pragma once

#include <memory>

#include "BaseComponent.h"
#include "Transform.h"

namespace dae
{
	class GameObject;

	enum class Direction;

	class MovementComponent final : public BaseComponent
	{
	public:
		//Constructor
		MovementComponent(std::shared_ptr<GameObject> gameObject, float speed);

		//Destructor
		virtual ~MovementComponent() override = default;

		//Rule of 5
		MovementComponent(const MovementComponent& other) = delete;
		MovementComponent(MovementComponent&& other) = delete;
		MovementComponent& operator=(const MovementComponent& other) = delete;
		MovementComponent& operator=(MovementComponent&& other) = delete;

		virtual void Update() override;
		virtual void Render() const override;
		virtual void RenderImGui() override;

		void Move(Direction moveDirection);

	private:
		float m_speed;
	};
}

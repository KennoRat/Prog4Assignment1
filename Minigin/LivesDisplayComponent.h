#pragma once
#include <memory>

#include "BaseComponent.h"
#include "Observer.h"

namespace dae
{
	class GameObject;

	class LivesDisplayComponent final : public BaseComponent, public Observer
	{
	public:
		//Constructor
		LivesDisplayComponent(std::shared_ptr<GameObject> gameObject);

		virtual void Update() override;
		virtual void Render() const override;
		virtual void RenderImGui() override;

		virtual void Notify(const GameObject& game_object, const Event& event) override;
	};
}


#pragma once
#include <memory>
#include <vector>

#include "BaseComponent.h"
#include "Transform.h"
#include "TextComponent.h"
#include "RenderComponent.h"

namespace dae
{

	class GameObject final
	{
	public:

		//Constructors
		GameObject() = default;

		//Destructor
		~GameObject();

		// Rule Of Five
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void Update();
		void Render() const;

		void SetPosition(float x, float y);
		void AddComponent(std::shared_ptr<BaseComponent> component);
		void RemoveComponent(std::shared_ptr<BaseComponent> component);
		template <typename GC>
		std::shared_ptr<GC> GetComponent() const;
		template <typename CC>
		bool CheckComponent() const;
		Transform GetTransform() const;

	private:
		Transform m_transform{};

		std::vector<std::shared_ptr<BaseComponent>> m_components;
	};
}
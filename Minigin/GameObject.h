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
		~GameObject() = default;

		//Rule Of Five
		GameObject(const GameObject& other) = default;
		GameObject(GameObject&& other) = default;
		GameObject& operator=(const GameObject& other) = default;
		GameObject& operator=(GameObject&& other) = default;

		void Update();
		void Render() const;

		void SetPosition(float x, float y);
		void AddComponent(std::unique_ptr<BaseComponent> component);
		template <typename GC>
		GC* GetComponent() const;
		template <typename RC>
		void RemoveComponent();
		template <typename CC>
		bool CheckComponent() const;
		Transform GetTransform() const;

	private:
		//Methods
		void DeleteComponent(BaseComponent* component);

		//Data

		Transform m_transform{};

		std::vector<std::unique_ptr<BaseComponent>> m_components;
		std::vector<BaseComponent*> m_componentsToBeDeleted;
	};

	template <typename GC>
	GC* GameObject::GetComponent() const
	{
		for (const std::unique_ptr<BaseComponent>& component : m_components)
		{
			// Use dynamic_cast for unique_ptr, returns raw pointer (GC*)
			if (auto* castedComponent = dynamic_cast<GC*>(component.get()))
			{
				return castedComponent;
			}
		}
		return nullptr;
	}


	//Template functions
	template<typename RC>
	inline void GameObject::RemoveComponent()
	{
		auto temp = GetComponent<RC>();

		if (temp != nullptr)
		{
			m_componentsToBeDeleted.push_back(std::move(temp));
		}
	}


	template <typename CC>
	bool GameObject::CheckComponent() const
	{
		return GetComponent<CC>() != nullptr;
	}
}
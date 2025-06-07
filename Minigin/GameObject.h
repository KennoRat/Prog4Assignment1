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
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void Update();
		void LateUpdate();
		void Render() const;
		void RenderImGui();

		void AddComponent(std::unique_ptr<BaseComponent> component);
		void SetLocalPosition(const Transform& Position);
		void SetLocalPosition(float x, float y, float z = 0.f);
		void SetParent(GameObject* parent, bool keepWorldPosition);

		int GetChildCount() const;
		Transform GetWorldPosition();
		GameObject* GetParent() const;
		GameObject* GetChildAt(int index) const;
		std::vector<GameObject*> GetChildren() const;

		template <typename GC>
		GC* GetComponent() const;
		template <typename RC>
		void RemoveComponent();
		template <typename CC>
		bool CheckComponent() const;

	private:
		//Methods
		void DeleteComponent(BaseComponent* component);
		void UpdateWorldPosition();
		void SetPositionDirty();
		void RemoveChild(GameObject* child);
		void AddChild(GameObject* child);
		bool IsChild(GameObject* child) const;

		//Data
		bool m_positionIsDirty{ true };
		Transform m_worldPosition{};
		Transform m_localPosition{};
		GameObject* m_parent{ nullptr };
		std::vector<GameObject*> m_children;

		std::vector<std::unique_ptr<BaseComponent>> m_components;
		std::vector<BaseComponent*> m_componentsToBeDeleted;
	};

	//Template functions
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
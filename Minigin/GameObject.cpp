#include <string>
#include "GameObject.h"

using namespace dae;

GameObject::~GameObject()
{
	for (auto& component : m_components)
	{
		component.reset();  // Releases the shared_ptr
	}
	m_components.clear();
}

void GameObject::Update()
{
	for (auto& component : m_components)
	{
		component->Update();
	}
}

void GameObject::Render() const
{
	for (auto& component : m_components)
	{
		component->Render();
	}
}

void GameObject::SetPosition(float x, float y)
{
	m_transform.SetPosition(x, y, 0.0f);
}

void GameObject::AddComponent(std::shared_ptr<BaseComponent> component)
{
	m_components.push_back(std::move(component));
}

void GameObject::RemoveComponent(std::shared_ptr<BaseComponent> component)
{
	auto it = std::remove(m_components.begin(), m_components.end(), component);
	for (auto iter = it; iter != m_components.end(); ++iter)
	{
		(*iter).reset();  // release shared_ptr
	}
	m_components.erase(it, m_components.end());

}

template <typename GC>
std::shared_ptr<GC> GameObject::GetComponent() const
{
	for (const std::shared_ptr<BaseComponent>& component : m_components)
	{
		std::shared_ptr<GC> castedComponent = std::dynamic_pointer_cast<GC>(component);
		if (castedComponent)
		{
			return castedComponent;
		}
	}
	return nullptr;
}

template <typename CC>
bool GameObject::CheckComponent() const
{
	return GetComponent<CC>() != nullptr;
}

Transform GameObject::GetTransform() const
{
	return m_transform;
}

#include <string>
#include "GameObject.h"

using namespace dae;

void GameObject::Update()
{
	for (auto& component : m_components)
	{
		component->Update();
	}

	if (m_componentsToBeDeleted.size() > 0)
	{
		for (auto& component : m_componentsToBeDeleted)
		{
			DeleteComponent(component);
		}
		m_componentsToBeDeleted.clear();
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

void GameObject::AddComponent(std::unique_ptr<BaseComponent> component)
{
	assert(component != nullptr);
	assert(component->GetGameObject() == this);
	m_components.push_back(std::move(component));
}

void GameObject::DeleteComponent(BaseComponent* component)
{
	auto it = std::remove_if(m_components.begin(), m_components.end(),
		[component](const std::unique_ptr<BaseComponent>& comp)
		{
			return comp.get() == component;
		});

	m_components.erase(it, m_components.end());
}

Transform GameObject::GetTransform() const
{
	return m_transform;
}

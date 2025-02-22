#include <string>
#include "GameObject.h"

using namespace dae;

void GameObject::Update()
{
	for (auto& component : m_components)
	{
		component->Update();
	}
}

void GameObject::LateUpdate()
{
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

void GameObject::SetLocalPosition(const Transform& Position)
{
	auto pos = Position.GetPosition();
	m_localPosition.SetPosition(pos.x, pos.y, pos.z);
	SetPositionDirty();
}

void GameObject::SetLocalPosition(float x, float y, float z)
{
	m_localPosition.SetPosition(x, y, z);
	SetPositionDirty();
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

	if (it != m_components.end())
	{
		m_components.erase(it, m_components.end());
	}
}

void GameObject::SetPositionDirty()
{
	m_positionIsDirty = true;
	for (auto& child : m_children)
	{
		child->SetPositionDirty();
	}
}

void dae::GameObject::RemoveChild(GameObject* child)
{
	auto it = std::remove(m_children.begin(), m_children.end(), child);
	if (it != m_children.end())
	{
		m_children.erase(it);
	}
}

void dae::GameObject::AddChild(GameObject* child)
{
	if (child && std::find(m_children.begin(), m_children.end(), child) == m_children.end())
	{
		m_children.push_back(child);
	}
}

bool dae::GameObject::IsChild(GameObject* child) const
{
	if (child && std::find(m_children.begin(), m_children.end(), child) != m_children.end())
	{
		return true;
	}
	return false;
}

Transform GameObject::GetWorldPosition()
{
	if (m_positionIsDirty)
	{
		UpdateWorldPosition();
	}
	return m_worldPosition;
}

void GameObject::SetParent(GameObject* parent, bool keepWorldPosition)
{
	if (IsChild(parent) || parent == this || m_parent == parent)
		return;

	if (parent == nullptr)
	{
		SetLocalPosition(GetWorldPosition());
	}
	else
	{
		if (keepWorldPosition)
			SetLocalPosition(GetWorldPosition() - parent->GetWorldPosition());
		SetPositionDirty();
	}

	if (m_parent) m_parent->RemoveChild(this);
	m_parent = parent;
	if (m_parent) m_parent->AddChild(this);
}

GameObject* GameObject::GetParent() const
{
	return m_parent;
}

int GameObject::GetChildCount() const
{
	return static_cast<int>(m_children.size());
}

GameObject* GameObject::GetChildAt(int index) const
{
	if (index < static_cast<int>(m_children.size()))
	{
		return m_children[index];
	}
	return nullptr;
}

std::vector<GameObject*> GameObject::GetChildren() const
{
	return m_children;
}

void GameObject::UpdateWorldPosition()
{
	if (m_positionIsDirty)
	{
		if (m_parent != nullptr)
		{
			m_worldPosition = m_parent->GetWorldPosition() + m_localPosition;
		}
		else
		{
			m_worldPosition = m_localPosition;
		}
		m_positionIsDirty = false;
	}
}



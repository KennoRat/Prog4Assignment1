#include "Scene.h"
#include "GameObject.h"

#include <algorithm>

using namespace dae;

unsigned int Scene::m_idCounter = 0;

Scene::Scene(const std::string& name) : m_name(name) {}

Scene::~Scene()
{
	for (auto& gameObject : m_objects)
	{
		gameObject.reset();  // Releases the shared_ptr
	}
	m_objects.clear();
};

void Scene::Add(std::shared_ptr<GameObject> object)
{
	m_objects.emplace_back(std::move(object));
}

void Scene::Remove(std::shared_ptr<GameObject> object)
{
	assert(object != nullptr);
	m_objectsToBeDeleted.push_back(object.get());
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void Scene::Update()
{
	for (const auto& object : m_objects)
	{
		object->Update();
	}

	if (m_objectsToBeDeleted.size() > 0)
	{
		for (const auto& object : m_objectsToBeDeleted)
		{
			DeleteObject(object);
		}
		m_objectsToBeDeleted.clear();
	}
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->Render();
	}
}

void Scene::DeleteObject(GameObject* object)
{
	if (object == nullptr) return;

	auto children = object->GetChildren();
	for(auto* child : children)
	{
		DeleteObject(child);
	}

	auto it = std::remove_if(m_objects.begin(), m_objects.end(),
		[object](const std::shared_ptr<GameObject>& obj)
		{
			return obj.get() == object;
		});

	if (it != m_objects.end())
	{
		m_objects.erase(it, m_objects.end());
	}
}




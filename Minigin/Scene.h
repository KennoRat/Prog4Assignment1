#pragma once
#include "SceneManager.h"

namespace dae
{
	class GameObject;
	class Scene final
	{
		friend Scene& SceneManager::CreateScene(const std::string& name);
	public:
		void Add(std::shared_ptr<GameObject> object);
		void Remove(std::shared_ptr<GameObject> object);
		void RemoveAll();

		void Update();
		void LateUpdate();
		void Render() const;
		void RenderImGui();

		~Scene();
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private: 
		explicit Scene(const std::string& name);

		void DeleteObject(GameObject* object);

		std::string m_name;
		std::vector < std::shared_ptr<GameObject>> m_objects{};
		std::vector <GameObject*> m_objectsToBeDeleted{};

		static unsigned int m_idCounter; 
	};

}

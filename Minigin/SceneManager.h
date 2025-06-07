#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Singleton.h"

namespace dae
{
	class Scene;

	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		//Destructor
		~SceneManager();

		//Rule of 5
		SceneManager(const SceneManager& other) = delete;
		SceneManager(SceneManager&& other) = delete;
		SceneManager& operator=(const SceneManager& other) = delete;
		SceneManager& operator=(SceneManager&& other) = delete;

		std::shared_ptr<Scene> CreateScene(const std::string& name);
		void RemoveScene(const std::string& name);
		bool SetActiveScene(const std::string& name);
		void PauseActiveScene(bool Pause);
		Scene* GetActiveScene() const;
		std::shared_ptr<Scene> GetSceneByName(const std::string& name);

		void Update();
		void LateUpdate();
		void Render();
		void RenderImGui();

	private:

		friend class Singleton<SceneManager>;
		SceneManager() = default;

		std::vector<std::shared_ptr<Scene>> m_scenes;
		Scene* m_pActiveScene{ nullptr };
		bool m_PauseActiveScene{ false };
	};
}

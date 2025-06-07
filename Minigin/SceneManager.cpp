#include "SceneManager.h"
#include "Scene.h"

using namespace dae;

SceneManager::~SceneManager()
{
	m_scenes.clear();
	m_pActiveScene = nullptr;
}

void SceneManager::Update()
{
	if (m_pActiveScene && !m_PauseActiveScene)
	{
		m_pActiveScene->Update();
	}
}

void SceneManager::LateUpdate()
{
	if (m_pActiveScene && !m_PauseActiveScene)
	{
		m_pActiveScene->LateUpdate();
	}
}

void SceneManager::Render()
{
	if (m_pActiveScene)
	{
		m_pActiveScene->Render();
	}
}

void SceneManager::RenderImGui()
{
	if (m_pActiveScene)
	{
		m_pActiveScene->RenderImGui();
	}
}

std::shared_ptr<Scene> SceneManager::CreateScene(const std::string& name)
{
	// Check if scene already exists
	auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
		[&name](const std::shared_ptr<Scene>& scene) 
		{
			return scene->GetName() == name;
		});

	if (it != m_scenes.end()) 
	{
		return *it; // Return existing scene
	}

	// Friend Private constructor
	std::shared_ptr<Scene> newScene = std::shared_ptr<Scene>(new Scene(name));
	m_scenes.push_back(newScene);

	// Set new active scene
	if (!m_pActiveScene) 
	{
		m_pActiveScene = newScene.get();
	}

	return newScene;
}

void SceneManager::RemoveScene(const std::string& name)
{
	auto it = std::remove_if(m_scenes.begin(), m_scenes.end(),
		[&name](const std::shared_ptr<Scene>& scene)
		{
			return scene->GetName() == name;
		});

	if (it != m_scenes.end())
	{
		// Reset active scene
		if (m_pActiveScene && m_pActiveScene->GetName() == name) 
		{
			m_pActiveScene = nullptr;
		}

		m_scenes.erase(it, m_scenes.end());
	}
}

bool SceneManager::SetActiveScene(const std::string& name)
{
	auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
		[&name](const std::shared_ptr<Scene>& scene) 
		{
			return scene->GetName() == name;
		});

	// Set new active scene
	if (it != m_scenes.end()) 
	{
		m_pActiveScene = (*it).get(); 
		return true;
	}

	m_pActiveScene = nullptr;
	return false;
}

void dae::SceneManager::PauseActiveScene(bool Pause)
{
	m_PauseActiveScene = Pause;
}

Scene* SceneManager::GetActiveScene() const
{
	return m_pActiveScene;
}

std::shared_ptr<Scene> dae::SceneManager::GetSceneByName(const std::string& name)
{
	auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
		[&name](const std::shared_ptr<Scene>& scene) 
		{
			return scene->GetName() == name;
		});

	if (it != m_scenes.end()) 
	{
		return *it;
	}
	return nullptr;
}

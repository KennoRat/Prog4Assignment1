#pragma once
#include <memory>

#include "BaseComponent.h"


namespace dae
{
	class GameObject;
	class Subject;

	class LivesComponent final : public BaseComponent
	{
	public:
		//Constructor
		LivesComponent(std::shared_ptr<GameObject> gameObject, std::unique_ptr<Subject> deathSubject, std::unique_ptr<Subject> respawnSubject,int startingLives);

		virtual void Update() override;
		virtual void Render() const override;
		virtual void RenderImGui() override;

		int GetLives() const { return m_lives; }
		void Die();
		void Respawn();

		Subject* GetDeathSubject() { return m_pObjectDiedEvent.get(); }
		Subject* GetRespawnSubject() { return m_pObjectRespawnedEvent.get(); };

	private:

		std::unique_ptr<Subject> m_pObjectDiedEvent = nullptr;
		std::unique_ptr<Subject> m_pObjectRespawnedEvent = nullptr;
		int m_lives;
	};
}
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
		LivesComponent(std::shared_ptr<GameObject> gameObject, std::unique_ptr<Subject> subject,int startingLives);

		virtual void Update() override;
		virtual void Render() const override;
		virtual void RenderImGui() override;

		int GetLives() const { return m_lives; }
		void Die();

	private:
		std::unique_ptr<Subject> m_pObjectDiedEvent = nullptr;
		int m_lives;
	};
}
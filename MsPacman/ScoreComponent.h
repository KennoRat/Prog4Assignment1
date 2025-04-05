#pragma once
#include <memory>

#include "BaseComponent.h"


namespace dae
{
	class GameObject;
	class Subject;

	class ScoreComponent final : public BaseComponent
	{
	public:
		//Constructor
		ScoreComponent(std::shared_ptr<GameObject> gameObject, std::unique_ptr<Subject> subject);

		virtual void Update() override;
		virtual void Render() const override;
		virtual void RenderImGui() override;

		int GetPoints() const { return m_points; }
		void GainPoints(int amount);

	private:
		std::unique_ptr<Subject> m_pScoreEvent = nullptr;
		int m_points = 0;
	};
}



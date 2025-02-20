#pragma once

#include "TextComponent.h"

namespace dae
{
	class GameObject;

	class FpsComponent final : public BaseComponent
	{
	public:
		//Constructor
		FpsComponent(std::shared_ptr<Font> font, std::shared_ptr<GameObject> gameObject);

		//Destructor
		~FpsComponent() = default;

		//Rule of 5
		FpsComponent(const FpsComponent& other) = default;
		FpsComponent(FpsComponent&& other) = default;
		FpsComponent& operator=(const FpsComponent& other) = default;
		FpsComponent& operator=(FpsComponent&& other) = default;

		virtual void Update() override;
		virtual void Render() const override;

	private:
		//TextComponent* m_textComponent;
		std::unique_ptr<TextComponent> m_textComponent{ nullptr };
	};
}



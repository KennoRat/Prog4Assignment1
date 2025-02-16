#pragma once
#include "TextComponent.h"

namespace dae
{
	class FpsComponent final : public TextComponent
	{
	public:
		//Constructor
		FpsComponent(const std::string& text, std::shared_ptr<Font> font, std::shared_ptr<GameObject> gameObjrct);

		//Destructor
		virtual ~FpsComponent() override = default;

		//Rule of 5
		FpsComponent(const FpsComponent& other) = default;
		FpsComponent(FpsComponent&& other) = default;
		FpsComponent& operator=(const FpsComponent& other) = default;
		FpsComponent& operator=(FpsComponent&& other) = default;

		virtual void Update() override;

	private:
	};
}



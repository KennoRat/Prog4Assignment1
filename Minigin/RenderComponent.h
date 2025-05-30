#pragma once
#include <memory>
#include <string>
#include "BaseComponent.h"

namespace dae
{
	class GameObject;
	class Texture2D;

	class RenderComponent final : public BaseComponent
	{
	public:
		//Constructor
		RenderComponent(std::shared_ptr<GameObject> gameObject);
		RenderComponent(std::shared_ptr<GameObject> gameObject, float width, float height);

		//Destructor
		virtual ~RenderComponent() override;

		// Rule Of Five
		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;

		virtual void Update() override; // Not used
		virtual void Render() const override;
		virtual void RenderImGui() override;

		void SetTexture(const std::string& filename);

	private:

		float m_width{};
		float m_height{};
		std::shared_ptr<Texture2D> m_texture{ nullptr };
	};
}


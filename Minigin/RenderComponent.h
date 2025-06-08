#pragma once
#include <memory>
#include <string>
#include <SDL_rect.h> 
#include "BaseComponent.h"

namespace dae
{
	class GameObject;
	class Texture2D;

	class RenderComponent final : public BaseComponent
	{
	public:
		//Constructor
		RenderComponent(std::shared_ptr<GameObject> gameObject, float renderWidth = 0.f, float renderHeight = 0.f);

		//Destructor
		virtual ~RenderComponent() override;

		// Rule Of Five
		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;

		virtual void Update() override;
		virtual void Render() const override;
		virtual void RenderImGui() override;

		void SetTexture(const std::string& filename);
		void SetTexture(std::shared_ptr<Texture2D> texture);

		// Sprite Sheets methods
		void SetSourceRect(const SDL_Rect& srcRect);
		void SetSourceRect(int x, int y, int w, int h);
		const SDL_Rect& GetSourceRect() const { return m_sourceRect; }
		bool HasSourceRect() const { return m_useSourceRect; }

		float GetRenderWidth() const { return m_renderWidth; }
		float GetRenderHeight() const { return m_renderHeight; }

		void SetRenderDimensions(float width, float height);

	private:

		std::shared_ptr<Texture2D> m_pTexture{ nullptr };

		SDL_Rect m_sourceRect{ 0,0,0,0 };
		bool m_useSourceRect{ false };

		float m_renderWidth{};
		float m_renderHeight{};
	};
}


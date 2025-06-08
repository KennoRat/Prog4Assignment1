#pragma once
#include <SDL.h>
#include "Singleton.h"

namespace dae
{
	class Texture2D;
	/**
	 * Simple RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
	public:
		void Init(SDL_Window* window);
		void Destroy();

		// Render frame phases
		void BeginFrame() const;
		void EndFrame() const;

		// Full texture
		void RenderTexture(const Texture2D& texture, float x, float y) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height) const;

		// Sprite Sheets
		void RenderTextureSpriteSheet(const Texture2D& texture, float x, float y, const SDL_Rect& srcRect) const;
		void RenderTextureSpriteSheet(const Texture2D& texture, float x, float y, float destWidth, float destHeight, const SDL_Rect& srcRect) const;

		// Render Rect
		void RenderFilledRect(float x, float y, float width, float height, SDL_Color color) const;

		SDL_Renderer* GetSDLRenderer() const;
		void GetWindowDimensions(int& width, int& height) const;

		const SDL_Color& GetBackgroundColor() const { return m_clearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_clearColor = color; }

	private:
		SDL_Renderer* m_renderer{};
		SDL_Window* m_window{};
		SDL_Color m_clearColor{};
	};
}


#pragma once
#include <string>
#include <memory>

#include "BaseComponent.h"
#include "Font.h"
#include "Texture2D.h"

namespace dae
{
	class GameObject;

	class TextComponent : public BaseComponent
	{
	public:
		
		//Constructor
		TextComponent(std::shared_ptr<GameObject> gameObject, const std::string& text, std::shared_ptr<Font> font);

		//Destructor
		virtual ~TextComponent() override;

		//Rule of 5
		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;

		virtual void Update() override;
		virtual void Render() const override;

		virtual void SetText(const std::string& text);
	private:

		bool m_needsUpdate;
		std::string m_text;
		std::shared_ptr<Font> m_font;
		std::shared_ptr<Texture2D> m_textTexture;
	};
}

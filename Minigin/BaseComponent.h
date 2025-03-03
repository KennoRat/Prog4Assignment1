#pragma once
#include <memory>

namespace dae
{
	class GameObject;

	class BaseComponent // Base class for all components
	{
	public:
		//Destructor
		virtual ~BaseComponent() = default;
		//Rule Of Five
		BaseComponent(const BaseComponent& other) = delete;
		BaseComponent(BaseComponent&& other) = delete;
		BaseComponent& operator=(const BaseComponent& other) = delete;
		BaseComponent& operator=(BaseComponent&& other) = delete;

		virtual void Update() = 0;
		virtual void Render() const = 0;
		virtual void RenderImGui() = 0;

		GameObject* GetGameObject() const { return m_gameObjectPtr; }
	protected:
		//Constructor
		explicit BaseComponent(GameObject& gameObject)
			: m_gameObjectPtr(&gameObject) {};
	private:
		GameObject* m_gameObjectPtr;
	};
}

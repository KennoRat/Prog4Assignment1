#pragma once
#include <memory>

namespace dae
{
	class GameObject;

	class BaseComponent // Base class for all components
	{
	public:
		//Constructor
		explicit BaseComponent(std::shared_ptr<GameObject> gameObject)
			: m_gameObject(gameObject) {
		};
		//Destructor
		virtual ~BaseComponent() = default;

		virtual void Update() = 0;
		virtual void Render() const = 0;

	protected:
		std::weak_ptr<GameObject> m_gameObject;  // Prevents circular reference
	};

}

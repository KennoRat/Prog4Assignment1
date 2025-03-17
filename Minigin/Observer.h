#pragma once

#pragma once
namespace dae
{
	struct Event;
	class GameObject;

	class Observer
	{
	public:

		virtual ~Observer() = default;

		virtual void Notify(const GameObject& game_object, const Event& event) = 0;

	};
}



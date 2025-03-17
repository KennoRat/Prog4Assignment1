#pragma once
#include <vector>


namespace dae
{
	class Observer;
	class GameObject;
	struct Event;

	class Subject
	{
	public:
		Subject() = default;

		void AddObserver(Observer* pObserver);
		void RemoveObserver(Observer* pObserver);

		void NotifyObservers(const GameObject& game_object, const Event& event);

	private:
		std::vector<Observer*> m_Observers;
	};
}

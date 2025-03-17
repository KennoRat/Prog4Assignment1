#include "Subject.h"
#include "Observer.h"

using namespace dae;

void Subject::AddObserver(Observer* pObserver)
{
	m_Observers.emplace_back(pObserver);
}

void Subject::RemoveObserver(Observer* pObserver)
{
	auto iterator = std::find(m_Observers.begin(), m_Observers.end(), pObserver);

	if (iterator == m_Observers.end())
	{
		return; // Observer not found
	}

	m_Observers.erase(iterator);
}

void Subject::NotifyObservers(const GameObject& game_object, const Event& event)
{
	for (Observer* pObserver : m_Observers)
	{
		pObserver->Notify(game_object, event);
	}
}
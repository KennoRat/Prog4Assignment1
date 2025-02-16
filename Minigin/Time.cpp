#include "Time.h"

using namespace dae;

Time::Time():
m_deltaTime(0.0), m_lastTime(std::chrono::high_resolution_clock::now()) 
{
}

Time& Time::GetInstance()
{
	static Time instance; // Singleton instance
	return instance;
}

void Time::Update()
{
	const auto currentTime = std::chrono::high_resolution_clock::now();
	m_deltaTime = std::chrono::duration<double>(currentTime - m_lastTime).count();
	m_lastTime = currentTime;
}

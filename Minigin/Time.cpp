#include "Time.h"

using namespace dae;

void Time::Update()
{
	const auto currentTime = std::chrono::high_resolution_clock::now();
	m_deltaTime = std::chrono::duration<double>(currentTime - m_lastTime).count();
	m_lastTime = currentTime;
}

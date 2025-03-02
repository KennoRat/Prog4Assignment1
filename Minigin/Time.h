#pragma once
#include "Singleton.h"
#include <chrono>

namespace dae
{
	class Time final: public Singleton<Time>
	{
	public:
		void Update();

		double GetDeltaTime() const { return m_deltaTime; }
		std::chrono::high_resolution_clock::time_point GetLastTime() const { return m_lastTime; }

	private:
		friend class Singleton<Time>;
		//Constructor
		Time() = default;

		double m_deltaTime{};
		std::chrono::high_resolution_clock::time_point m_lastTime{ std::chrono::high_resolution_clock::now() };
	};
}

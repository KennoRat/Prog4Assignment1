#pragma once
#include <chrono>

namespace dae
{
	class Time final
	{
	public:
		static Time& GetInstance();

		void Update();

		double GetDeltaTime() const { return m_deltaTime; }
		std::chrono::high_resolution_clock::time_point GetLastTime() const { return m_lastTime; }

	private:
		//Constructor
		Time();

		~Time() = default;

		// Rule Of Five
		Time(const Time&) = delete;
		Time(Time&&) = delete;
		Time& operator=(const Time&) = delete;
		Time& operator=(Time&&) = delete;

		double m_deltaTime;
		std::chrono::high_resolution_clock::time_point m_lastTime;
	};
}

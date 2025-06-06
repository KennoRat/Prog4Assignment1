#pragma once
#include <string>
#include <functional>
#include <memory>

namespace dae
{
	// Forward declaration
	class GameStateMachine;
	class GameState;

	class Minigin
	{
	public:
		// Constructor and Destructor
		explicit Minigin(const std::string& dataPath);
		~Minigin();

		// Rule Of 5
		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;

		void Run(const std::function<void()>& load);
		void SetInitialGameState(std::unique_ptr<GameState> state);
		GameStateMachine* GetGameStateMachine() const { return m_pGameStateMachine.get(); }
	private:

		std::unique_ptr<GameStateMachine> m_pGameStateMachine;
	};
}
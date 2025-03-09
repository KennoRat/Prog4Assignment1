#pragma once

namespace dae
{
	class GameObject;

	class Command
	{
	public:
		virtual ~Command() {};
		virtual void Execute() = 0;
	};

	class GameObjectCommand : public Command
	{
	public:
		GameObjectCommand(GameObject* gameObject);
		virtual ~GameObjectCommand();

	protected:
		GameObject* GetGameObject() const { return m_gameObject; }

	private:
		GameObject* m_gameObject;
	};
}

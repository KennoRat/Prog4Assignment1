#include "Command.h"
#include "GameObject.h"
#include "MovementComponent.h"

using namespace dae;

GameObjectCommand::GameObjectCommand(GameObject* gameObject):m_gameObject{gameObject}
{
}

GameObjectCommand::~GameObjectCommand()
{
	m_gameObject = nullptr;
}

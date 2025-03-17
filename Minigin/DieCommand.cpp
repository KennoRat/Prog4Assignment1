#include "DieCommand.h"
#include "GameObject.h"
#include "LivesComponent.h"

using namespace dae;

DieCommand::DieCommand(std::shared_ptr<GameObject> gameObject):
	GameObjectCommand(gameObject.get())
{
}

void DieCommand::Execute()
{
	GetGameObject()->GetComponent<LivesComponent>()->Die();
}

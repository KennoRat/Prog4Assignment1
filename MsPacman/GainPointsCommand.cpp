#include "GainPointsCommand.h"
#include "GameObject.h"
#include "ScoreComponent.h"

using namespace dae;

GainPointsCommand::GainPointsCommand(std::shared_ptr<GameObject> gameObject) :
	GameObjectCommand(gameObject.get())
{
}

void GainPointsCommand::Execute()
{
	GetGameObject()->GetComponent<ScoreComponent>()->GainPoints(100);
}

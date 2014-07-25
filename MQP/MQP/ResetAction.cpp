#include "ResetAction.h"
#include "Game.h"

using namespace C4;

ResetAction::ResetAction(unsigned long type) : Action(type)
{
}

ResetAction::~ResetAction()
{
}

void ResetAction::Begin(void)
{
	GameWorld* world = (GameWorld*)(TheWorldMgr->GetWorld());
	GameCamera* camera = (GameCamera*)world->GetCamera();
	camera->Reset();
}

void ResetAction::End(void)
{
}
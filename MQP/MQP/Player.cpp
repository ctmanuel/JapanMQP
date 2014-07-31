#include "Player.h"
#include "Game.h"

using namespace C4;

PlayerController::PlayerController() : Controller(kControllerPlayer)
{
	PlayerNode = GetTargetNode();
	SetPlayerModel(Model::Get(kModelPlayer));
}

PlayerController::PlayerController(const PlayerController& playerController) : Controller(playerController)
{

}

Controller *PlayerController::Replicate(void) const
{
	return (new PlayerController(*this));
}

PlayerController::~PlayerController()
{

}

bool PlayerController::ValidNode(const Node *node)
{
	return ((node) && 
		((node->GetNodeType() == kNodeModel) ||
		 (node->GetNodeType() == kNodeGeometry)));r
}

void PlayerController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);
}

void PlayerController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
}

void PlayerController::Preprocess(void)
{
	Controller::Preprocess();
}

void PlayerController::Move(void)
{
	Point3D currentpos = PlayerNode->GetNodePosition();
	PlayerNode->SetNodePosition(Point3D(currentpos.x += 5.0F, currentpos.y, currentpos.z));
}

Point3D PlayerController::PlayerPosition()
{
	
	 return PlayerNode->GetNodePosition();
}


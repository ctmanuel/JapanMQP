#include "Tank.h"
#include "Game.h"

using namespace C4;

TankController::TankController() : Controller(kControllerTank)
{
}

TankController::TankController(const TankController& tankController) :
	Controller(tankController)
{
}

Controller* TankController::Replicate(void) const
{
	return (new TankController(*this));
}

TankController::~TankController()
{
}

bool TankController::ValidNode(const Node* node)
{
	return ((node) && (node->GetNodeType() == kNodeModel));
}

void TankController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);
}

void TankController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
}

void TankController::Preprocess(void)
{
	Controller::Preprocess();
}

void TankController::Move(void)
{
	// TODO
}
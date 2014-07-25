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
	if (leap.isConnected())
	{
		Leap::HandList hands = leap.frame().hands();
		if (!hands.isEmpty())
		{
			Leap::Hand hand = hands.frontmost();
			Point3D leapMotion = 
				Point3D(hand.palmPosition()[2] * 0.01f, hand.palmPosition()[0] * 0.01, hand.palmPosition()[1] * 0.01);
			GetTargetNode()->SetNodePosition(Point3D(1.0f, 0.0f, 2.0f) + leapMotion);
			GetTargetNode()->Invalidate();
		}
		else
		{
			GetTargetNode()->SetNodePosition(Point3D(1.0f, 0.0f, 2.0f));
			GetTargetNode()->Invalidate();
		}
	}
	else
	{
		GetTargetNode()->SetNodePosition(Point3D(1.0f, 0.0f, 2.0f));
		GetTargetNode()->Invalidate();
	}
}
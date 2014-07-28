#include "HandController.h"
#include "Game.h"

using namespace C4;

HandController::HandController() : Controller(kControllerHand)
{
}

HandController::HandController(const HandController& handController) : 
	Controller(handController)
{
}

Controller* HandController::Replicate(void) const
{
	return (new HandController(*this));
}

HandController::~HandController()
{
}

void HandController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);
}

void HandController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
}

void HandController::Preprocess(void)
{
	Controller::Preprocess();
	startOrientation = GetTargetNode()->GetNodeTransform().GetMatrix3D();
}

void HandController::Move(void)
{
	// TODO: Set up basePosition based on player position
	Point3D basePosition = Point3D(3.0f, 0.0f, 0.5f);

	Point3D leapMotion = Point3D(0.0f, 0.0f, 0.0f);
	if (leap.isConnected())
	{
		Leap::Hand hand = leap.frame().hands().frontmost();
		leapMotion.x = hand.palmPosition()[2] * -0.005f;
		leapMotion.y = hand.palmPosition()[0] * -0.005f;
		leapMotion.z = (hand.palmPosition()[1] * 0.005f) - 0.5f;

		Quaternion x, y, z;
		x.SetRotationAboutX(-1 * hand.palmNormal().roll());
		y.SetRotationAboutY(-1 * hand.direction().pitch());
		z.SetRotationAboutZ((-1 * hand.direction().yaw()));// +K::pi_over_2);
		GetTargetNode()->SetNodeMatrix3D((x * y * z).GetRotationMatrix() * startOrientation);
	}

	GetTargetNode()->SetNodePosition(basePosition + leapMotion);
	GetTargetNode()->Invalidate();
}

#include "HandController.h"
#include "Game.h"

using namespace C4;

HandController::HandController() : Controller(kControllerHand)
{
	lightPath = nullptr;
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
	/*
	Model *playerModel = Model::Get(kModelPlayer);
	const Point3D& PlayerPosition = playerModel->GetNodePosition();
	Point3D basePosition = Point3D(PlayerPosition.x + 1.1F, PlayerPosition.y + 1.1F, PlayerPosition.z);
	*/
	Point3D basePosition = Point3D(0.0f, 0.0f, 0.0f);

	Point3D leapMotion = Point3D(0.0f, 0.0f, 0.0f);

	if (leap.isConnected())
	{
		Leap::HandList hands = leap.frame().hands();
		if (!hands.isEmpty())
		{
			Leap::Hand hand = hands.frontmost();
			leapMotion.x = hand.stabilizedPalmPosition()[2] * -0.005f;
			leapMotion.y = hand.stabilizedPalmPosition()[0] * -0.005f;
			leapMotion.z = (hand.stabilizedPalmPosition()[1] - Z_MID) * 0.005f;

			Quaternion x, y, z;
			x.SetRotationAboutX(-1 * hand.palmNormal().roll());
			y.SetRotationAboutY(-1 * hand.direction().pitch());
			z.SetRotationAboutZ((-1 * hand.direction().yaw()));
			GetTargetNode()->SetNodeMatrix3D((x * y * z).GetRotationMatrix() * startOrientation);

			if (lightPath)
			{
				lightPath->ChangeRoll(hand.palmNormal().roll() * -1.0f * ROLL_SENSITIVITY);
			}
		}
	}

	GetTargetNode()->SetNodePosition(basePosition + leapMotion);
	GetTargetNode()->Invalidate();

	if (lightPath)
	{
		Point3D position = GetTargetNode()->GetNodePosition();
		lightPath->ChangePitch(position.z * PITCH_SENSITIVITY);
		lightPath->ChangeYaw(position.y * YAW_SENSITIVITY * (float)TheTimeMgr->GetDeltaTime());
	}
}

void HandController::SetLightPath(LightPathController* lightPath)
{
	this->lightPath = lightPath;
}
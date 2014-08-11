#include "HandController.h"
#include "Game.h"

using namespace C4;

HandController::HandController() : Controller(kControllerAnimatedHand)
{
	lightPath = nullptr;
	animatedModelPath = "Model/Gauntlet_Animated";
	backward = 0.0f;
}

HandController::HandController(const HandController& handController) : 
	Controller(handController)
{
	backward = 0.0f;
}

HandController::HandController(const char* amp, bool b) : Controller(kControllerAnimatedHand)
{
	backward = b;
	animatedModelPath = amp;
}

Controller* HandController::Replicate(void) const
{
	return (new HandController(*this));
}

HandController::~HandController()
{
}

bool HandController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeGeometry || node->GetNodeType() == kNodeModel);
}

void HandController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);

	data << animatedModelPath;
	data << backward;
}

void HandController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);

	data >> animatedModelPath;
	data >> backward;
}

void HandController::Preprocess(void)
{
	//Animation setup and control:
	//Find animation model, set interpolator loop at start frame
	Controller::Preprocess();
	Model *myModel = GetTargetModel();
	frameAnimator = new FrameAnimator(myModel);
	myModel->SetRootAnimator(frameAnimator);
	Interpolator *interpolator = frameAnimator->GetFrameInterpolator();
	frameAnimator->SetAnimation(animatedModelPath);
	if (backward)
		interpolator->SetMode(kInterpolatorBackward | kInterpolatorLoop);
	else
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);

	startOrientation = GetTargetNode()->GetNodeTransform().GetMatrix3D();
	Node* root = GetTargetNode()->GetRootNode();
	Node* node = root;
	do
	{
		if (node->GetController())
		{
			if (node->GetController()->GetControllerType() == kControllerPlayer)
			{
				player = (MainPlayerController*)(node->GetController());
			}
		}
		node = root->GetNextNode(node);
	} while (node);
}

void HandController::Move(void)
{
	//animate model
	Model *myModel = GetTargetModel();
	myModel->Animate();

	// TODO: Set up basePosition based on player position
	Point3D basePosition(2.0f, 0.0f, 0.5f);
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

		lightPath->ChangePitch(leapMotion.z * PITCH_SENSITIVITY);
		lightPath->ChangeYaw(leapMotion.y * YAW_SENSITIVITY * (float)TheTimeMgr->GetDeltaTime());
	}

}

void HandController::SetLightPath(LightPathController* lightPath)
{
	this->lightPath = lightPath;
}
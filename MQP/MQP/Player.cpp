#include "Player.h"
#include "Game.h"

using namespace C4;


MainPlayerController::MainPlayerController(float azimuth) :
		CharacterController(kControllerPlayer),
		playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;

	modelAzimuth = azimuth;
	modelAltitude = 0.0F;
}

MainPlayerController::MainPlayerController() :
	CharacterController(kControllerPlayer),
	playerInteractor(this)
{
}

MainPlayerController::MainPlayerController(const MainPlayerController& playerController) :
		CharacterController(playerController),
		playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;

	modelAzimuth = 0.0F;
	modelAltitude = 0.0F;
}

MainPlayerController::~MainPlayerController()
{
}

//make duplicate of controller with pointer to current controller
Controller *MainPlayerController::Replicate(void) const
{
	return (new MainPlayerController(*this));
}

bool MainPlayerController::ValidNode(const Node *node)
{
	return ((node) && (node->GetNodeType() == kNodeModel) || node->GetNodeType() == kNodeGeometry);
}

void MainPlayerController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);
}

void MainPlayerController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
}

void MainPlayerController::Preprocess(void)
{
	//This function is called once before the target node is ever
	//rendered or moved. The base class PreProcess() function should
	// always be called first, and then the subclass can do whatever 
	//preprocessing it needs to do.

	Controller::Preprocess();

	//TODO: Set rigid body flags
	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation);
	SetFrictionCoefficient(0.0F);

	//We use a frame animator to play animation resources
	//for the player model

	Model *player = GetTargetNode();
	frameAnimator.SetTargetModel(player);
	player->SetRootAnimator(&frameAnimator);

	//Initialize the previous center of mass to the current center of mass
	//so that this doesn't contain garbage the first time we call ActivateTriggers()

	previousCenterOfMass = GetFinalWorldTransform() * GetCenterOfMass();

	//Register our interactor with the world.

	player->GetWorld()->AddInteractor(&playerInteractor);
}

void MainPlayerController::Move(void)
{
	//This is called once per frame to allow the controller to 
	//move its target node.

}

void MainPlayerController::SetPlayerMotion(int32 motion){
	//This function sets the animation resource corresponding to 
	//the current type of motion assigned to the player

	Interpolator *interpolator = frameAnimator.GetFrameInterpolator();

	if (motion == kMotionStand)
	{
		frameAnimator.SetAnimation("player/Stand");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}
	else if (motion == kMotionForward)
	{
		frameAnimator.SetAnimation("player/Forward");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}
}

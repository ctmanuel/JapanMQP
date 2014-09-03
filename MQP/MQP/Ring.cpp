#include "Ring.h"
#include "Game.h"

using namespace C4;

RingController::RingController() :
CharacterController(kControllerRing)
{
}

RingController::RingController(const RingController& ringController) :
CharacterController(ringController)
{
}

RingController::~RingController()
{
}

Controller* RingController::Replicate(void) const
{
	return (new RingController(*this));
}

bool RingController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeModel);
}

void RingController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);
}

void RingController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
}

void RingController::Preprocess(void)
{
	CharacterController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake);
	SetFrictionCoefficient(0.0F);
	//Give it 0 gravity
	SetGravityMultiplier(0.0F);
	
	/*Potential TODO
	//use frame animator to play animation resources for the hand model
	Model *myModel = GetTargetModel();
	frameAnimator = new FrameAnimator(myModel);
	myModel->SetRootAnimator(frameAnimator);
	Interpolator *interpolator = frameAnimator->GetFrameInterpolator();
	frameAnimator->SetAnimation(animatedModelPath);
	if (backward)
		interpolator->SetMode(kInterpolatorBackward | kInterpolatorLoop);
	else
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
		*/
}

void RingController::Move(void)
{

	if (active){
		if (expandTimer > 0){
			expandTimer--;
		}
		else if (expandTimer <= 0){
			ExpandRings(0.5);
			expandTimer = 60;
			active = false;
		}
	}

}

void RingController::ExpandRings(float size){
	active = true;
	Transform4D t = GetTargetNode()->GetNodeTransform();
	Point3D p = GetTargetNode()->GetNodePosition();
	t.SetScale(size);
	SetRigidBodyTransform(t);
	SetRigidBodyPosition(p);

}
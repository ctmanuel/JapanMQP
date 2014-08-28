#include "Ring.h"
#include "Game.h"

using namespace C4;

RingController::RingController() :
	Controller(kControllerRing)
{
	active = true;
}

RingController::RingController(const RingController& ringController) :
	Controller(ringController)
{
	active = true;
}

RingController::~RingController()
{
}

Controller* RingController::Replicate(void) const
{
	return (new RingController(*this));
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
	Controller::Preprocess();
}

void RingController::Move(void)
{
	if (expandTimer >= 0){
		expandTimer--;
	}
	if (expandTimer <= 0){
		Expand(-5)
	}

}

void RingController::Expand(float size){

	Node *thisnode = GetTargetNode();
	Transform4D transform;
	transform = thisnode->GetNodeTransform();
	transform.SetScale(size);
	thisnode->SetNodeTransform(transform);
}
#include "Ring.h"

using namespace C4;

RingController::RingController() :
	Controller(kControllerRing)
{
}

RingController::RingController(const RingController& ringController) :
	Controller(ringController)
{
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

}
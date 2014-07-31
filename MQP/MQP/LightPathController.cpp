#include "LightPathController.h"
#include "Game.h"

using namespace C4;

LightPathController::LightPathController() : Controller(kControllerLightPath)
{
}

LightPathController::LightPathController(const LightPathController& lightPathController) :
Controller(lightPathController)
{
}

Controller* LightPathController::Replicate(void) const
{
	return (new LightPathController(*this));
}

LightPathController::~LightPathController()
{
}

bool LightPathController::ValidNode(const Node* node)
{
	return ((node) && (node->GetNodeType() == kNodeGeometry));
}

void LightPathController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);
}

void LightPathController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
}

void LightPathController::Preprocess(void)
{
	Controller::Preprocess();
}

void LightPathController::Move(void)
{
	counter++;

	Matrix3D stretch;
	stretch.SetScale(0.1f * counter, 1.0f, 1.0f);

	GetTargetNode()->SetNodeMatrix3D(Transform4D(stretch));
	GetTargetNode()->Invalidate();
}
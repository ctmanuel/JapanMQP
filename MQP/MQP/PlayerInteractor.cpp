#include "C4World.h"
 #include "C4Engine.h"
 #include "Player.h"
 #include "PlayerInteractor.h"

using namespace C4;

PlayerInteractor::PlayerInteractor(MainPlayerController *controller)
 {
	playerController = controller;
	}

PlayerInteractor::PlayerInteractor()
 {
	
		}

PlayerInteractor::~PlayerInteractor()
 {
	}

void PlayerInteractor::HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position)
{
	//Always call the base class counterpart

	Interactor::HandleInteractionEvent(type, node, position);

	//If the node with which we are interacting has a controller, 
	//pass the event through to that controller
	Controller *controller = node->GetController();
	if (controller)
	{
		controller->HandleInteractionEvent(type, position);
	}
}

#include "C4Engine.h"

#include "GameCamera.h"
#include "Player.h"
#include "Game.h"

using namespace C4;

extern Game* TheGame;

GameCamera::GameCamera() : FrustumCamera(2.0F, 1.0F)
{
	playerNode = nullptr;
	lookedForPlayer = false;
}

GameCamera::~GameCamera()
{
}

void GameCamera::Preprocess(void)
{
	FrustumCamera::Preprocess();
}

Node *GameCamera::GetPlayerNode()
{
	World* wrld = TheWorldMgr->GetWorld();
	Node* root = wrld->GetRootNode();
	Node* node = root;
	do
	{
		if (node->GetController())
		{
			if (node->GetController()->GetControllerType() == kControllerPlayer)
			{
				// Make this a subnode of player
				return node;
			}
		}
		node = root->GetNextNode(node);
	} while (node);

	return nullptr;
}

void GameCamera::Move(void)
{
	if (inTransition){
		bias -= 0.008f;
		Transition();
	}
	
	if (bias <= 0.0f){
		inTransition = false;
		bias = 1.0f;
	}

	if (playerNode != nullptr){
		Point3D oldPos = GetNodePosition();

		SetNodePosition(Point3D(0.0f, 0.0f, 1.0f));
		//SetNodePosition(Point3D(0.0f, 0.0f, 1.64f));

		// temp
		//SetNodePosition(Point3D(-10.0f, 0.0f, 3.0f));

		LookAtPoint(Point3D(1.0f, 0.0f, 1.26f));
		//if (TheMessageMgr->Server())
			SetNodeTransform(playerNode->GetNodeTransform() * GetNodeTransform());

		// Potentially set off triggers
		TheWorldMgr->GetWorld()->ActivateTriggers(oldPos, GetNodePosition(), 0.0f);
	}
	else if (lookedForPlayer) // There is no player in this world. It's probably the menu
	{
		SetNodePosition(Point3D(0.0f, 0.0f, 1.64f));

		// temp
		//SetNodePosition(Point3D(-2.0f, 0.0f, 1.64f));

		LookAtPoint(Point3D(1.0f, 0.0f, 1.64f));
	}
	else
	{
		lookedForPlayer = true;
		playerNode = GetPlayerNode();
	}

	// These two lines handle Rift head tracking
	Matrix3D m = GetNodeTransform().GetMatrix3D() * Inverse((TheGame->lookOrigin).GetRotationMatrix());
	SetNodeMatrix3D(m * TheWorldMgr->GetTrackingOrientation().GetRotationMatrix());
}

void GameCamera::Reset(void)
{
	TheGame->lookOrigin = TheWorldMgr->GetTrackingOrientation();
}

void GameCamera::Transition(void){
	//Engine::Report("In transition");
	inTransition = true;
	ColorRGBA r = ColorRGBA(1, 1, 1);
	ColorRGBA a = ColorRGBA(bias, bias, bias, bias);
	TheWorldMgr->GetWorld()->SetFinalColorTransform(r, a);
}
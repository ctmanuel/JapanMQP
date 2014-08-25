#include "GameCamera.h"
#include "Player.h"
#include "C4Engine.h"

using namespace C4;

GameCamera::GameCamera() : FrustumCamera(2.0F, 1.0F)
{
	origin = TheWorldMgr->GetTrackingOrientation();
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
	if (playerNode != nullptr){
		SetNodePosition(Point3D(0.0f, 0.0f, 1.64f));

		// temp
		SetNodePosition(Point3D(-10.0f, 0.0f, 3.0f));

		LookAtPoint(Point3D(1.0f, 0.0f, 1.64f));

		SetNodeTransform(playerNode->GetNodeTransform() * GetNodeTransform());
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
	Matrix3D m = GetNodeTransform().GetMatrix3D() * Inverse(origin.GetRotationMatrix());
	SetNodeMatrix3D(m * TheWorldMgr->GetTrackingOrientation().GetRotationMatrix());
}

void GameCamera::Reset(void)
{
	origin = TheWorldMgr->GetTrackingOrientation();
}

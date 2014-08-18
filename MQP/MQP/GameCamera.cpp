#include "GameCamera.h"
#include "Player.h"
#include "C4Engine.h"

using namespace C4;

GameCamera::GameCamera() : FrustumCamera(2.0F, 1.0F)
{
	origin = TheWorldMgr->GetTrackingOrientation();
	playerModel = Model::Get(kModelPlayer);
	playerNode = nullptr;
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
				node->AddNewSubnode(this);
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
		Point3D playerpos = playerNode->GetNodePosition();
		SetNodePosition(Point3D(0.0f, 0.0f, 1.64f));

		// temp
		SetNodePosition(Point3D(-10.0f, 0.0f, 3.0f));

		LookAtPoint(Point3D(1.0f, 0.0f, 1.64f));
	}
	else{
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

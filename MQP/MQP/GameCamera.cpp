#include "GameCamera.h"
#include "Player.h"
#include "C4Engine.h"

using namespace C4;

GameCamera::GameCamera() : FrustumCamera(2.0F, 1.0F)
{
	origin = TheWorldMgr->GetTrackingOrientation();
	playerModel = Model::Get(kModelPlayer);
}

GameCamera::~GameCamera()
{
}

void GameCamera::Preprocess(void)
{
	FrustumCamera::Preprocess();
}

void GameCamera::Move(void)
{
/*	if (model)
	{
		//find player controller
		PlayerController *controller = static_cast<PlayerController *>(model->GetController());

		//calculate locat coordinate frame for the camera based on the direction the player is looking
		const Point3D& startPosition = model->GetWorldPosition();
		SetNodePosition(Point3D(startPosition.x, startPosition.y, startPosition.z));
		LookAtPoint(Point3D(100.0F, 0.0F, 1.0F));
		Matrix3D m = GetNodeTransform().GetMatrix3D() * Inverse(origin.GetRotationMatrix());
		SetNodeMatrix3D(m * TheWorldMgr->GetTrackingOrientation().GetRotationMatrix());
	}*/
//	else
//	{
	Point3D startPosition = playerModel->GetNodePosition();
	SetNodePosition(Point3D(startPosition.x+1.5F, startPosition.y+1.5F, startPosition.z+1.5F));
		//SetNodePosition(Point3D(4.0f, 0.0f, 4.0f));
		LookAtPoint(Point3D(100.0F, 0.0F, 1.0F));
		Matrix3D m = GetNodeTransform().GetMatrix3D() * Inverse(origin.GetRotationMatrix());
		SetNodeMatrix3D(m * TheWorldMgr->GetTrackingOrientation().GetRotationMatrix());
//	}
	/*
	Node *root = TheWorldMgr->GetWorld()->GetRootNode();
	Node *node = root;
	do
	{
		if (node->GetNodeName() =="Player"){
			Engine::Report("Thisii a repotr");
			startPosition = node->GetNodePosition();
		}
		Engine::Report(node->GetNodeName());
		node = root->GetNextNode(node);
	} while (node); */
}

void GameCamera::Reset(void)
{
	origin = TheWorldMgr->GetTrackingOrientation();
}

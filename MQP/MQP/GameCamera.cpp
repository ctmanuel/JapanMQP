#include "GameCamera.h"
#include "Player.h"
#include "C4Engine.h"

using namespace C4;

GameCamera::GameCamera() : FrustumCamera(2.0F, 1.0F)
{
	origin = TheWorldMgr->GetTrackingOrientation();
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
	Point3D startPosition = Point3D(0.0, 0.0, 1.0);
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
	} while (node);

	SetNodePosition(Point3D(startPosition.x , startPosition.y, startPosition.z));
	LookAtPoint(Point3D(100.0F, 0.0F, 1.0F));
	Matrix3D m = GetNodeTransform().GetMatrix3D() * Inverse(origin.GetRotationMatrix());
	SetNodeMatrix3D(m * TheWorldMgr->GetTrackingOrientation().GetRotationMatrix());
}

void GameCamera::Reset(void)
{
	origin = TheWorldMgr->GetTrackingOrientation();
}

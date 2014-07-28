#include "GameCamera.h"

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
	SetNodePosition(Point3D(-5.0F, 0.0F, 1.0F));
	LookAtPoint(Point3D(100.0F, 0.0F, 1.0F));
	Matrix3D m = GetNodeTransform().GetMatrix3D() * Inverse(origin.GetRotationMatrix());
	SetNodeMatrix3D(m * TheWorldMgr->GetTrackingOrientation().GetRotationMatrix());
}

void GameCamera::Reset(void)
{
	origin = TheWorldMgr->GetTrackingOrientation();
}

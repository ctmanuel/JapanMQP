#include "GameCamera.h"

using namespace C4;

GameCamera::GameCamera() : FrustumCamera(2.0F, 1.0F)
{
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
	SetNodePosition(Point3D(4.0F, 4.0F, 3.0F));
	SetNodeMatrix3D(TheWorldMgr->GetTrackingOrientation().GetRotationMatrix());
}

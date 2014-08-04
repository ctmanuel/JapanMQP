#include "GameCamera.h"
#include "Player.h"
#include "C4Engine.h"
#include "Game.h"

using namespace C4;

GameCamera::GameCamera() : FrustumCamera(2.0F, 1.0F)
{
	origin = TheWorldMgr->GetTrackingOrientation();
	playerModel = Model::Get(kModelPlayer);

	//find player position, set camera start position to it
	Point3D startPosition = playerModel->GetNodePosition();
	SetNodePosition(Point3D(startPosition.x + 1.5F, startPosition.y + 1.5F, startPosition.z + 1.5F));
	

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
	/*Note, LookAtPoint needs to be called every frame*/
	LookAtPoint(Point3D(0.0F, 0.0F, 1.0F)); 

	Matrix3D m = GetNodeTransform().GetMatrix3D() * Inverse(origin.GetRotationMatrix());
	SetNodeMatrix3D(m * TheWorldMgr->GetTrackingOrientation().GetRotationMatrix());
	//Point3D currentpos = GetNodePosition();
	//SetNodePosition(Point3D(currentpos.x , currentpos.y, currentpos.z));
	//Engine::Report("This %f", TheWorldMgr->GetTrackingOrientation().GetDirectionX().x);

}

void GameCamera::Reset(void)
{
	origin = TheWorldMgr->GetTrackingOrientation();
	
}

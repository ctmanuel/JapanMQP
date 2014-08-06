#include "Player.h"

using namespace C4;


MainPlayerController::MainPlayerController(float azimuth) :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;
	
	speed = START_SPEED;
	splinePoints.push_back(SplineVector3D(0.0F, 0.0F, 0.0F));

	modelAzimuth = azimuth;
	modelAltitude = 0.0F;
}
 MainPlayerController::MainPlayerController() :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	 speed = START_SPEED;
	 splinePoints.push_back(SplineVector3D(0.0F, 0.0F, 0.0F));
}

 MainPlayerController::MainPlayerController(const MainPlayerController& playerController) :
CharacterController(playerController),
playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;
	
	speed = START_SPEED;
	splinePoints.push_back(SplineVector3D(0.0F, 0.0F, 0.0F));

	modelAzimuth = 0.0F;
	modelAltitude = 0.0F;
}

 MainPlayerController::~MainPlayerController()
{
	
}

//make duplicate of controller with pointer to current controller
Controller *MainPlayerController::Replicate(void) const
 {
	return (new MainPlayerController(*this));
}

 bool MainPlayerController::ValidNode(const Node *node)
{
	return ((node) && (node->GetNodeType() == kNodeModel) || node->GetNodeType() == kNodeGeometry);
}

 void MainPlayerController::Pack(Packer& data, unsigned long packFlags) const
{
	Controller::Pack(data, packFlags);
}

 void MainPlayerController::Unpack(Unpacker& data, unsigned long unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
}

 void MainPlayerController::Preprocess(void)
{
	//This function is called once before the target node is ever
	//rendered or moved. The base class PreProcess() function should
	// always be called first, and then the subclass can do whatever 
	//preprocessing it needs to do.
		
	Controller::Preprocess();
}

void MainPlayerController::LightpathNode(Node *node){
	//lightPathNodes.push(node);

	splinePoints.push_back(SplineVector3D(node->GetNodePosition().x, node->GetNodePosition().y, node->GetNodePosition().z));
}

void MainPlayerController::Move(void)
{
	float yaw = atan2(lightPathFront.y() - GetTargetNode()->GetNodePosition().y, lightPathFront.x() - GetTargetNode()->GetNodePosition().x);
	Matrix3D rotation;
	rotation.SetEulerAngles(0.0f, 0.0f, yaw);
	GetTargetNode()->SetNodeMatrix3D(rotation);

	if (splinePoints.size() >= 4)
	{
		std::vector<SplineVector3D> lp = splinePoints;
		lp.push_back(lightPathFront);
		SplineVector3D prev = lp[lp.size() - 2];
		SplineVector3D diff = (lightPathFront - prev) / 1000.0f;
		SplineVector3D next = lightPathFront + diff;
		lp.push_back(next);

		std::shared_ptr<Spline> spline = std::make_shared<CRSpline>(lp, 1.0F);
		SplineLengthCalculator lengthCalc(spline);

		double length = lengthCalc.findLength(0, 1, false, 1.0f);
		length *= spline->getMaxT();

		SplineVector3D pos = spline->getPosition(((length - DISTANCE_TO_PATH) / length) * spline->getMaxT());

		GetTargetNode()->SetNodePosition(Point3D(pos.x(), pos.y(), pos.z()));

		if (splinePoints.size() > MAX_SPLINE_POINTS)
		{
			splinePoints.erase(splinePoints.begin());
		}
	}
	GetTargetNode()->Invalidate();
 }

void MainPlayerController::ReportLightpathFront(Point3D front)
{
	SplineVector3D frontv(front.x, front.y, front.z);
	lightPathFront = frontv;
}

Point3D MainPlayerController::GetDestination()
{
	return Point3D(lightPathFront.x(), lightPathFront.y(), lightPathFront.z());
}

void MainPlayerController::SetPlayerMotion(int32 motion){
	//This function sets the animation resource corresponding to 
	//the current type of motion assigned to the player
		
	Interpolator *interpolator = frameAnimator.GetFrameInterpolator();
	
	if (motion == kMotionStand)
	{
		frameAnimator.SetAnimation("player/Stand");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}
	else if (motion == kMotionForward)
	{
		frameAnimator.SetAnimation("player/Forward");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}
	
}

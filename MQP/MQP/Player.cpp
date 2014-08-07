#include "Player.h"

using namespace C4;


MainPlayerController::MainPlayerController(float azimuth) :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;
	
	speed = START_SPEED;

	modelAzimuth = azimuth;
	modelAltitude = 0.0F;
}

MainPlayerController::MainPlayerController() :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	 speed = START_SPEED;
}

 MainPlayerController::MainPlayerController(const MainPlayerController& playerController) :
CharacterController(playerController),
playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;
	
	speed = START_SPEED;

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
	// rendered or moved. The base class PreProcess() function should
	// always be called first, and then the subclass can do whatever 
	// preprocessing it needs to do.
		
	 Controller::Preprocess();

	 // Spline needs at least two points in front of the player and two points behind.
	 // These are the points behind.
	 Point3D position = GetTargetNode()->GetNodePosition();
	 splinePoints.push_back(SplineVector3D(position.x - 2.0f, position.y, position.z));
	 splinePoints.push_back(SplineVector3D(position.x - 1.0f, position.y, position.z));
}

void MainPlayerController::LightpathNode(Node *node){
	//lightPathNodes.push(node);

	splinePoints.push_back(SplineVector3D(node->GetNodePosition().x, node->GetNodePosition().y, node->GetNodePosition().z));
}

void MainPlayerController::Move(void)
{
	// Set up spline
	std::vector<SplineVector3D> lp = splinePoints;
	lp.push_back(lightPathFront);
	SplineVector3D prev = lp[lp.size() - 2];
	SplineVector3D diff = (lightPathFront - prev) / 1000.0f;
	SplineVector3D next = lightPathFront + diff;
	lp.push_back(next);

	// Find distance along spline
	std::shared_ptr<Spline> spline = std::make_shared<CRSpline>(lp, 1.0F);
	SplineLengthCalculator lengthCalc(spline);
	double length = lengthCalc.findLength(0, 1, false, 1.0f);
	length *= spline->getMaxT();

	// Move
	SplineVector3D pos = spline->getPosition(((length - DISTANCE_TO_PATH) / length) * spline->getMaxT());
	GetTargetNode()->SetNodePosition(Point3D(pos.x(), pos.y(), pos.z()));

	// Face front of path
	Point3D position = GetTargetNode()->GetNodePosition();
	float yaw = atan2(lightPathFront.y() - position.y, lightPathFront.x() - position.x);
	float horiz = sqrt(pow(lightPathFront.x() - position.x, 2) + pow(lightPathFront.y() - position.y, 2));
	float pitch = atan2(horiz, lightPathFront.z() - position.z) - K::pi_over_2;
	Matrix3D rotation;
	rotation.SetEulerAngles(0.0f, pitch, yaw);
	GetTargetNode()->SetNodeMatrix3D(rotation);

	// Keep set of points below max
	if (splinePoints.size() > MAX_SPLINE_POINTS)
	{
		splinePoints.erase(splinePoints.begin());
	}

	// Always call this after moving a node
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

void MainPlayerController::SetPlayerMotion(int32 motion)
{
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


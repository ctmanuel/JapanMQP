#include "Player.h"
#include "Game.h"

using namespace C4;

extern Game* TheGame;

MainPlayerController::MainPlayerController(float azimuth) :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	playerMotion = kMotionNone;
	movementFlags = 0;
	
	speed = START_SPEED;

	modelAzimuth = azimuth;
	modelAltitude = 0.0F;

	levelTime = 0;
}

MainPlayerController::MainPlayerController() :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	speed = START_SPEED;

	levelTime = 0;
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

	levelTime = 0;
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
		
	 CharacterController::Preprocess();

	 SetRigidBodyFlags(kRigidBodyFixedOrientation);

	 // Spline needs at least two points in front of the player and two points behind.
	 // These are the points behind.
	 Point3D position = GetTargetNode()->GetNodePosition();
	 splinePoints.push_back(SplineVector3D(position.x - 2.0f, position.y, position.z));
	 splinePoints.push_back(SplineVector3D(position.x - 1.0f, position.y, position.z));

	 // Start level time at 0
	 levelTime = 0;
}

void MainPlayerController::LightpathNode(Node *node){
	splinePoints.push_back(SplineVector3D(node->GetNodePosition().x, node->GetNodePosition().y, node->GetNodePosition().z));
}

void MainPlayerController::Move(void)
{
	// Update time
	levelTime += TheTimeMgr->GetDeltaTime();

	// Face front of path
	Vector3D d = Point3D(lightPathFront.x(), lightPathFront.y(), lightPathFront.z()) - GetTargetNode()->GetNodePosition();
	float horiz = sqrt((d.x * d.x) + (d.y * d.y));
	float pitch = atan2(horiz, d.z) - K::pi_over_2;
	Matrix3D pitchm, yawm;
	pitchm.SetRotationAboutY(pitch);
	float yaw = atan2(d.y, d.x);
	yawm.SetRotationAboutZ(yaw);
	SetRigidBodyMatrix3D(yawm * pitchm);

	// Set up spline
	std::vector<SplineVector3D> lp = splinePoints;
	if (lightPathFront != lp.back())
	{
		lp.push_back(lightPathFront);
		SplineVector3D prev = lp[lp.size() - 2];
		SplineVector3D diff = (lightPathFront - prev) / 1000.0f;
		SplineVector3D next = lightPathFront + diff;
		lp.push_back(next);
	}
	else
	{
		SplineVector3D diff = (lp.back() - lp[lp.size() - 2]) / 1000.0f;
		lp.push_back(lp.back() + diff);
	}

	// Find distance along spline
	std::shared_ptr<Spline> spline = std::make_shared<CRSpline>(lp, 1.0F);
	SplineLengthCalculator lengthCalc(spline);
	double length = lengthCalc.findLength(0, 1, false, 1.0f);
	length *= spline->getMaxT();

	// Save old height to track height change
	float oldZ = GetTargetNode()->GetNodePosition().z;

	// Move
	Point3D oldPos = GetTargetNode()->GetNodePosition();
	SplineVector3D pos = spline->getPosition(((length - DISTANCE_TO_PATH) / length) * spline->getMaxT());
	Point3D newPos = Point3D(pos.x(), pos.y(), pos.z());	//going to this position
	Vector3D movement = (newPos - oldPos) / (speed * TheTimeMgr->GetFloatDeltaTime());

	if (!isnan(movement.x) && !isnan(movement.y) && !isnan(movement.z))
	{
		if (abs(movement.x) > 0.01f || abs(movement.y) > 0.01f || abs(movement.z) > 0.01f)
		{
			newPos = (oldPos + (movement * speed * TheTimeMgr->GetFloatDeltaTime() / 30.0f));
		}
	}
	direction = movement;
	SetRigidBodyPosition(newPos);

	// Change speed based on uphill/downhill
	float climb = oldZ - GetTargetNode()->GetNodePosition().z;
	speed += climb * HILL_ACCELERATION;

	// Slow down if turning too sharply and not banking
	SplineVector3D bpos = spline->getPosition((((length - DISTANCE_TO_PATH) - 0.1f) / length) * spline->getMaxT());
	SplineVector3D fpos = spline->getPosition((((length - DISTANCE_TO_PATH) + 0.1f) / length) * spline->getMaxT());
	float curve = speed * (atan2(fpos.x() - pos.x(), fpos.y() - pos.y()) - atan2(pos.x() - bpos.x(), pos.y() - bpos.y()));
	float roll = 0.0f;
	if (!rollHistory.empty())
	{
		int index = rollHistory.size() - (int)ceil((DISTANCE_TO_PATH / speed) / ((float)ROLL_REPORT_FREQUENCY / 1000.0f));
		if (index > 0)
		{
			roll = rollHistory[index];
			std::vector<float>::iterator begin = rollHistory.begin();
			std::vector<float>::iterator upto = begin;
			std::advance(upto, rollHistory.size() - index);
			rollHistory.erase(begin, begin + index);
		}
		else
		{
			roll = rollHistory.front();
		}
	}

	bool turnSlow = (abs(curve) > TURN_SLOW_THRESHOLD) && (((curve / abs(curve)) * roll) < ROLL_REQUIREMENT);
	if (turnSlow)
	{
		speed -= TURN_ACCELERATION * TheTimeMgr->GetFloatDeltaTime() / 1000.0f;
	}

	// Base acceleration
	if ((speed < BASE_SPEED) && ((-1.0f * climb / (TheTimeMgr->GetFloatDeltaTime() / 1000.0f)) < BASE_CLIMB_THRESHOLD) && !turnSlow)
	{
		speed += BASE_ACCELERATION * TheTimeMgr->GetFloatDeltaTime() / 1000.0f;
	}

	// Maintain speed limits
	if (speed > MAX_SPEED)
	{
		speed = MAX_SPEED;
	}
	if (speed < MIN_SPEED)
	{
		speed = MIN_SPEED;
	}

	// Keep set of points below max
	if (splinePoints.size() > MAX_SPLINE_POINTS)
	{
		splinePoints.erase(splinePoints.begin());
	}

	// Always call this after moving a node
	GetTargetNode()->Invalidate();

	/*
	// Face direction of movement
	Vector3D d = GetTargetNode()->GetNodePosition() - oldPos;
	float horiz = sqrt((d.x * d.x) + (d.y * d.y));
	float pitch = atan2(horiz, d.z) - K::pi_over_2;
	Matrix3D pitchm, yawm;
	pitchm.SetRotationAboutY(pitch);
	float yaw = atan2(d.y, d.x);
	yawm.SetRotationAboutZ(yaw);
	GetTargetNode()->SetNodeMatrix3D(yawm * pitchm);
	*/

	// Potentially set off triggers
	GetTargetNode()->GetWorld()->ActivateTriggers(oldPos, newPos, 0.0f);
 }

void MainPlayerController::ReportLightpathFront(Point3D front)
{
	SplineVector3D frontv(front.x, front.y, front.z);
	lightPathFront = frontv;
}

void MainPlayerController::ReportRoll(float roll)
{
	rollHistory.push_back(roll);
}

Point3D MainPlayerController::GetLightPathFront(void)
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

RigidBodyStatus MainPlayerController::HandleNewGeometryContact(const GeometryContact* contact)
{
	TheGame->SetLevelEndState(levelEndFailed);
	TheGame->StartLevel("Menu");
	return kRigidBodyUnchanged;
}

RigidBodyStatus MainPlayerController::HandleNewRigidBodyContact(const RigidBodyContact* contact, RigidBodyController* contactBody)
{
	TheGame->SetLevelEndState(levelEndFailed);
	TheGame->StartLevel("Menu");
	return kRigidBodyUnchanged;
}
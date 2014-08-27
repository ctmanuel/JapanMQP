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
	powerUp = powerUpNone;

	TheGame->SetPlayerController(this);
}

MainPlayerController::MainPlayerController() :
CharacterController(kControllerPlayer),
playerInteractor(this)
{
	speed = START_SPEED;

	levelTime = 0;
	powerUp = powerUpNone;

	TheGame->SetPlayerController(this);
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
	powerUp = powerUpNone;

	TheGame->SetPlayerController(this);
}

 MainPlayerController::~MainPlayerController()
{
	 pathSound->Stop();
	 pathSound->Release();

	 if (banking)
	 {
		 bankSound->VaryVolume(0.0f, 500, true);
	 }
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

	 // Play path sound effect
	 pathSound = new Sound;
	 WaveStreamer* streamer = new WaveStreamer;
	// streamer->AddComponent("SoundEffects/path");
	// pathSound->Stream(streamer);
	 pathSound->Load("SoundEffects/path-looping");
	 pathSound->SetLoopCount(kSoundLoopInfinite);
	 pathSound->Play();

	 banking = false;
}

void MainPlayerController::LightpathNode(Node *node){
	splinePoints.push_back(SplineVector3D(node->GetNodePosition().x, node->GetNodePosition().y, node->GetNodePosition().z));
}

void MainPlayerController::Move(void)
{
	// temp
	switch (powerUp)
	{
	case powerUpNone:
		TheEngine->Report("You have no power up");
		break;
	case powerUpSpeedBoost:
		TheEngine->Report("You have speed boost");
		break;
	case powerUpRingExpander:
		TheEngine->Report("You have ring expander");
		break;
	}

	// Update time
	levelTime += TheTimeMgr->GetDeltaTime();
	TheGame->SetLastLevelTime(levelTime);

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

	bool turnSlow = false;// = (abs(curve) > TURN_SLOW_THRESHOLD) && (((curve / abs(curve)) * roll) < ROLL_REQUIREMENT);
	bool bankingPrev = banking;
	if ((abs(curve) > TURN_SLOW_THRESHOLD))
	{
		if ((((curve / abs(curve)) * roll) < ROLL_REQUIREMENT))
		{
			speed -= TURN_ACCELERATION * TheTimeMgr->GetFloatDeltaTime() / 1000.0f;
			turnSlow = true;
			banking = false;
		}
		else
		{
			if (!banking)
			{
				banking = true;
				bankSound = new Sound;
				bankSound->Load("SoundEffects/bank-looping");
				bankSound->SetLoopCount(kSoundLoopInfinite);
				bankSound->Delay(10);
				bankSound->VaryVolume(0.0f, 0);
				bankSound->VaryVolume(0.5f, 500);
			}
		}
	}
	else
	{
		banking = false;
	}
	// If banking is ending this frame, stop sound
	if (bankingPrev && !banking)
	{
		bankSound->VaryVolume(0.0f, 500, true);
	}

	// temp
	if (banking)
	{
		TheEngine->Report("banking");
	}
	if (turnSlow)
	{
		TheEngine->Report("turn slow");
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

	// Adjust path sound frequency based on speed
	pathSound->VaryFrequency(speed / START_SPEED, 0);

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

void MainPlayerController::AddSpeed(float speedChange)
{
	speed += speedChange;

	if (speed > MAX_SPEED)
	{
		speed = MAX_SPEED;
	}
	if (speed < MIN_SPEED)
	{
		speed = MIN_SPEED;
	}
}

PowerUp MainPlayerController::GetPowerUp(void)
{
	return powerUp;
}

void MainPlayerController::SetPowerUp(PowerUp powerUp)
{
	this->powerUp = powerUp;
}

void MainPlayerController::UsePowerUp(void)
{
	switch (powerUp)
	{
	case powerUpSpeedBoost:
		// do something

		// temp
		TheEngine->Report("Using speed boost!");

		break;
	case powerUpRingExpander:
		// do someting

		// temp
		TheEngine->Report("Using ring expander!");

		break;
	}

	powerUp = powerUpNone;
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
	Geometry* geometry = contact->GetContactGeometry();
	if (geometry->GetNodeName() && Text::CompareText(geometry->GetNodeName(), "downer"))
	{
		Sound* sound = new Sound;
		sound->Load("SoundEffects/downer");
		sound->Play();
		SetLinearVelocity(GetOriginalLinearVelocity());
		SetExternalLinearResistance(Vector2D(0.0F, 0.0F));
		AddSpeed(-2.0f);
		GetPhysicsController()->PurgeGeometryContacts(geometry);
		delete geometry;
		return (kRigidBodyContactsBroken);
	}
	else if (geometry->GetNodeName() && Text::CompareText(geometry->GetNodeName(), "speedBoost"))
	{
		GetPhysicsController()->PurgeGeometryContacts(geometry);
		delete geometry;
		powerUp = powerUpSpeedBoost;
		return (kRigidBodyContactsBroken);
	}
	else if (geometry->GetNodeName() && Text::CompareText(geometry->GetNodeName(), "ringExpander"))
	{
		GetPhysicsController()->PurgeGeometryContacts(geometry);
		delete geometry;
		powerUp = powerUpRingExpander;
		return (kRigidBodyContactsBroken);
	}
	else
	{
		Sound* sound = new Sound;
		sound->Load("SoundEffects/crash");
		sound->Play();
		TheGame->SetLevelEndState(levelEndFailed);
		TheGame->StartLevel("Menu");
		return (kRigidBodyUnchanged);
	}
}

RigidBodyStatus MainPlayerController::HandleNewRigidBodyContact(const RigidBodyContact* contact, RigidBodyController* contactBody)
{
	Sound* sound = new Sound;
	sound->Load("SoundEffects/crash");
	sound->Play();
	TheGame->SetLevelEndState(levelEndFailed);
	TheGame->StartLevel("Menu");
	return kRigidBodyUnchanged;
}
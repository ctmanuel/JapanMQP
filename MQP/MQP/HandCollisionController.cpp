#include "Game.h"
#include "HandCollisionController.h"

HandCollisionController::HandCollisionController() : RigidBodyController(kControllerHandCollision)
{
	// This constructor is only called when a new ball model is created.
}

HandCollisionController::HandCollisionController(const HandCollisionController& handCollisionController) : RigidBodyController(handCollisionController)
{
	// This constructor is called when a ball controller is cloned.
}

HandCollisionController::~HandCollisionController()
{

}

Controller *HandCollisionController::Replicate(void) const
{
	return (new HandCollisionController(*this));
}

bool HandCollisionController::ValidNode(const Node *node)
{

	return (node->GetNodeType() == kNodeModel);
}

void HandCollisionController::Preprocess(void)
{
	// This function is called once before the target node is ever
	// rendered or moved. The base class Preprocess() function should
	// always be called first, and then the subclass can do whatever
	// preprocessing it needs to do. In this case, we set the ball's
	// coefficient of restitution.

	RigidBodyController::Preprocess();

	SetRestitutionCoefficient(0.95F);
	SetLinearVelocity(Math::RandomUnitVector3D() * 2.0F);
}

RigidBodyStatus HandCollisionController::HandleNewRigidBodyContact
(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	// This function is called when the ball makes contact with another rigid body.
	/*
	if (contact->get)
	{
		// Add a sound effect and some sparks to the world.
		
		Node *node = GetTargetNode();
		Zone *zone = node->GetWorld()->GetRootNode();
		Point3D position = node->GetWorldTransform() * contact->GetContactPosition();

		OmniSource *source = new OmniSource("model/Ball", 40.0F);
		source->SetNodePosition(position);
		zone->AddNewSubnode(source);

		SparkParticleSystem *sparks = new SparkParticleSystem(20);
		sparks->SetNodePosition(position);
		zone->AddNewSubnode(sparks);
		
		Engine::Report("Ran into something");
	}*/
	Engine::Report("Ran into something");
	return (kRigidBodyUnchanged);
}

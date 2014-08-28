#ifndef _HAND_COLLISION_CONTROL_H_
#define _HAND_COLLISION_CONTROL_H_


#include "C4World.h"

namespace C4
{
	enum
	{
		kControllerHandCollision = 'hndc'
	};

	class HandCollisionController : public RigidBodyController
	{
	private:

		HandCollisionController(const HandCollisionController& handCollisionController);

		Controller *Replicate(void) const override;

	public:

		HandCollisionController();
		~HandCollisionController();

		static bool ValidNode(const Node *node);

		void Preprocess(void) override;

		RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody);
	};
}

#endif

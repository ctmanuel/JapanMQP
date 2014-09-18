#pragma once

#include "C4Controller.h"
#include "C4Models.h"
#include "C4World.h"
#include "C4Animation.h"
#include "C4Character.h"

namespace C4
{
	enum
	{
		kControllerRing = 'ring'
	};

	enum
	{
		kModelRingSmall = 'rngs',
		kModelRingMedium = 'rngm',
		kModelRingLarge = 'rngl'
	};

	class RingController : public Controller
	{

	private:
		RingController(const RingController& ringController);
		Controller* Replicate(void) const;

	public:
		RingController();
		~RingController();

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		void Preprocess(void);
		void Move(void);
	};
}

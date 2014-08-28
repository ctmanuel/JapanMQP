#pragma once

#include "C4Controller.h"

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
		float expandTimer = 0;
		bool active;
		RingController(const RingController& ringController);
		Controller* Replicate(void) const;

	public:
		RingController();
		~RingController();
		
		void Expand(float size);

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		void Preprocess(void);
		void Move(void);
	};
}
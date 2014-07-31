#pragma once

#include "C4Controller.h"
#include "C4World.h"

namespace C4
{
	enum
	{
		kControllerLightPath = 'lpth'
	};

	class LightPathController : public Controller
	{
	private:

		LightPathController(const LightPathController& lightPathController);
		Controller* Replicate(void) const;

		int counter = 0;

	public:
		LightPathController();
		~LightPathController();

		static bool ValidNode(const Node* node);

		void Pack(Packer& data, unsigned long packFlags) const;
		void Unpack(Unpacker& data, unsigned long unpackFlags);

		void Preprocess(void);

		void Move(void);
	};
}

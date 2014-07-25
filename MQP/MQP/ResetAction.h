#pragma once

#include "C4Input.h"

namespace C4
{
	enum
	{
		kActionReset = 'rset'
	};
	class ResetAction : public Action
	{
	public:

		ResetAction(unsigned long type);
		~ResetAction();

		void Begin(void);
		void End(void);
	};
}

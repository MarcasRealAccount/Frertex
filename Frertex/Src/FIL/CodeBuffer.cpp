#include "Frertex/FIL/CodeBuffer.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::FIL
{
	void CodeBuffer::NOP()
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0001'0000);
	}

	void CodeBuffer::RET()
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0001'0001);
	}
} // namespace Frertex::FIL
#pragma once

#include <cstdint>

namespace Frertex::FIL
{
	enum class ETypeQualifier : std::uint32_t
	{
		None = 0,
		In,
		Out,
		InOut
	};
}
#pragma once

#include <cstdint>

#include <string_view>

namespace Frertex::FIL
{
	enum class ETypeQualifier : std::uint32_t
	{
		None = 0,
		In,
		Out,
		InOut
	};

	std::string_view TypeQualifierToString(ETypeQualifier typeQualifier);
} // namespace Frertex::FIL
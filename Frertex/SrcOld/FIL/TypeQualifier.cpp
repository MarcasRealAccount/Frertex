#include "Frertex/FIL/TypeQualifier.h"

namespace Frertex::FIL
{
	std::string_view TypeQualifierToString(ETypeQualifier typeQualifier)
	{
		switch (typeQualifier)
		{
		case ETypeQualifier::None: return "None";
		case ETypeQualifier::In: return "In";
		case ETypeQualifier::Out: return "Out";
		case ETypeQualifier::InOut: return "InOut";
		}
		return "?";
	}
} // namespace Frertex::FIL
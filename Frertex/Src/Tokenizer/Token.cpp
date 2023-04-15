// Auto generated

#include "Frertex/Tokenizer/Token.h"

namespace Frertex::Tokenizer
{
	std::string_view TokenClassToString(ETokenClass clazz)
	{
		switch (clazz)
		{
		case ETokenClass::Unknown: return "Unknown";
		case ETokenClass::Identifier: return "Identifier";
		case ETokenClass::String: return "String";
		case ETokenClass::BinaryInteger: return "BinaryInteger";
		case ETokenClass::OctalInteger: return "OctalInteger";
		case ETokenClass::DecimalInteger: return "DecimalInteger";
		case ETokenClass::HexInteger: return "HexInteger";
		case ETokenClass::Float: return "Float";
		case ETokenClass::HexFloat: return "HexFloat";
		case ETokenClass::Symbol: return "Symbol";
		case ETokenClass::Comment: return "Comment";
		case ETokenClass::MultilineComment: return "MultilineComment";
		}
		return "Unknown";
	}
} // namespace Frertex::Tokenizer
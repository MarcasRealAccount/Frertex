#include "Frertex/Parser/Literal.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Parser
{
	IntegerLiteral ParseIntegerLiteral(AST::Node* node, Source::Sources* sources)
	{
		PROFILE_FUNC;

		if (node->getType() != AST::EType::IntegerLiteral)
			return { 0, IntegerLiteral::ESize::None };

		IntegerLiteral lit      = { 0, IntegerLiteral::ESize::Int };
		bool           negative = false;
		std::size_t    radix    = 10;

		auto&       token     = node->getToken();
		auto        str       = token.getView(sources);
		std::size_t offset    = 0;
		std::size_t end       = str.size();
		std::size_t maxDigits = 0;

		switch (token.m_Class)
		{
		case Tokenizer::ETokenClass::Integer:
			if (str[0] == '-')
			{
				negative = true;
				++offset;
			}
			else if (str[0] == '+')
			{
				++offset;
			}
			break;
		case Tokenizer::ETokenClass::BinaryInteger:
			if (str[0] != '0' ||
			    (str[1] != 'b' && str[1] != 'B'))
			{
				// reportError(token.m_Index,
				//             token.m_Index,
				//	           { token.getSpan(m_Sources) },
				//	           token.getStart(m_Sources),
				//	           "Expected binary literal to start with '0b' or '0B'");
				return { 0, IntegerLiteral::ESize::None };
			}

			lit.m_Sign = true;
			radix      = 2;
			offset += 2;
			break;
		case Tokenizer::ETokenClass::OctalInteger:
			if (str[0] != '0' ||
			    (str[1] != 'o' && str[1] != 'O'))
			{
				// reportError(token.m_Index,
				//             token.m_Index,
				//	           { token.getSpan(m_Sources) },
				//	           token.getStart(m_Sources),
				//	           "Expected octal literal to start with '0o' or '0O'");
				return { 0, IntegerLiteral::ESize::None };
			}

			lit.m_Sign = true;
			radix      = 8;
			offset += 2;
			break;
		case Tokenizer::ETokenClass::HexInteger:
			if (str[0] != '0' ||
			    (str[1] != 'x' && str[1] != 'X'))
			{
				// reportError(token.m_Index,
				//             token.m_Index,
				//	           { token.getSpan(m_Sources) },
				//	           token.getStart(m_Sources),
				//	           "Expected hex literal to start with '0x' or '0X'");
				return { 0, IntegerLiteral::ESize::None };
			}

			lit.m_Sign = true;
			radix      = 16;
			offset += 2;
			break;
		default:
			break;
		}

		while (end > offset && std::isdigit(str[end - 1]))
		{
			switch (str[end - 1])
			{
			case 'u':
			case 'U':
				if (lit.m_Sign)
				{
					// reportError(token.m_Index
					//			   token.m_Index
					//			   { token.getSpan(m_Sources) },
					//			   token.getStart(m_Sources),
					//             "Integer literal should not contain 'u' or 'U' multiple times");
					return { 0, IntegerLiteral::ESize::None };
				}

				lit.m_Sign = true;
				break;
			case 's':
			case 'S':
				if (lit.m_Size != IntegerLiteral::ESize::Int)
				{
					// reportError(token.m_Index
					//			   token.m_Index
					//			   { token.getSpan(m_Sources) },
					//			   token.getStart(m_Sources),
					//             "Integer literal should not contain sized suffixes multiple times");
					return { 0, IntegerLiteral::ESize::None };
				}

				lit.m_Size = IntegerLiteral::ESize::Short;
				break;
			case 'l':
			case 'L':
				if (lit.m_Size != IntegerLiteral::ESize::Int)
				{
					// reportError(token.m_Index
					//			   token.m_Index
					//			   { token.getSpan(m_Sources) },
					//			   token.getStart(m_Sources),
					//             "Integer literal should not contain sized suffixes multiple times");
					return { 0, IntegerLiteral::ESize::None };
				}

				lit.m_Size = IntegerLiteral::ESize::Long;
				break;
			default:
				// reportError(token.m_Index
				//			   token.m_Index
				//			   { token.getSpan(m_Sources) },
				//			   token.getStart(m_Sources),
				//             "Integer literal suffix unrecognized");
				break;
			}

			--end;
		}

		switch (lit.m_Size)
		{
		case IntegerLiteral::ESize::Byte:
			switch (radix)
			{
			case 2:
				maxDigits = 8;
				break;
			case 8:
				maxDigits = 3;
				break;
			case 10:
				maxDigits = 3;
				break;
			case 16:
				maxDigits = 2;
				break;
			}
			break;
		case IntegerLiteral::ESize::Short:
			switch (radix)
			{
			case 2:
				maxDigits = 16;
				break;
			case 8:
				maxDigits = 6;
				break;
			case 10:
				maxDigits = 5;
				break;
			case 16:
				maxDigits = 4;
				break;
			}
			break;
		case IntegerLiteral::ESize::Int:
			switch (radix)
			{
			case 2:
				maxDigits = 32;
				break;
			case 8:
				maxDigits = 11;
				break;
			case 10:
				maxDigits = 10;
				break;
			case 16:
				maxDigits = 8;
				break;
			}
			break;
		case IntegerLiteral::ESize::Long:
			switch (radix)
			{
			case 2:
				maxDigits = 64;
				break;
			case 8:
				maxDigits = 22;
				break;
			case 10:
				maxDigits = negative ? 19 : 20;
				break;
			case 16:
				maxDigits = 16;
				break;
			}
			break;
		}

		if (end - offset > maxDigits)
		{
			// reportError(token.m_Index
			//			   token.m_Index
			//			   { token.getSpan(m_Sources) },
			//			   token.getStart(m_Sources),
			//             "Integer literal contains too many digits");
			return { 0, IntegerLiteral::ESize::None };
		}

		for (; offset < end; ++offset)
		{
			lit.m_Value *= radix;
			std::uint8_t d = 0;
			char         c = str[offset];
			if (c >= '0' && c <= '9')
			{
				d = c - '0';
			}
			else if (c >= 'A' && c <= 'F')
			{
				d = c - 'A' + 10;
			}
			else if (c >= 'a' && c <= 'f')
			{
				d = c - 'a' + 10;
			}
			else
			{
				// reportError(token.m_Index
				//			   token.m_Index
				//			   { token.getSpan(m_Sources) },
				//			   token.getStart(m_Sources),
				//             "Integer literal contains unrecognized digit");
				return { 0, IntegerLiteral::ESize::None };
			}

			switch (radix)
			{
			case 2:
				if (d > 1)
				{
					// reportError(token.m_Index
					//			   token.m_Index
					//			   { token.getSpan(m_Sources) },
					//			   token.getStart(m_Sources),
					//             "Integer literal contains digit outside current radix 2");
					return { 0, IntegerLiteral::ESize::None };
				}
				break;
			case 8:
				if (d > 7)
				{
					// reportError(token.m_Index
					//			   token.m_Index
					//			   { token.getSpan(m_Sources) },
					//			   token.getStart(m_Sources),
					//             "Integer literal contains digit outside current radix 8");
					return { 0, IntegerLiteral::ESize::None };
				}
				break;
			case 10:
				if (d > 9)
				{
					// reportError(token.m_Index
					//			   token.m_Index
					//			   { token.getSpan(m_Sources) },
					//			   token.getStart(m_Sources),
					//             "Integer literal contains digit outside current radix 10");
					return { 0, IntegerLiteral::ESize::None };
				}
				break;
			case 16:
				if (d > 15)
				{
					// reportError(token.m_Index
					//			   token.m_Index
					//			   { token.getSpan(m_Sources) },
					//			   token.getStart(m_Sources),
					//             "Integer literal contains digit outside current radix 16");
					return { 0, IntegerLiteral::ESize::None };
				}
				break;
			}
			lit.m_Value += d;
		}

		if (negative)
			lit.m_Value = ~lit.m_Value + 1;

		return lit;
	}

	FloatLiteral ParseFloatLiteral(AST::Node* node, Source::Sources* sources)
	{
		PROFILE_FUNC;

		return { 0.0, FloatLiteral::ESize::None };
	}

	BoolLiteral ParseBoolLiteral(AST::Node* node, Source::Sources* sources)
	{
		PROFILE_FUNC;

		auto str = node->getToken().getView(sources);
		if (str == "true")
			return { true, true };
		else if (str == "false")
			return { false, true };

		return { false, false };
	}
} // namespace Frertex::Parser
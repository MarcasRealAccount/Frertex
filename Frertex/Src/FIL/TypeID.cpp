#include "Frertex/FIL/TypeID.h"

namespace Frertex::FIL
{
	ETypeID StringToBuiltinTypeID(std::string_view str)
	{
	}

	std::string TypeIDToString(ETypeID type)
	{
		if (type == ETypeID::Void)
			return "void";

		if (static_cast<std::uint64_t>(type) < 0x8000'0000)
		{
			std::uint64_t rows    = ((static_cast<std::uint64_t>(type) >> 20) & 3) + 1;
			std::uint64_t columns = ((static_cast<std::uint64_t>(type) >> 22) & 3) + 1;

			std::string   str;
			std::uint64_t baseTypeID = static_cast<std::uint64_t>(type);

			ETypeID baseType = static_cast<ETypeID>((baseTypeID & 0x7'0000) | ((baseTypeID & 0xFFFF) / (rows * columns)));
			switch (baseType)
			{
			case ETypeID::Bool:
				str = "bool";
				break;
			case ETypeID::Byte:
				str = "byte";
				break;
			case ETypeID::UByte:
				str = "ubyte";
				break;
			case ETypeID::Short:
				str = "short";
				break;
			case ETypeID::UShort:
				str = "ushort";
				break;
			case ETypeID::Int:
				str = "int";
				break;
			case ETypeID::UInt:
				str = "uint";
				break;
			case ETypeID::Long:
				str = "long";
				break;
			case ETypeID::ULong:
				str = "ulong";
				break;
			case ETypeID::Half:
				str = "half";
				break;
			case ETypeID::Float:
				str = "float";
				break;
			case ETypeID::Double:
				str = "double";
				break;
			default:
				str = "void";
				break;
			}

			if (rows != 1 || columns != 1)
				str += std::to_string(rows);
			if (columns != 1)
				str += "x" + std::to_string(columns);
			return str;
		}
		else if (static_cast<std::uint64_t>(type) < 0x8000'0000'0000'0000)
		{
			switch (type)
			{
			case ETypeID::BuiltinPosition: return "BuiltinPosition";
			case ETypeID::BuiltinPointSize: return "BuiltinPointSize";
			default: return {};
			}
		}
		else
		{
			return {};
		}
	}

	bool TypeIDIsBuiltin(ETypeID type)
	{
		return static_cast<std::uint64_t>(type) >= 0x8000'0000'0000'0000;
	}

	bool TypeIDIsUserDefined(ETypeID type)
	{
		return static_cast<std::uint64_t>(type) >= 0x8000'0000 &&
		       static_cast<std::uint64_t>(type) < 0x8000'0000'0000'0000;
	}

	ETypeID TypeIDGetBase(ETypeID type)
	{
		std::uint64_t typeID = static_cast<std::uint64_t>(type);
		if (typeID >= 0x8000'0000)
			return type;

		std::uint64_t rows    = (typeID >> 20) & 0b11;
		std::uint64_t columns = (typeID >> 22) & 0b11;
		if (!columns)
		{
			if (!rows)
				return type;
			else
				return static_cast<ETypeID>(((typeID & 0xFFFF) / (rows + 1)) | (typeID & ~0xF0'FFFF));
		}
		else
		{
			if (!rows)
				return static_cast<ETypeID>(((typeID & 0xFFFF) / (columns + 1)) | (typeID & ~0xF0'FFFF));
			else
				return static_cast<ETypeID>(((typeID & 0xFFFF) / (columns + 1)) | (typeID & ~0xC0'FFFF));
		}
	}
} // namespace Frertex::FIL
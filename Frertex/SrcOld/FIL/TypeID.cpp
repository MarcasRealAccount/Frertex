#include "Frertex/FIL/TypeID.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::FIL
{
	ETypeID StringToBuiltinTypeID(std::string_view str)
	{
		PROFILE_FUNC;

		if (str == "Position")
			return ETypeID::BuiltinPosition;
		else if (str == "PointSize")
			return ETypeID::BuiltinPointSize;
		else if (str == "ClipDistance")
			return ETypeID::BuiltinClipDistance;
		else if (str == "CullDistance")
			return ETypeID::BuiltinCullDistance;
		else if (str == "VertexIndex")
			return ETypeID::BuiltinVertexIndex;
		else if (str == "InstanceIndex")
			return ETypeID::BuiltinInstanceIndex;
		else
			return ETypeID::Void;
	}

	ETypeID StringToTypeID(std::string_view str)
	{
		PROFILE_FUNC;

		if (str == "void")
			return ETypeID::Void;

		ETypeID     baseType = ETypeID::Void;
		std::size_t offset   = 0;

		if (str.starts_with("bool"))
		{
			baseType = ETypeID::Bool;
			offset   = 4;
		}
		else if (str.starts_with("byte"))
		{
			baseType = ETypeID::Byte;
			offset   = 4;
		}
		else if (str.starts_with("ubyte"))
		{
			baseType = ETypeID::UByte;
			offset   = 5;
		}
		else if (str.starts_with("short"))
		{
			baseType = ETypeID::Short;
			offset   = 5;
		}
		else if (str.starts_with("ushort"))
		{
			baseType = ETypeID::UShort;
			offset   = 6;
		}
		else if (str.starts_with("int"))
		{
			baseType = ETypeID::Int;
			offset   = 3;
		}
		else if (str.starts_with("uint"))
		{
			baseType = ETypeID::UInt;
			offset   = 4;
		}
		else if (str.starts_with("long"))
		{
			baseType = ETypeID::Long;
			offset   = 4;
		}
		else if (str.starts_with("ulong"))
		{
			baseType = ETypeID::ULong;
			offset   = 5;
		}
		else if (str.starts_with("half"))
		{
			baseType = ETypeID::Half;
			offset   = 3;
		}
		else if (str.starts_with("float"))
		{
			baseType = ETypeID::Float;
			offset   = 5;
		}
		else if (str.starts_with("double"))
		{
			baseType = ETypeID::Double;
			offset   = 6;
		}
		else
		{
			return ETypeID::UserDefinedBegin;
		}

		std::uint64_t rows = 0, columns = 0;
		if (str.size() - offset > 0 && str[offset] >= '1' && str[offset] <= '4')
		{
			rows = str[offset] - '1';
			++offset;
			if (str.size() - offset > 1 && str[offset] == 'x' && str[offset + 1] >= '1' && str[offset + 1] <= '4')
			{
				columns = str[offset + 1] - '1';
				offset += 2;
			}
		}

		std::uint64_t baseTypeID = static_cast<std::uint64_t>(baseType);

		return static_cast<ETypeID>((baseTypeID & ~0xFFFF) | (baseTypeID & 0xFFFF) * (rows + 1) * (columns + 1) | (rows << 20) | (columns << 22));
	}

	std::string TypeIDToString(ETypeID type)
	{
		PROFILE_FUNC;

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
		PROFILE_FUNC;

		return static_cast<std::uint64_t>(type) >= 0x8000'0000'0000'0000;
	}

	bool TypeIDIsUserDefined(ETypeID type)
	{
		PROFILE_FUNC;

		return static_cast<std::uint64_t>(type) >= 0x8000'0000 &&
		       static_cast<std::uint64_t>(type) < 0x8000'0000'0000'0000;
	}

	ETypeID TypeIDGetBase(ETypeID type)
	{
		PROFILE_FUNC;

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

	bool TypeIDIsCorrectForBuiltin(ETypeID type, ETypeID builtin)
	{
		PROFILE_FUNC;

		switch (builtin)
		{
		case ETypeID::BuiltinPosition: return type == ETypeID::Float4;
		case ETypeID::BuiltinPointSize: return type == ETypeID::Float;
		case ETypeID::BuiltinClipDistance: return type == ETypeID::Float;
		case ETypeID::BuiltinCullDistance: return type == ETypeID::Float;
		case ETypeID::BuiltinVertexIndex: return type == ETypeID::UInt;
		case ETypeID::BuiltinInstanceIndex: return type == ETypeID::UInt;
		default: return false;
		}
	}

	ETypeQualifier GetBuiltinTypeQualifier(ETypeID builtin)
	{
		PROFILE_FUNC;

		switch (builtin)
		{
		case ETypeID::BuiltinPosition: return ETypeQualifier::Out;
		case ETypeID::BuiltinPointSize: return ETypeQualifier::Out;
		case ETypeID::BuiltinClipDistance: return ETypeQualifier::Out;
		case ETypeID::BuiltinCullDistance: return ETypeQualifier::Out;
		case ETypeID::BuiltinVertexIndex: return ETypeQualifier::In;
		case ETypeID::BuiltinInstanceIndex: return ETypeQualifier::In;
		default: return ETypeQualifier::None;
		}
	}
} // namespace Frertex::FIL
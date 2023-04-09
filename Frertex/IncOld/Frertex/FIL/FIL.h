#pragma once

#include "EntrypointType.h"
#include "Frertex/Utils/Buffer.h"
#include "Frertex/Utils/Utils.h"
#include "TypeID.h"
#include "TypeQualifier.h"

#include <filesystem>

namespace Frertex::FIL
{
	struct EntrypointParameter
	{
	public:
		EntrypointParameter()
		    : m_Location(0), m_TypeID(0) {}

		EntrypointParameter(std::uint64_t location, std::uint64_t typeID)
		    : m_Location(location), m_TypeID(typeID) {}

	public:
		std::uint64_t m_Location;
		std::uint64_t m_TypeID;
	};

	struct Entrypoint
	{
	public:
		Entrypoint()
		    : m_Type(EEntrypointType::None), m_CodeOffset(0), m_CodeLength(0), m_NameOffset(0), m_NameLength(0) {}

		Entrypoint(EEntrypointType type, std::uint64_t codeOffset, std::uint64_t codeLength, std::uint64_t nameOffset, std::uint64_t nameLength, Utils::CopyMovable<std::vector<EntrypointParameter>>&& inputs, Utils::CopyMovable<std::vector<EntrypointParameter>>&& outputs)
		    : m_Type(type), m_CodeOffset(codeOffset), m_CodeLength(codeLength), m_NameOffset(nameOffset), m_NameLength(nameLength), m_Inputs(inputs.get()), m_Outputs(outputs.get()) {}

	public:
		EEntrypointType m_Type;
		std::uint64_t   m_CodeOffset, m_CodeLength;
		std::uint64_t   m_NameOffset, m_NameLength;

		std::vector<EntrypointParameter> m_Inputs;
		std::vector<EntrypointParameter> m_Outputs;
	};

	struct FunctionParameter
	{
	public:
		FunctionParameter()
		    : m_TypeID(0) {}

		FunctionParameter(Utils::CopyMovable<std::vector<ETypeQualifier>>&& qualifiers, std::uint64_t typeID)
		    : m_Qualifiers(qualifiers.get()), m_TypeID(typeID) {}

	public:
		std::vector<ETypeQualifier> m_Qualifiers;

		std::uint64_t m_TypeID;
	};

	struct Function
	{
	public:
		Function()
		    : m_CodeOffset(0), m_CodeLength(0), m_NameOffset(0), m_NameLength(0), m_ReturnTypeID(0) {}

		Function(std::uint64_t codeOffset, std::uint64_t codeLength, std::uint64_t nameOffset, std::uint64_t nameLength, std::uint64_t returnTypeID, Utils::CopyMovable<std::vector<FunctionParameter>>&& parameters)
		    : m_CodeOffset(codeOffset), m_CodeLength(codeLength), m_NameOffset(nameOffset), m_NameLength(nameLength), m_ReturnTypeID(returnTypeID), m_Parameters(parameters.get()) {}

	public:
		std::uint64_t m_CodeOffset, m_CodeLength;
		std::uint64_t m_NameOffset, m_NameLength;

		std::uint64_t                  m_ReturnTypeID;
		std::vector<FunctionParameter> m_Parameters;
	};

	struct Type
	{
	public: // TODO(MarcasRealAccount): Implement FIL Types
	};

	struct Binary
	{
	public:
		const Entrypoint* getEntrypoint(std::uint64_t id) const;
		const Function*   getFunction(std::uint64_t id) const;
		std::string_view  getFunctionName(const Function* function) const;
		const Type*       getType(std::uint64_t id) const;

		std::vector<std::uint8_t> toBinary() const;

		void fromBinary(std::vector<std::uint8_t>& rawBinary);

	private:
		void fromBinary1(Utils::Buffer& binary);

	public:
		std::vector<Entrypoint>    m_Entrypoints;
		std::vector<Function>      m_Functions;
		std::vector<std::uint8_t>  m_Strings;
		std::vector<Type>          m_Types;
		std::vector<std::uint32_t> m_Code;
	};

	void WriteToFile(const Binary& binary, const std::filesystem::path& filepath);
	void ReadFromFile(Binary& binary, const std::filesystem::path& filepath);
} // namespace Frertex::FIL
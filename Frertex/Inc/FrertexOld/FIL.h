#pragma once

#include "Utils/Utils.h"

#include <cstdint>

#include <filesystem>
#include <string>
#include <vector>

namespace Frertex
{
	namespace FIL
	{
		enum class EEntrypointType : std::uint32_t
		{
			None                         = 0x0000,
			VertexShader                 = 0x0001,
			TessellationControlShader    = 0x0002,
			TessellationEvaluationShader = 0x0004,
			GeometryShader               = 0x0008,
			FragmentShader               = 0x0010,
			ComputeShader                = 0x0020,
			RTRayGenShader               = 0x0100,
			RTAnyHitShader               = 0x0200,
			RTClosestHitShader           = 0x0400,
			RTMissShader                 = 0x0800,
			RTIntersectionShader         = 0x1000,
			RTCallableShader             = 0x2000,
			NVTaskShader                 = 0x0040,
			NVMeshShader                 = 0x0080
		};

		std::string_view EntrypointTypeToString(EEntrypointType type);

		enum class ETypeQualifier : std::uint32_t
		{
			None = 0,
			In,
			Out,
			InOut
		};

		// w___'____'____'____'____'____'____'____'z___'____'ccrr'_bfu'ssss'ssss'ssss'ssss
		// s = Size (bytes)
		// u = Unsigned flag
		// f = Floating point flag
		// b = Boolean flag
		// r = Vector or Matrix Rows minus one
		// c = Vector or Matrix Columns minus one
		// z = Builtin type ID flag
		// w = User defined type ID flag

		enum class ETypeIDs : std::uint64_t
		{
			Void   = 0,
			Bool   = 0x0004'0001,
			Byte   = 0x0000'0001,
			UByte  = 0x0001'0001,
			Short  = 0x0000'0002,
			UShort = 0x0001'0002,
			Int    = 0x0000'0004,
			UInt   = 0x0001'0004,
			Long   = 0x0000'0008,
			ULong  = 0x0001'0008,
			Half   = 0x0002'0002,
			Float  = 0x0002'0004,
			Double = 0x0002'0008,

			Bool2   = 0x0014'0002,
			Bool3   = 0x0024'0003,
			Bool4   = 0x0034'0004,
			Byte2   = 0x0010'0002,
			Byte3   = 0x0020'0003,
			Byte4   = 0x0030'0004,
			UByte2  = 0x0011'0002,
			UByte3  = 0x0021'0003,
			UByte4  = 0x0031'0004,
			Short2  = 0x0010'0004,
			Short3  = 0x0020'0006,
			Short4  = 0x0030'0008,
			UShort2 = 0x0011'0004,
			UShort3 = 0x0021'0006,
			UShort4 = 0x0031'0008,
			Int2    = 0x0010'0008,
			Int3    = 0x0020'000C,
			Int4    = 0x0030'0010,
			UInt2   = 0x0011'0008,
			UInt3   = 0x0021'000C,
			UInt4   = 0x0031'0010,
			Long2   = 0x0010'0010,
			Long3   = 0x0020'0018,
			Long4   = 0x0030'0020,
			ULong2  = 0x0011'0010,
			ULong3  = 0x0021'0018,
			ULong4  = 0x0031'0020,
			Half2   = 0x0012'0004,
			Half3   = 0x0022'0006,
			Half4   = 0x0032'0008,
			Float2  = 0x0012'0008,
			Float3  = 0x0022'000C,
			Float4  = 0x0032'0010,
			Double2 = 0x0012'0010,
			Double3 = 0x0022'0018,
			Double4 = 0x0032'0020,

			Bool1x2 = 0x0044'0002,
			Bool1x3 = 0x0084'0003,
			Bool1x4 = 0x00C4'0004,
			Bool2x2 = 0x0054'0004,
			Bool2x3 = 0x0094'0006,
			Bool2x4 = 0x00D4'0008,
			Bool3x2 = 0x0064'0006,
			Bool3x3 = 0x00A4'0009,
			Bool3x4 = 0x00E4'000C,
			Bool4x2 = 0x0074'0008,
			Bool4x3 = 0x00B4'000C,
			Bool4x4 = 0x00F4'0010,

			Byte1x2 = 0x0040'0002,
			Byte1x3 = 0x0080'0003,
			Byte1x4 = 0x00C0'0004,
			Byte2x2 = 0x0050'0004,
			Byte2x3 = 0x0090'0006,
			Byte2x4 = 0x00D0'0008,
			Byte3x2 = 0x0060'0006,
			Byte3x3 = 0x00A0'0009,
			Byte3x4 = 0x00E0'000C,
			Byte4x2 = 0x0070'0008,
			Byte4x3 = 0x00B0'000C,
			Byte4x4 = 0x00F0'0010,

			UByte1x2 = 0x0041'0002,
			UByte1x3 = 0x0081'0003,
			UByte1x4 = 0x00C1'0004,
			UByte2x2 = 0x0051'0004,
			UByte2x3 = 0x0091'0006,
			UByte2x4 = 0x00D1'0008,
			UByte3x2 = 0x0061'0006,
			UByte3x3 = 0x00A1'0009,
			UByte3x4 = 0x00E1'000C,
			UByte4x2 = 0x0071'0008,
			UByte4x3 = 0x00B1'000C,
			UByte4x4 = 0x00F1'0010,

			Short1x2 = 0x0040'0004,
			Short1x3 = 0x0080'0006,
			Short1x4 = 0x00C0'0008,
			Short2x2 = 0x0050'0008,
			Short2x3 = 0x0090'000C,
			Short2x4 = 0x00D0'0010,
			Short3x2 = 0x0060'000C,
			Short3x3 = 0x00A0'0012,
			Short3x4 = 0x00E0'0018,
			Short4x2 = 0x0070'0010,
			Short4x3 = 0x00B0'0018,
			Short4x4 = 0x00F0'0020,

			UShort1x2 = 0x0041'0004,
			UShort1x3 = 0x0081'0006,
			UShort1x4 = 0x00C1'0008,
			UShort2x2 = 0x0051'0008,
			UShort2x3 = 0x0091'000C,
			UShort2x4 = 0x00D1'0010,
			UShort3x2 = 0x0061'000C,
			UShort3x3 = 0x00A1'0012,
			UShort3x4 = 0x00E1'0018,
			UShort4x2 = 0x0071'0010,
			UShort4x3 = 0x00B1'0018,
			UShort4x4 = 0x00F1'0020,

			Int1x2 = 0x0040'0008,
			Int1x3 = 0x0080'000C,
			Int1x4 = 0x00C0'0010,
			Int2x2 = 0x0050'0010,
			Int2x3 = 0x0090'0018,
			Int2x4 = 0x00D0'0020,
			Int3x2 = 0x0060'0018,
			Int3x3 = 0x00A0'0024,
			Int3x4 = 0x00E0'0030,
			Int4x2 = 0x0070'0020,
			Int4x3 = 0x00B0'0030,
			Int4x4 = 0x00F0'0040,

			UInt1x2 = 0x0041'0008,
			UInt1x3 = 0x0081'000C,
			UInt1x4 = 0x00C1'0010,
			UInt2x2 = 0x0051'0010,
			UInt2x3 = 0x0091'0018,
			UInt2x4 = 0x00D1'0020,
			UInt3x2 = 0x0061'0018,
			UInt3x3 = 0x00A1'0024,
			UInt3x4 = 0x00E1'0030,
			UInt4x2 = 0x0071'0020,
			UInt4x3 = 0x00B1'0030,
			UInt4x4 = 0x00F1'0040,

			Long1x2 = 0x0040'0010,
			Long1x3 = 0x0080'0018,
			Long1x4 = 0x00C0'0020,
			Long2x2 = 0x0050'0020,
			Long2x3 = 0x0090'0030,
			Long2x4 = 0x00D0'0040,
			Long3x2 = 0x0060'0030,
			Long3x3 = 0x00A0'0048,
			Long3x4 = 0x00E0'0060,
			Long4x2 = 0x0070'0040,
			Long4x3 = 0x00B0'0060,
			Long4x4 = 0x00F0'0080,

			ULong1x2 = 0x0041'0010,
			ULong1x3 = 0x0081'0018,
			ULong1x4 = 0x00C1'0020,
			ULong2x2 = 0x0051'0020,
			ULong2x3 = 0x0091'0030,
			ULong2x4 = 0x00D1'0040,
			ULong3x2 = 0x0061'0030,
			ULong3x3 = 0x00A1'0048,
			ULong3x4 = 0x00E1'0060,
			ULong4x2 = 0x0071'0040,
			ULong4x3 = 0x00B1'0060,
			ULong4x4 = 0x00F1'0080,

			Half1x2 = 0x0042'0004,
			Half1x3 = 0x0082'0006,
			Half1x4 = 0x00C2'0008,
			Half2x2 = 0x0052'0008,
			Half2x3 = 0x0092'000C,
			Half2x4 = 0x00D2'0010,
			Half3x2 = 0x0062'000C,
			Half3x3 = 0x00A2'0012,
			Half3x4 = 0x00E2'0018,
			Half4x2 = 0x0072'0010,
			Half4x3 = 0x00B2'0018,
			Half4x4 = 0x00F2'0020,

			Float1x2 = 0x0042'0008,
			Float1x3 = 0x0082'000C,
			Float1x4 = 0x00C2'0010,
			Float2x2 = 0x0052'0010,
			Float2x3 = 0x0092'0018,
			Float2x4 = 0x00D2'0020,
			Float3x2 = 0x0062'0018,
			Float3x3 = 0x00A2'0024,
			Float3x4 = 0x00E2'0030,
			Float4x2 = 0x0072'0020,
			Float4x3 = 0x00B2'0030,
			Float4x4 = 0x00F2'0040,

			Double1x2 = 0x0042'0010,
			Double1x3 = 0x0082'0018,
			Double1x4 = 0x00C2'0020,
			Double2x2 = 0x0052'0020,
			Double2x3 = 0x0092'0030,
			Double2x4 = 0x00D2'0040,
			Double3x2 = 0x0062'0030,
			Double3x3 = 0x00A2'0048,
			Double3x4 = 0x00E2'0060,
			Double4x2 = 0x0072'0040,
			Double4x3 = 0x00B2'0060,
			Double4x4 = 0x00F2'0080,

			BuiltinBegin    = 0x8000'0000,
			BuiltinPosition = 0x8000'0000,
			BuiltinPointSize,

			UserDefinedBegin = 0x8000'0000'0000'0000
		};

		std::string TypeIDToString(ETypeIDs type);
		bool        TypeIDIsBuiltIn(ETypeIDs type);
		bool        TypeIDIsUserDefined(ETypeIDs type);
		ETypeIDs    TypeIDGetBase(ETypeIDs type);

		struct Type
		{
		public: // TODO(MarcasRealAccount): Implement
		};

		struct EntrypointParameter
		{
		public:
			EntrypointParameter(std::uint64_t location, std::uint64_t typeID)
			    : m_Location(location), m_TypeID(typeID) {}

		public:
			std::uint64_t m_Location;
			std::uint64_t m_TypeID;
		};

		struct Entrypoint
		{
		public:
			Entrypoint(EEntrypointType type, std::uint64_t functionID, Utils::CopyMovable<std::vector<EntrypointParameter>>&& inputs, Utils::CopyMovable<std::vector<EntrypointParameter>>&& outputs)
			    : m_Type(type), m_FunctionID(functionID), m_Inputs(inputs.get()), m_Outputs(outputs.get()) {}

		public:
			EEntrypointType m_Type;
			std::uint64_t   m_FunctionID;

			std::vector<EntrypointParameter> m_Inputs;
			std::vector<EntrypointParameter> m_Outputs;
		};

		struct FunctionParameter
		{
		public:
			std::vector<ETypeQualifier> m_Qualifiers;

			std::uint64_t m_TypeID;
		};

		struct Function
		{
		public:
			Function(std::uint64_t codeOffset, std::uint64_t codeLength, std::uint64_t nameOffset, std::uint64_t nameLength, std::uint64_t returnTypeID, Utils::CopyMovable<std::vector<FunctionParameter>>&& parameters)
			    : m_CodeOffset(codeOffset), m_CodeLength(codeLength), m_ReturnTypeID(returnTypeID), m_Parameters(parameters.get()) {}

		public:
			std::uint64_t m_CodeOffset, m_CodeLength;
			std::uint64_t m_NameOffset, m_NameLength;

			std::uint64_t                  m_ReturnTypeID;
			std::vector<FunctionParameter> m_Parameters;
		};

		struct Binary
		{
		public:
			const Entrypoint* getEntrypoint(std::uint64_t id) const;
			const Function*   getFunction(std::uint64_t id) const;
			const Function*   getFunction(const Entrypoint* entrypoint) const;
			std::string_view  getFunctionName(const Function* function) const;
			const Type*       getType(std::uint64_t id) const;
			std::string_view  getTypename(const Type* type) const;

			std::vector<std::uint8_t> toBinary() const;

			// TODO(MarcasRealAccount): Implement
			// void fromBinary(const std::vector<std::uint8_t>& binary);

		public:
			std::vector<Entrypoint>    m_Entrypoints;
			std::vector<Function>      m_Functions;
			std::vector<std::uint8_t>  m_Strings;
			std::vector<Type>          m_Types;
			std::vector<std::uint32_t> m_Code;
		};

		struct CodeBuffer
		{
		public:
			void pushNOP();
			void pushRet();

			std::vector<std::uint32_t>&& getCode() { return std::move(m_Code); }

		private:
			std::vector<std::uint32_t> m_Code;
		};
	} // namespace FIL
} // namespace Frertex
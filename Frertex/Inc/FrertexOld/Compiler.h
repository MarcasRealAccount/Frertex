#pragma once

#include "AST.h"
#include "FIL.h"
#include "Message.h"
#include "Utils/IEEE754.h"
#include "Utils/Utils.h"

#include <stack>

namespace Frertex::Compiler
{
	namespace Literal
	{
		enum class EIntegerSize
		{
			Byte,
			Short,
			Int,
			Long
		};

		struct Integer
		{
		public:
			Integer(std::uint64_t value, EIntegerSize size, bool sign = true) : m_Value(value), m_Size(size), m_Sign(sign) {}

			std::uint8_t  getUByte() const { return static_cast<std::uint8_t>(m_Value); }
			std::uint16_t getUShort() const { return static_cast<std::uint16_t>(m_Value); }
			std::uint32_t getUInt() const { return static_cast<std::uint32_t>(m_Value); }
			std::uint64_t getULong() const { return static_cast<std::uint64_t>(m_Value); }
			std::int8_t   getByte() const { return static_cast<std::int8_t>(m_Value); }
			std::int16_t  getShort() const { return static_cast<std::int16_t>(m_Value); }
			std::int32_t  getInt() const { return static_cast<std::int32_t>(m_Value); }
			std::int64_t  getLong() const { return static_cast<std::int64_t>(m_Value); }

		public:
			std::uint64_t m_Value;
			EIntegerSize  m_Size;
			bool          m_Sign;
		};

		enum class EFloatSize
		{
			Half,
			Float,
			Double
		};

		struct Float
		{
		public:
			Float(std::uint64_t value, EFloatSize size) : m_Size(size), m_Value(value) {}

			Utils::IEEE754::Half getHalf() const
			{
				switch (m_Size)
				{
				case EFloatSize::Half: return std::bit_cast<Utils::IEEE754::Half>(static_cast<std::uint16_t>(m_Value));
				case EFloatSize::Float: return static_cast<Utils::IEEE754::Half>(std::bit_cast<float>(static_cast<std::uint32_t>(m_Value)));
				case EFloatSize::Double: return static_cast<Utils::IEEE754::Half>(std::bit_cast<double>(static_cast<std::uint64_t>(m_Value)));
				}
				return 0.0;
			}

			float getFloat() const
			{
				switch (m_Size)
				{
				case EFloatSize::Half: return static_cast<float>(std::bit_cast<Utils::IEEE754::Half>(static_cast<std::uint16_t>(m_Value)));
				case EFloatSize::Float: return std::bit_cast<float>(static_cast<std::uint32_t>(m_Value));
				case EFloatSize::Double: return static_cast<float>(std::bit_cast<double>(m_Value));
				}
				return 0.0f;
			}

			double getDouble() const
			{
				switch (m_Size)
				{
				case EFloatSize::Half: return static_cast<double>(std::bit_cast<Utils::IEEE754::Half>(static_cast<std::uint16_t>(m_Value)));
				case EFloatSize::Float: return static_cast<double>(std::bit_cast<float>(static_cast<std::uint32_t>(m_Value)));
				case EFloatSize::Double: return std::bit_cast<double>(m_Value);
				}
				return 0.0;
			}

		public:
			EFloatSize    m_Size;
			std::uint64_t m_Value;
		};
	} // namespace Literal

	struct FunctionDefinition
	{
	public:
		FunctionDefinition(Utils::CopyMovable<std::string>&& name, FIL::EEntrypointType type)
		    : m_Name(name.get()), m_Type(type) {}

		std::string          m_Name;
		FIL::EEntrypointType m_Type;
	};

	struct FunctionParameter
	{
	public:
		FunctionParameter(Utils::CopyMovable<std::vector<FIL::ETypeQualifier>>&& qualifiers, std::uint32_t builtinTypeID, Utils::CopyMovable<std::string>&& typeName, std::uint64_t location)
		    : m_Qualifiers(qualifiers.get()), m_BuiltinTypeID(builtinTypeID), m_Typename(typeName.get()), m_Location(location) {}

	public:
		std::vector<FIL::ETypeQualifier> m_Qualifiers;
		std::uint32_t                    m_BuiltinTypeID;
		std::string                      m_Typename;
		std::uint64_t                    m_Location;
	};

	struct FunctionDeclaration : FunctionDefinition
	{
	public:
		FunctionDeclaration(Utils::CopyMovable<std::string>&& name, FIL::EEntrypointType type, Utils::CopyMovable<std::vector<FunctionParameter>>&& parameters, Utils::CopyMovable<std::vector<std::uint32_t>>&& code)
		    : FunctionDefinition(name.get(), type), m_Parameters(parameters.get()), m_Code(code.get()) {}

	public:
		std::vector<FunctionParameter> m_Parameters;

		std::vector<std::uint32_t> m_Code;
	};

	struct TypeInfo
	{
	public:
	};

	struct VariableInfo
	{
	public:
		TypeInfo* m_Type;
	};

	struct StackFrame
	{
	public:
		std::unordered_map<std::string, TypeInfo>     m_Types;
		std::unordered_map<std::string, VariableInfo> m_Variables;

		std::uint64_t m_CurrentFunctionID;
		std::string   m_CurrentNamespace;
	};

	class State
	{
	public:
		State(Sources* sources);

		FIL::Binary compile(const AST::Result& ast);

	private:
		void getFunctionDefinitions();

		Literal::Integer getIntegerLiteral(AST::Node* node);
		Literal::Float   getFloatLiteral(AST::Node* node);
		bool             getBoolLiteral(AST::Node* node);

		void compileFunctionDeclaration(AST::Node* node);
		void compileAssignmentExpression(AST::Node* node);
		void compileExpression(AST::Node* node);
		void compileStatements(AST::Node* root);

		FIL::ETypeIDs getBuiltinTypeID(std::string_view type) const;

	private:
		Sources* m_Sources;

		std::vector<FunctionDefinition>  m_FunctionDefinitions;
		std::vector<FunctionDeclaration> m_FunctionDeclarations;

		std::stack<StackFrame> m_Stack;
	};
} // namespace Frertex::Compiler
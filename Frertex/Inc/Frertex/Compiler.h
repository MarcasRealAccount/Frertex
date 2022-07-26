#pragma once

#include "AST.h"
#include "FIL.h"
#include "Message.h"
#include "Utils/Utils.h"

namespace Frertex
{
	enum class EIntegerLiteralSize
	{
		Byte,
		Short,
		Int,
		Long
	};

	struct IntegerLiteral
	{
	public:
		IntegerLiteral(std::uint8_t byte, bool u = false)
		    : m_Size(EIntegerLiteralSize::Byte), m_Unsigned(u), m_Byte(byte) {}
		IntegerLiteral(std::uint16_t short_, bool u = false)
		    : m_Size(EIntegerLiteralSize::Short), m_Unsigned(u), m_Short(short_) {}
		IntegerLiteral(std::uint32_t int_, bool u = false)
		    : m_Size(EIntegerLiteralSize::Int), m_Unsigned(u), m_Int(int_) {}
		IntegerLiteral(std::uint64_t long_, bool u = false)
		    : m_Size(EIntegerLiteralSize::Long), m_Unsigned(u), m_Long(long_) {}

		std::uint8_t getUByte() const
		{
			switch (m_Size)
			{
			case EIntegerLiteralSize::Byte: return m_Byte;
			case EIntegerLiteralSize::Short: return static_cast<std::uint8_t>(m_Short);
			case EIntegerLiteralSize::Int: return static_cast<std::uint8_t>(m_Int);
			case EIntegerLiteralSize::Long: return static_cast<std::uint8_t>(m_Long);
			default: return 0U;
			}
		}

		std::uint16_t getUShort() const
		{
			switch (m_Size)
			{
			case EIntegerLiteralSize::Byte: return static_cast<std::uint16_t>(m_Byte);
			case EIntegerLiteralSize::Short: return m_Short;
			case EIntegerLiteralSize::Int: return static_cast<std::uint16_t>(m_Int);
			case EIntegerLiteralSize::Long: return static_cast<std::uint16_t>(m_Long);
			default: return 0U;
			}
		}

		std::uint32_t getUInt() const
		{
			switch (m_Size)
			{
			case EIntegerLiteralSize::Byte: return static_cast<std::uint32_t>(m_Byte);
			case EIntegerLiteralSize::Short: return static_cast<std::uint32_t>(m_Short);
			case EIntegerLiteralSize::Int: return m_Int;
			case EIntegerLiteralSize::Long: return static_cast<std::uint32_t>(m_Long);
			default: return 0U;
			}
		}

		std::uint64_t getULong() const
		{
			switch (m_Size)
			{
			case EIntegerLiteralSize::Byte: return static_cast<std::uint64_t>(m_Byte);
			case EIntegerLiteralSize::Short: return static_cast<std::uint64_t>(m_Short);
			case EIntegerLiteralSize::Int: return static_cast<std::uint64_t>(m_Int);
			case EIntegerLiteralSize::Long: return m_Long;
			default: return 0ULL;
			}
		}

		EIntegerLiteralSize m_Size;
		bool                m_Unsigned;
		union
		{
			std::uint8_t  m_Byte;
			std::uint16_t m_Short;
			std::uint32_t m_Int;
			std::uint64_t m_Long;
		};
	};

	enum class EFloatLiteralSize
	{
		Half,
		Float,
		Double
	};

	struct FloatLiteral
	{
	public:
		FloatLiteral(std::uint16_t half) : m_Size(EFloatLiteralSize::Half), m_Half(half) {}
		FloatLiteral(float float_) : m_Size(EFloatLiteralSize::Float), m_Float(float_) {}
		FloatLiteral(double double_) : m_Size(EFloatLiteralSize::Double), m_Double(double_) {}

		EFloatLiteralSize m_Size;
		union
		{
			std::uint16_t m_Half;
			float         m_Float;
			double        m_Double;
		};
	};

	class Compiler
	{
	public:
		Compiler(Sources* sources);

		FIL compile(Utils::CopyMovable<AST>&& ast);

		void addWarning(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message);
		void addError(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message);
		void addWarning(const Token& token, std::size_t point, Utils::CopyMovable<std::string>&& message)
		{
			addWarning(token.m_FileID, token.m_SourceID, token.m_Index, token.m_Length, point, message.get());
		}
		void addError(const Token& token, std::size_t point, Utils::CopyMovable<std::string>&& message)
		{
			addError(token.m_FileID, token.m_SourceID, token.m_Index, token.m_Length, point, message.get());
		}

		auto& getMessages() const { return m_Messages; }

	private:
		void getFunctionDefinitions(ASTNode& root, const std::string& prefix = "");

		IntegerLiteral getIntegerLiteral(ASTNode& node);
		FloatLiteral   getFloatLiteral(ASTNode& node);

		void compileFunctionDeclaration(ASTNode& node, const std::string& prefix = "");
		void compileAssignmentExpression(ASTNode& node, CodeBuffer& buffer);
		void compileExpression(ASTNode& node, CodeBuffer& buffer);
		void compileStatements(ASTNode& root, CodeBuffer& buffer, const std::string& prefix = "");

		ETypeIDs getBuiltinTypeID(std::string_view type) const;

	private:
		Sources* m_Sources;

		std::vector<Message> m_Messages;

		std::vector<FunctionDefinition>  m_FunctionDefinitions;
		std::vector<FunctionDeclaration> m_FunctionDeclarations;
	};
} // namespace Frertex
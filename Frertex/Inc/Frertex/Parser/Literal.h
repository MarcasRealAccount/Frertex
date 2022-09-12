#pragma once

#include "AST.h"
#include "Frertex/Math/Half.h"

#include <cstdint>

namespace Frertex::Parser
{
	struct IntegerLiteral
	{
	public:
		enum class ESize : std::uint8_t
		{
			None,
			Byte,
			Short,
			Int,
			Long
		};

	public:
		IntegerLiteral(std::uint64_t value, ESize size, bool sign = true)
		    : m_Value(value), m_Size(size), m_Sign(sign) {}

		std::int8_t   getByte() const { return static_cast<std::int8_t>(m_Value); }
		std::int16_t  getShort() const { return static_cast<std::int16_t>(m_Value); }
		std::int32_t  getInt() const { return static_cast<std::int32_t>(m_Value); }
		std::int64_t  getLong() const { return m_Value; }
		std::uint8_t  getUByte() const { return static_cast<std::uint8_t>(m_Value); }
		std::uint16_t getUShort() const { return static_cast<std::uint16_t>(m_Value); }
		std::uint32_t getUInt() const { return static_cast<std::uint32_t>(m_Value); }
		std::uint64_t getULong() const { return m_Value; }

	public:
		std::uint64_t m_Value;
		ESize         m_Size;
		bool          m_Sign;
	};

	struct FloatLiteral
	{
	public:
		enum class ESize : std::uint8_t
		{
			None,
			Half,
			Float,
			Double
		};

	public:
		FloatLiteral(double value, ESize size)
		    : m_Value(value), m_Size(size) {}

		Math::Half getHalf() const { return static_cast<Math::Half>(m_Value); }
		float      getFloat() const { return static_cast<float>(m_Value); }
		double     getDouble() const { return static_cast<double>(m_Value); }

	public:
		double m_Value;
		ESize  m_Size;
	};

	struct BoolLiteral
	{
	public:
		BoolLiteral(bool value, bool valid)
		    : m_Value(value), m_Valid(valid) {}

	public:
		bool m_Value;
		bool m_Valid;
	};

	IntegerLiteral ParseIntegerLiteral(AST::Node* node, Source::Sources* sources);
	FloatLiteral   ParseFloatLiteral(AST::Node* node, Source::Sources* sources);
	BoolLiteral    ParseBoolLiteral(AST::Node* node, Source::Sources* sources);
} // namespace Frertex::Parser
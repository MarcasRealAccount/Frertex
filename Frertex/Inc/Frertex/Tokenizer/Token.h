#pragma once

#include "Frertex/Source.h"

#include <cstddef>
#include <cstdint>

namespace Frertex::Tokenizer
{
	enum class ECharacterClass : std::uint32_t
	{
		Unknown = 0,
		Whitespace,
		Tab,
		Newline,
		NonDigit,
		Digit,
		Symbol
	};

	std::string_view CharacterClassToString(ECharacterClass clazz);

	enum class ETokenClass : std::uint32_t
	{
		Unknown = 0,
		Identifier,
		String,
		Integer,
		BinaryInteger,
		OctalInteger,
		HexInteger,
		Float,
		HexFloat,
		LastSymbol,
		Preprocessor,
		Comment,
		MultilineComment,

		FirstSymbol = LastSymbol | 0x8000'0000
	};

	constexpr ETokenClass GetBaseTokenClass(ETokenClass clazz) { return static_cast<ETokenClass>(static_cast<std::uint32_t>(clazz) & 0x7FFF'FFFF); }

	std::string_view TokenClassToString(ETokenClass clazz);

	struct ContextualToken
	{
	public:
		std::uint8_t m_State         = 0;
		ETokenClass  m_NewTokenClass = ETokenClass::Unknown;
	};

	struct Token
	{
	public:
		Token();
		Token(ETokenClass clazz, std::size_t index, std::size_t length, std::uint32_t fileID, std::uint32_t sourceID);

		Source::SourcePoint getStart(const Source::Sources* sources) const { return sources->getPoint(m_Index, m_SourceID); }
		Source::SourcePoint getEnd(const Source::Sources* sources) const { return sources->getPoint(m_Index + m_Length, m_SourceID); }
		Source::SourceSpan  getSpan(const Source::Sources* sources) const { return sources->getSpan(m_Index, m_Length, m_SourceID); }
		std::string_view    getView(const Source::Sources* sources) const { return sources->getView(m_Index, m_Length, m_SourceID); }
		std::string_view    getFilename(const Source::Sources* sources) const { return sources->getSourceName(m_FileID); }

	public:
		ETokenClass   m_Class;
		std::size_t   m_Index, m_Length;
		std::uint32_t m_FileID, m_SourceID;
	};

	struct ReconstructionData
	{
	public:
		struct Span
		{
		public:
			Span(std::size_t offset, std::size_t length, const Source::SourceSpan& sourceSpan)
			    : m_Offset(offset), m_Length(length), m_SourceSpan(sourceSpan) {}

		public:
			std::size_t        m_Offset, m_Length;
			Source::SourceSpan m_SourceSpan;
		};

	public:
		Span getEnvelopingSpan(const Source::SourceSpan& span);

	public:
		std::string       m_Str;
		std::vector<Span> m_Spans;
	};

	ReconstructionData ReconstructSource(const Source::Sources*    sources,
	                                     const std::vector<Token>& tokens,
	                                     std::size_t               start = 0ULL,
	                                     std::size_t               end   = ~0ULL);

	extern ECharacterClass s_CharacterClassLUT[128];
	extern ContextualToken s_ContextualTokenLUT[13][256];
} // namespace Frertex::Tokenizer
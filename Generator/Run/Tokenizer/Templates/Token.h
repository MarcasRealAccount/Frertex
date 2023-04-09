#pragma once

#include <cstdint>

#include <string_view>

namespace Frertex::Tokenizer
{
	enum class ECharacterClass : $TYPE$
	{
		$CHARCLASSES$
	};

	enum class ETokenClass : $TYPE$
	{
		$TOKENCLASSES$
	};

	std::string_view TokenClassToString(ETokenClass clazz);

	struct Token
	{
	public:
		ETokenClass   Class;
		std::uint32_t Length;
		std::uint64_t Start;
	};
} // namespace Frertex::Tokenizer
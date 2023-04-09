#include "TknzParser.h"

#include <UTF/UTF.h>

#include <cctype>

static std::size_t SkipWhitespaces(std::string_view source, std::size_t offset)
{
	while (offset < source.size() &&
		   std::isspace(source[offset]))
		++offset;
	return offset;
}

static std::size_t EndOfIdentifier(std::string_view source, std::size_t start)
{
	while (start < source.size())
	{
		if (!std::isalnum(source[start]) && source[start] != '_')
			break;
		++start;
	}
	return start;
}

static std::size_t EndOfInteger(std::string_view source, std::size_t start)
{
	while (start < source.size())
	{
		if (!std::isdigit(source[start]) && source[start] != '\'')
			break;
		++start;
	}
	return start;
}

static std::size_t EndOfClass(std::string_view source, std::size_t start)
{
	return source.find_first_of('}', start);
}

static std::size_t EndOfStatement(std::string_view source, std::size_t start)
{
	return source.find_first_of(';', start);
}

static std::size_t EndOfArgs(std::string_view source, std::size_t start)
{
	return source.find_first_of(')', start);
}

static std::size_t EndOfChar(std::string_view source, std::size_t start)
{
	bool escaped = false;
	while (start < source.size())
	{
		auto c = source[start];
		if (escaped)
			escaped = false;
		else if (c == '\\')
			escaped = true;
		else if (c == '\'')
			break;
		++start;
	}
	return start;
}

static std::uint64_t GetCharValue(std::string_view str)
{
	if (str.empty())
		return 0;

	switch (str[0])
	{
	case '\\':
		if (str.size() < 2)
			return 0;
		switch (str[1])
		{
		case 'a': return 7;
		case 'b': return 8;
		case 't': return 9;
		case 'n': return 10;
		case 'v': return 11;
		case 'f': return 12;
		case 'r': return 13;
		case 'u': return std::strtoull(str.data() + 2, nullptr, 16);
		default: return 0;
		}
	default:
		return static_cast<std::uint64_t>(str[0]);
	}
}

static std::uint64_t GetBaseStateValue(std::string_view str)
{
	if (str == "Step") return TknzResultStateStep;
	if (str == "End") return TknzResultStateEnd;
	if (str == "Error") return TknzResultStateError;
	return 0;
}

static std::string_view ASCIICharacterClasses[0x80] {};
static std::string_view UTFCharacterClasses[0x10'FFFF] {};

static class ClassesFiller
{
public:
	ClassesFiller()
	{
		// Fill ASCII character classes
		for (std::uint8_t i = 0; i < 0x80; ++i)
		{
			switch (i)
			{
			case '\t': ASCIICharacterClasses[i] = "Whitespace"; break;
			case '\n': ASCIICharacterClasses[i] = "Newline"; break;
			case '\v': ASCIICharacterClasses[i] = "Whitespace"; break;
			case '\f': ASCIICharacterClasses[i] = "Whitespace"; break;
			case '\r': ASCIICharacterClasses[i] = "Whitespace"; break;
			case ' ': ASCIICharacterClasses[i] = "Whitespace"; break;
			default:
				if (std::isalpha(i))
					ASCIICharacterClasses[i] = "NonDigit";
				else if (std::isdigit(i))
					ASCIICharacterClasses[i] = "Digit";
				else if (std::ispunct(i))
					ASCIICharacterClasses[i] = "Symbol";
				else
					ASCIICharacterClasses[i] = "Unknown";
				break;
			}
		}

		// Fill UTF character classes
		for (std::uint64_t i = 0; i < 0x10'FFFF; ++i)
		{
			switch (i)
			{
			case '\t': UTFCharacterClasses[i] = "Whitespace"; break;
			case '\n': UTFCharacterClasses[i] = "Newline"; break;
			case '\v': UTFCharacterClasses[i] = "Whitespace"; break;
			case '\f': UTFCharacterClasses[i] = "Whitespace"; break;
			case '\r': UTFCharacterClasses[i] = "Whitespace"; break;
			case ' ': UTFCharacterClasses[i] = "Whitespace"; break;
			default:
				if (i < 128)
				{
					if (std::isalpha(static_cast<int>(i)))
						UTFCharacterClasses[i] = "NonDigit";
					else if (std::isdigit(static_cast<int>(i)))
						UTFCharacterClasses[i] = "Digit";
					else if (std::ispunct(static_cast<int>(i)))
						UTFCharacterClasses[i] = "Symbol";
					else
						UTFCharacterClasses[i] = "Unknown";
				}
				else
				{
					// if (UTF::IsAlphabetic(i))
					//	UTFCharacterClasses[i] = "NonDigit";
					// else if (std::IsDigit(i))
					//	UTFCharacterClasses[i] = "Digit";
					// else if (std::IsPunct(i))
					//	UTFCharacterClasses[i] = "Symbol";
					// else
					UTFCharacterClasses[i] = "Unknown";
				}
				break;
			}
		}
	}
} ClassesFillerThingy;

Tknz TknzParse(std::string_view source)
{
	Tknz tknz {};

	std::size_t offset = 0;
	offset             = SkipWhitespaces(source, offset);
	if (offset >= source.size()) return tknz;

	// Options
	while (offset < source.size() &&
		   source[offset] == '!')
	{
		offset = SkipWhitespaces(source, offset + 1);
		if (offset >= source.size()) break;
		std::size_t idenStart = offset;
		std::size_t idenEnd   = EndOfIdentifier(source, idenStart);
		if (idenEnd > source.size()) break;
		auto iden = source.substr(idenStart, idenEnd - idenStart);
		offset    = SkipWhitespaces(source, idenEnd);
		if (offset >= source.size() ||
			source[offset] != '=') break;
		offset = SkipWhitespaces(source, offset + 1);
		if (offset >= source.size()) break;
		std::size_t statementStart = offset;
		std::size_t statementEnd   = EndOfStatement(source, offset);
		if (statementEnd >= source.size()) break;
		offset = SkipWhitespaces(source, statementEnd + 1);

		if (iden == "TokenClasses")
		{
			if (source[statementStart] != '[' || source[statementEnd - 1] != ']')
				continue;

			std::size_t offset2 = statementStart + 1;
			while (offset2 < statementEnd)
			{
				std::size_t classIdenStart = SkipWhitespaces(source, offset2);
				std::size_t classIdenEnd   = EndOfIdentifier(source, classIdenStart);
				auto        classIden      = source.substr(classIdenStart, classIdenEnd - classIdenStart);

				tknz.TokenClasses.emplace_back(
					TknzClass { .Name     = std::string { classIden },
								.Excluded = false,
								.Rules    = {} });

				offset2 = SkipWhitespaces(source, classIdenEnd);
				if (source[offset2] != ',')
					break;
				++offset2;
			}
		}
		else if (iden == "Excluded")
		{
			if (source[statementStart] != '[' || source[statementEnd - 1] != ']')
				continue;

			std::size_t offset2 = statementStart + 1;
			while (offset2 < statementEnd)
			{
				std::size_t classIdenStart = SkipWhitespaces(source, offset2);
				std::size_t classIdenEnd   = EndOfIdentifier(source, classIdenStart);
				auto        classIden      = source.substr(classIdenStart, classIdenEnd - classIdenStart);

				for (auto& tc : tknz.TokenClasses)
				{
					if (tc.Name == classIden)
					{
						tc.Excluded = true;
						break;
					}
				}

				offset2 = SkipWhitespaces(source, classIdenEnd);
				if (source[offset2] != ',')
					break;
				++offset2;
			}
		}
		else if (iden == "StartTokenClass")
		{
			std::size_t classIdenStart = statementStart;
			std::size_t classIdenEnd   = EndOfIdentifier(source, classIdenStart);
			auto        classIden      = source.substr(classIdenStart, classIdenEnd - classIdenStart);

			tknz.StartTokenClass = classIden;
		}
		else if (iden == "Charset")
		{
			std::size_t charsetStart = statementStart;
			std::size_t charsetEnd   = EndOfIdentifier(source, charsetStart);
			auto        charset      = source.substr(charsetStart, charsetEnd - charsetStart);

			tknz.Charset = charset;
			if (charset == "ASCII")
				tknz.SizePerState = 7;
			else if (charset == "UTF")
				tknz.SizePerState = 32;
			else
				return {};
		}
		else if (iden == "States")
		{
			std::size_t statesStart = statementStart;
			std::size_t statesEnd   = EndOfInteger(source, statesStart);
			auto        states      = source.substr(statesStart, statesEnd - statesStart);

			tknz.ExtraStates = std::strtoull(states.data(), nullptr, 10);
		}
	}
	if (offset >= source.size()) return tknz;

	// Classes
	while (offset < source.size())
	{
		offset = SkipWhitespaces(source, offset);
		if (offset >= source.size()) break;
		std::size_t idenStart = offset;
		std::size_t idenEnd   = EndOfIdentifier(source, idenStart);
		if (idenEnd > source.size()) break;
		auto iden = source.substr(idenStart, idenEnd - idenStart);
		offset    = SkipWhitespaces(source, idenEnd);
		if (offset >= source.size() || source[offset] != '{') break;
		std::size_t classStart = offset;
		std::size_t classEnd   = EndOfClass(source, classStart);
		if (classEnd >= source.size()) break;
		offset = classEnd + 1;

		TknzClass* clazz = nullptr;
		for (auto& tc : tknz.TokenClasses)
		{
			if (tc.Name == iden)
			{
				clazz = &tc;
				break;
			}
		}
		if (!clazz) break;

		// Rules
		std::size_t offset2 = classStart + 1;
		while (offset2 < classEnd)
		{
			TknzRule rule {};

			offset2 = SkipWhitespaces(source, offset2);
			if (offset2 >= classEnd) break;
			std::size_t ruleStart = offset2;
			std::size_t ruleEnd   = EndOfStatement(source, ruleStart);
			if (ruleEnd >= classEnd) break;
			offset2 = ruleEnd + 1;

			// Predicates
			std::size_t offset3 = ruleStart;
			while (offset3 < ruleEnd)
			{
				offset3 = SkipWhitespaces(source, offset3);
				if (offset3 >= ruleEnd) break;
				if ((offset3 + 1) < ruleEnd &&
					source[offset3] == '=' &&
					source[offset3 + 1] == '>')
					break;

				bool inverse = false;
				if (source[offset3] == '!')
				{
					inverse = true;
					offset3 = SkipWhitespaces(source, offset3 + 1);
					if (offset3 >= ruleEnd) break;
				}

				if (source[offset3] == '\'')
				{
					std::size_t charStart = offset3 + 1;
					std::size_t charEnd   = EndOfChar(source, charStart);
					if (charEnd >= ruleEnd) break;
					offset3 = charEnd + 1;

					rule.Predicates.emplace_back(TknzPredicate {
						.Type      = ETknzPredicateType::Char,
						.Inverse   = inverse,
						.Statement = GetCharValue(source.substr(charStart, charEnd - charStart)) });
				}
				else
				{
					std::size_t idenStart2 = offset3;
					std::size_t idenEnd2   = EndOfIdentifier(source, idenStart2);
					if (idenEnd2 > ruleEnd) break;
					offset3    = idenEnd2;
					auto iden2 = source.substr(idenStart2, idenEnd2 - idenStart2);

					if (iden2 == "State")
					{
						offset3 = SkipWhitespaces(source, idenEnd2);
						if (offset3 >= ruleEnd) break;
						if (source[offset3] != '(') break;
						std::size_t argsStart = offset3 + 1;
						std::size_t argsEnd   = EndOfArgs(source, argsStart);
						if (argsEnd >= ruleEnd) break;
						offset3 = argsEnd + 1;

						std::size_t offset4 = SkipWhitespaces(source, argsStart);
						if (offset4 >= argsEnd) break;

						std::size_t stateStart = offset4;
						std::size_t stateEnd   = EndOfInteger(source, stateStart);
						if (stateEnd > argsEnd) break;

						rule.Predicates.emplace_back(TknzPredicate {
							.Type      = ETknzPredicateType::ExtraState,
							.Inverse   = inverse,
							.Statement = std::strtoull(source.data() + stateStart, nullptr, 10) });
					}
					else
					{
						rule.Predicates.emplace_back(TknzPredicate {
							.Type      = ETknzPredicateType::Class,
							.Inverse   = inverse,
							.Statement = std::string { iden2 } });
					}
				}
				offset3 = SkipWhitespaces(source, offset3);
				if (offset3 >= ruleEnd || source[offset3] != '+') break;
				++offset3;
			}

			if ((offset3 + 1) >= ruleEnd ||
				source[offset3] != '=' ||
				source[offset3 + 1] != '>')
				break;
			offset3 += 2;
			if (offset3 >= ruleEnd) break;

			// Results
			while (offset3 < ruleEnd)
			{
				offset3 = SkipWhitespaces(source, offset3);
				if (offset3 >= ruleEnd) break;
				std::size_t idenStart2 = offset3;
				std::size_t idenEnd2   = EndOfIdentifier(source, idenStart2);
				if (idenEnd2 > ruleEnd) break;
				offset3    = idenEnd2;
				auto iden2 = source.substr(idenStart2, idenEnd2 - idenStart2);

				if (iden2 == "State")
				{
					offset3 = SkipWhitespaces(source, idenEnd2);
					if (offset3 >= ruleEnd) break;
					if (source[offset3] != '(') break;
					std::size_t argsStart = offset3 + 1;
					std::size_t argsEnd   = EndOfArgs(source, argsStart);
					if (argsEnd >= ruleEnd) break;
					offset3 = argsEnd + 1;

					std::size_t offset4 = SkipWhitespaces(source, argsStart);
					if (offset4 >= argsEnd) break;

					std::size_t stateStart = offset4;
					std::size_t stateEnd   = EndOfInteger(source, stateStart);
					if (stateEnd > argsEnd) break;

					rule.Results.emplace_back(TknzResult {
						.Type      = ETknzResultType::ExtraState,
						.Statement = std::strtoull(source.data() + stateStart, nullptr, 10) });
				}
				else if (iden2 == "Transition")
				{
					offset3 = SkipWhitespaces(source, idenEnd2);
					if (offset3 >= ruleEnd) break;
					if (source[offset3] != '(') break;
					std::size_t argsStart = offset3 + 1;
					std::size_t argsEnd   = EndOfArgs(source, argsStart);
					if (argsEnd >= ruleEnd) break;
					offset3 = argsEnd + 1;

					std::size_t offset4 = SkipWhitespaces(source, argsStart);
					if (offset4 >= argsEnd) break;

					std::size_t class2Start = offset4;
					std::size_t class2End   = EndOfIdentifier(source, class2Start);
					if (class2End > argsEnd) break;

					rule.Results.emplace_back(TknzResult {
						.Type      = ETknzResultType::Transition,
						.Statement = std::string { source.substr(class2Start, class2End - class2Start) } });
				}
				else
				{
					rule.Results.emplace_back(TknzResult {
						.Type      = ETknzResultType::BaseState,
						.Statement = GetBaseStateValue(iden2) });
				}
				offset3 = SkipWhitespaces(source, offset3);
				if (offset3 >= ruleEnd || source[offset3] != '+') break;
				++offset3;
			}

			clazz->Rules.emplace_back(std::move(rule));
		}
	}
	return tknz;
}

std::string_view* TknzGetCharClasses(Tknz& tknz)
{
	if (tknz.Charset == "ASCII")
		return ASCIICharacterClasses;
	if (tknz.Charset == "UTF")
		return UTFCharacterClasses;
	return nullptr;
}

TknzRule* TknzMatchRule(Tknz& tknz, TknzClass& clazz, std::uint64_t character, std::uint64_t extraState)
{
	std::string_view* characterClasses = TknzGetCharClasses(tknz);
	for (auto& rule : clazz.Rules)
	{
		bool matchesChar       = true;
		bool matchesExtraState = true;

		for (auto& predicate : rule.Predicates)
		{
			switch (predicate.Type)
			{
			case ETknzPredicateType::Class:
			{
				auto charClazz = characterClasses[character & ((1ULL << tknz.SizePerState) - 1ULL)];
				if (predicate.Inverse)
					matchesChar = matchesChar && std::get<std::string>(predicate.Statement) != charClazz;
				else
					matchesChar = matchesChar && std::get<std::string>(predicate.Statement) == charClazz;
				break;
			}
			case ETknzPredicateType::Char:
				if (predicate.Inverse)
					matchesChar = matchesChar && std::get<std::uint64_t>(predicate.Statement) != character;
				else
					matchesChar = matchesChar && std::get<std::uint64_t>(predicate.Statement) == character;
				break;
			case ETknzPredicateType::ExtraState:
			{
				if (predicate.Inverse)
					matchesExtraState = matchesExtraState && std::get<std::uint64_t>(predicate.Statement) != extraState;
				else
					matchesExtraState = matchesExtraState && std::get<std::uint64_t>(predicate.Statement) == extraState;
				break;
			}
			}

			if (!matchesChar || !matchesExtraState)
				break;
		}

		if (matchesChar && matchesExtraState)
			return &rule;
	}
	return nullptr;
}

std::uint64_t TknzRuleBaseState([[maybe_unused]] Tknz& tknz, [[maybe_unused]] TknzClass& clazz, TknzRule& rule)
{
	std::uint64_t value = 0ULL;
	for (auto& result : rule.Results)
		if (result.Type == ETknzResultType::BaseState)
			value |= std::get<std::uint64_t>(result.Statement);
	return value;
}

std::uint64_t TknzRuleExtraState([[maybe_unused]] Tknz& tknz, [[maybe_unused]] TknzClass& clazz, TknzRule& rule)
{
	std::uint64_t value = 0ULL;
	for (auto& result : rule.Results)
		if (result.Type == ETknzResultType::ExtraState)
			value |= 1ULL << std::get<std::uint64_t>(result.Statement);
	return value;
}

std::uint64_t TknzRuleTransition(Tknz& tknz, TknzClass& clazz, TknzRule& rule)
{
	for (auto& result : rule.Results)
	{
		if (result.Type == ETknzResultType::Transition)
		{
			std::string_view clazzName = std::get<std::string>(result.Statement);
			for (std::size_t i = 0; i < tknz.TokenClasses.size(); ++i)
				if (tknz.TokenClasses[i].Name == clazzName)
					return i;
		}
	}
	for (std::size_t i = 0; i < tknz.TokenClasses.size(); ++i)
		if (tknz.TokenClasses[i].Name == clazz.Name)
			return i;
	return 0;
}
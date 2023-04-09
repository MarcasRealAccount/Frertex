#include "Tokenizer.h"
#include "TknzParser.h"

#include <cctype>
#include <cstdint>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

enum class ECharacterClass : std::uint8_t
{
	Unknown = 0,
	Whitespace,
	Tab,
	Newline,
	NonDigit,
	Digit,
	Symbol
};

static std::string_view CharacterClassToString(ECharacterClass clazz)
{
	switch (clazz)
	{
	case ECharacterClass::Unknown: return "Unknown";
	case ECharacterClass::Whitespace: return "Whitespace";
	case ECharacterClass::Tab: return "Tab";
	case ECharacterClass::Newline: return "Newline";
	case ECharacterClass::NonDigit: return "NonDigit";
	case ECharacterClass::Digit: return "Digit";
	case ECharacterClass::Symbol: return "Symbol";
	default: return "Unknown";
	}
}

std::string FormatStr(std::string_view format, const std::vector<std::pair<std::string, std::string>>& replacements)
{
	std::string result;
	std::size_t offset = 0;
	while (offset < format.size())
	{
		std::size_t replacementStart = format.find_first_of('$', offset);
		if (replacementStart == std::string::npos) break;
		std::size_t replacementEnd = format.find_first_of('$', replacementStart + 1);
		if (replacementEnd == std::string::npos) break;
		std::string_view replacementID = format.substr(replacementStart + 1, replacementEnd - replacementStart - 1);
		result                         += format.substr(offset, replacementStart - offset);
		offset                         = replacementEnd + 1;
		for (auto& repl : replacements)
		{
			if (repl.first == replacementID)
			{
				result += repl.second;
				break;
			}
		}
	}
	if (offset < format.size())
		result += format.substr(offset);
	return result;
}

void GenerateTokenizer()
{
	std::string tknzSource;
	{
		std::ifstream file("Tokenizer/Tokens.tknz", std::ios::ate | std::ios::binary);
		if (!file)
		{
			std::cerr << "Failed to open 'Tokenizer/Tokens.tknz'\n";
			return;
		}
		tknzSource.resize(file.tellg());
		file.seekg(0);
		file.read(tknzSource.data(), tknzSource.size());
		file.close();
	}
	Tknz tknz = TknzParse(tknzSource);
	if (tknz.Charset != "ASCII")
	{
		std::cerr << "TokenLUT only supports ASCII charset!\n";
		return;
	}

	std::size_t extraStateBits   = tknz.ExtraStates == 0 ? 0 : std::bit_width(tknz.ExtraStates - 1ULL);
	std::size_t charBits         = tknz.SizePerState;
	std::size_t tokenClassBits   = std::bit_width(tknz.TokenClasses.size());
	std::size_t totalBitWidth    = extraStateBits + charBits + tokenClassBits;
	std::size_t requiredBitWidth = std::bit_ceil(totalBitWidth);
	if (requiredBitWidth > 64)
	{
		std::cerr << "More than 64 bits of total state is unsupported!\n";
		return;
	}

	std::uint64_t extraStateBit  = 0;
	std::uint64_t charBit        = extraStateBits;
	std::uint64_t tokenClassBit  = extraStateBits + charBits;
	std::uint64_t extraStateMask = ((1ULL << extraStateBits) - 1ULL) << extraStateBit;
	std::uint64_t charMask       = ((1ULL << charBits) - 1ULL) << charBit;
	std::uint64_t tokenClassMask = ((1ULL << tokenClassBits) - 1ULL) << tokenClassBit;

	std::uint64_t  totalStates = 1ULL << totalBitWidth;
	std::uint64_t* nextStates  = new std::uint64_t[totalStates];
	for (std::uint64_t curState = 0; curState < totalStates; ++curState)
	{
		std::uint64_t extraStateIn = (curState & extraStateMask) >> extraStateBit;
		std::uint64_t charIn       = (curState & charMask) >> charBit;
		std::uint64_t tokenClassIn = (curState & tokenClassMask) >> tokenClassBit;

		std::uint64_t nextState = TknzResultStateError;
		if (tokenClassIn < tknz.TokenClasses.size())
		{
			auto& tokenClass = tknz.TokenClasses[tokenClassIn];
			auto  rule       = TknzMatchRule(tknz, tokenClass, charIn, extraStateIn);
			if (rule)
			{
				std::uint64_t charOut       = TknzRuleBaseState(tknz, tokenClass, *rule);
				std::uint64_t extraStateOut = TknzRuleExtraState(tknz, tokenClass, *rule);
				std::uint64_t tokenClassOut = TknzRuleTransition(tknz, tokenClass, *rule);

				nextState = extraStateOut << extraStateBit | charOut << charBit | tokenClassOut << tokenClassBit;
			}
		}
		nextStates[curState] = nextState;
	}
	auto charClasses = TknzGetCharClasses(tknz);

	std::string charClassesStr = "Unknown = 0,\nWhitespace,\nNewline,\nNonDigit,\nDigit,\nSymbol";
	std::string charClassesLUTStr;
	std::string tokenClassesStr;
	std::string tokenClassesCasesStr;
	std::string tokenClassesIncludedLUTStr;
	std::string tokenLUTStr;

	std::size_t charClassesCount = 1ULL << charBits;
	for (std::size_t i = 0; i < charClassesCount; ++i)
	{
		if (i > 0)
			charClassesLUTStr += ", ";
		charClassesLUTStr += "ECharacterClass::";
		charClassesLUTStr += charClasses[i];
	}

	for (std::size_t i = 0; i < tknz.TokenClasses.size(); ++i)
	{
		tokenClassesStr            += tknz.TokenClasses[i].Name;
		tokenClassesCasesStr       += "case ETokenClass::" + tknz.TokenClasses[i].Name + ": return \"" + tknz.TokenClasses[i].Name + "\";";
		tokenClassesIncludedLUTStr += tknz.TokenClasses[i].Excluded ? "false" : "true";
		if (i < tknz.TokenClasses.size() - 1)
		{
			tokenClassesStr            += ",\n";
			tokenClassesIncludedLUTStr += ",\n";
			tokenClassesCasesStr       += "\n";
		}
	}

	for (std::size_t i = 0; i < totalStates; ++i)
	{
		if (i > 0)
			tokenLUTStr += ", 0x";
		else
			tokenLUTStr += "0x";
		tokenLUTStr += (std::ostringstream {} << std::hex << std::uppercase << std::setfill('0') << std::setw(totalBitWidth / 4) << nextStates[i]).str();
	}
	delete[] nextStates;

	std::vector<std::pair<std::string, std::string>> replacements {
		{"TYPE",				   "std::uint" + std::to_string(totalBitWidth) + "_t"																								  },
		{ "CHARTYPE",              "std::uint" + std::to_string(std::bit_ceil(tknz.SizePerState)) + "_t"                                                                               },
		{ "EXTRASTATEMASK",        (std::ostringstream {} << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(totalBitWidth / 4) << extraStateMask).str()          },
		{ "EXTRASTATEBIT",         std::to_string(extraStateBit)																													   },
		{ "CHARMASK",              (std::ostringstream {} << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(totalBitWidth / 4) << charMask).str()                },
		{ "CHARBIT",               std::to_string(charBit)																															 },
		{ "TOKENCLASSMASK",        (std::ostringstream {} << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(totalBitWidth / 4) << tokenClassMask).str()          },
		{ "TOKENCLASSBIT",         std::to_string(tokenClassBit)																													   },
		{ "INITIALTOKENCLASS",     tknz.StartTokenClass																																},
		{ "TOKENCLASSES",          tokenClassesStr																																	 },
		{ "TOKENCLASSESCASES",     tokenClassesCasesStr																																},
		{ "CHARCLASSESCOUNT",      (std::ostringstream {} << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(totalBitWidth / 4) << charClassesCount).str()        },
		{ "CHARCLASSES",           charClassesStr																																	  },
		{ "CHARCLASSESLUT",        charClassesLUTStr																																   },
		{ "TOKENCLASSCOUNT",       (std::ostringstream {} << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(totalBitWidth / 4) << tknz.TokenClasses.size()).str()},
		{ "TOKENCLASSINCLUDEDLUT", tokenClassesIncludedLUTStr																														  },
		{ "TOKENLUTSIZE",          (std::ostringstream {} << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(totalBitWidth / 4) << totalStates).str()             },
		{ "TOKENLUT",              tokenLUTStr																																		 },
	};

	std::vector<std::pair<std::string, std::string>> templates {
		{"Tokenizer/Templates/Token.h",        "Tokenizer/Out/Inc/Frertex/Tokenizer/Token.h"},
		{ "Tokenizer/Templates/Token.cpp",     "Tokenizer/Out/Src/Tokenizer/Token.cpp"      },
		{ "Tokenizer/Templates/LUTs.cpp",      "Tokenizer/Out/Src/Tokenizer/LUTs.cpp"       },
		{ "Tokenizer/Templates/Tokenizer.cpp", "Tokenizer/Out/Src/Tokenizer/Tokenizer.cpp"  },
	};
	for (auto& tmpl : templates)
	{
		std::string format;
		{
			std::ifstream file(tmpl.first, std::ios::ate);
			if (!file)
			{
				std::cerr << "Failed to open '" << tmpl.first << "'\n";
				continue;
			}
			format.resize(file.tellg());
			file.seekg(0);
			file.read(format.data(), format.size());
			file.close();
		}

		std::string result = FormatStr(format, replacements);

		{
			std::filesystem::create_directories(std::filesystem::path { tmpl.second }.parent_path());
			std::ofstream file(tmpl.second);
			if (!file)
			{
				std::cerr << "Failed to open '" << tmpl.second << "'\n";
				continue;
			}
			file << result;
			file.close();
		}
	}
}
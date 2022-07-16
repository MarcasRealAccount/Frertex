#include "Frertex/Preprocessor.h"
#include "Frertex/Tokenizer.h"
#include "Frertex/Utils/Utils.h"

#include <format>
#include <unordered_map>

namespace Frertex
{
	std::vector<Token> Preprocess(std::vector<Token>&& tokens, IncludeHandler includeHandler, std::string_view filename, std::vector<std::string>& includedFilenames, std::vector<Message>& messages)
	{
		includedFilenames.emplace_back(filename);

		std::unordered_map<std::string, std::string> macros;

		std::vector<Token> output = std::move(tokens);
		for (auto itr = output.begin(); itr != output.end();)
		{
			if (itr->m_Class == ETokenClass::Preprocessor)
			{
				auto preprocessorSpan = itr->m_Span;

				std::vector<Token> preprocessorTokens = Tokenize(itr->m_Str.substr(1), preprocessorSpan.m_Start);

				itr = output.erase(itr);

				if (preprocessorTokens.empty())
					continue;

				auto& directive = preprocessorTokens[0];
				if (directive.m_Class != ETokenClass::Identifier)
				{
					messages.emplace_back(EMessageType::Error, directive.m_Span, directive.m_Span.m_Start, "Expected identifier");
					continue;
				}

				if (directive.m_Str == "include")
				{
					if (preprocessorTokens.size() < 2)
					{
						SourcePoint point = directive.m_Span.m_End;
						++point.m_Column;
						messages.emplace_back(EMessageType::Error, SourceSpan { point, point }, point, "Expected string or identifier");
						continue;
					}
					else if (preprocessorTokens.size() > 2)
					{
						messages.emplace_back(EMessageType::Warning, SourceSpan { preprocessorTokens[2].m_Span.m_Start, preprocessorTokens.rbegin()->m_Span.m_End }, preprocessorTokens[2].m_Span.m_Start, "Unused");
					}

					auto&       input = preprocessorTokens[1];
					std::string include;

					switch (input.m_Class)
					{
					case ETokenClass::String:
						include = input.m_Str;
						break;
					case ETokenClass::Identifier:
					{
						auto itr2 = macros.find(input.m_Str);
						if (itr2 == macros.end())
						{
							messages.emplace_back(EMessageType::Error, input.m_Span, input.m_Span.m_Start, "Macro has not been defined");
							continue;
						}
						include = itr2->second;
						break;
					}
					}

					if (std::find(includedFilenames.begin(), includedFilenames.end(), include) == includedFilenames.end())
					{
						std::vector<Token> includedTokens = Tokenize(includeHandler(include), { 0, 0, 0, includedFilenames.size() });
						includedTokens                    = Preprocess(std::move(includedTokens), includeHandler, include, includedFilenames, messages);
						itr                               = output.insert(itr, includedTokens.begin(), includedTokens.end());
					}
				}
				else
				{
					messages.emplace_back(EMessageType::Warning, directive.m_Span, directive.m_Span.m_Start, std::format("Unknown preprocessor directive \"{}\"", Utils::EscapeString(directive.m_Str)));
				}
			}
			else
			{
				++itr;
			}
		}
		return output;
	}
} // namespace Frertex
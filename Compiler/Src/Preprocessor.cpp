#include "Frertex/Preprocessor.h"
#include "Frertex/Tokenizer.h"
#include "Frertex/Utils/Utils.h"

#include <format>
#include <unordered_map>

namespace Frertex
{
	Preprocessor::Preprocessor(IncludeHandler includeHandler)
	    : m_IncludeHandler(includeHandler) {}

	std::vector<Token> Preprocessor::process(Utils::CopyMovable<std::vector<Token>>&& tokens, std::string_view filename)
	{
		m_IncludedFilenames.emplace_back(filename);

		std::vector<Token> output = tokens.get();

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
					addError(directive.m_Span, directive.m_Span.m_Start, "Expected identifier");
					continue;
				}

				if (directive.m_Str == "include")
				{
					if (preprocessorTokens.size() < 2)
					{
						SourcePoint point = directive.m_Span.m_End;
						++point.m_Column;
						addError(SourceSpan { point, point }, point, "Expected string or identifier");
						continue;
					}
					else if (preprocessorTokens.size() > 2)
					{
						addWarning(SourceSpan { preprocessorTokens[2].m_Span.m_Start, preprocessorTokens.rbegin()->m_Span.m_End }, preprocessorTokens[2].m_Span.m_Start, "Unused");
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
						auto macro = getMacro(input.m_Str);
						if (!macro)
						{
							addError(input.m_Span, input.m_Span.m_Start, "Macro has not been defined");
							continue;
						}
						if (macro->empty())
						{
							addWarning(input.m_Span, input.m_Span.m_Start, "Macro is empty, skipping include");
							continue;
						}
						if ((*macro)[0].m_Class != ETokenClass::String)
						{
							addError(input.m_Span, input.m_Span.m_Start, "Macro requires first token to be a string (Future: Evaluate all tokens first)");
							continue;
						}
						include = (*macro)[0].m_Str;
						break;
					}
					}

					if (!hasIncludedFile(include))
					{
						auto result = m_IncludeHandler(include);
						if (result.m_Status == EIncludeStatus::Failure)
						{
							addError(input.m_Span, input.m_Span.m_Start, std::format("File \"{}\" not found", Utils::EscapeString(include)));
							continue;
						}

						std::vector<Token> includedTokens = Tokenize(result.m_Source, { 0, 0, 0, m_IncludedFilenames.size() });
						includedTokens                    = process(std::move(includedTokens), include);
						itr                               = output.insert(itr, includedTokens.begin(), includedTokens.end());
					}
				}
				else if (directive.m_Str == "define")
				{
					if (preprocessorTokens.size() < 2)
					{
						SourcePoint point = directive.m_Span.m_End;
						++point.m_Column;
						addError(SourceSpan { point, point }, point, "Expected identifier");
						continue;
					}

					auto& name = preprocessorTokens[1];
					if (name.m_Class != ETokenClass::Identifier)
					{
						addError(name.m_Span, name.m_Span.m_Start, "Expected identifier");
						continue;
					}

					if (preprocessorTokens.size() > 2)
					{
						bool macroFunction = false;
						if (preprocessorTokens.size() >= 4) // Safe to say it might be a macro function
						{
							if (preprocessorTokens[2].m_Class == ETokenClass::Symbol && preprocessorTokens[2].m_Str == "(")
							{
								std::size_t functionEnd = 3;
								while (functionEnd < preprocessorTokens.size())
								{
									auto& token = preprocessorTokens[functionEnd];
									if (token.m_Class == ETokenClass::Symbol && token.m_Str == ")")
										break;
									++functionEnd;
								}
								if (functionEnd == preprocessorTokens.size())
								{
									addError(preprocessorTokens[3].m_Span, preprocessorTokens[3].m_Span.m_Start, "Expected ')'");
									continue;
								}
								macroFunction = true;

								addError(SourceSpan { preprocessorTokens[1].m_Span.m_Start, preprocessorTokens[functionEnd].m_Span.m_End }, preprocessorTokens[2].m_Span.m_Start, "Macro functions not yet implemented, sorry :>");

								if (hasMacro(name.m_Str))
									addWarning(name.m_Span, name.m_Span.m_Start, "Macro already defined, redefining");

								/*std::vector<Token> valueTokens(preprocessorTokens.size() - functionEnd);
								for (std::size_t i = 0; i < valueTokens.size(); ++i)
									valueTokens[i] = std::move(preprocessorTokens[i + functionEnd]);
								addMacro(name.m_Str, std::move(valueTokens));*/
							}
						}

						if (!macroFunction)
						{
							if (hasMacro(name.m_Str))
								addWarning(name.m_Span, name.m_Span.m_Start, "Macro already defined, redefining");

							std::vector<Token> valueTokens(preprocessorTokens.size() - 2);
							for (std::size_t i = 0; i < valueTokens.size(); ++i)
								valueTokens[i] = std::move(preprocessorTokens[i + 2]);
							addMacro(name.m_Str, std::move(valueTokens));
						}
					}
					else // Plain macro
					{
						if (hasMacro(name.m_Str))
							addWarning(name.m_Span, name.m_Span.m_Start, "Macro already defined, redefining");
						addMacro(name.m_Str);
					}
				}
				else if (directive.m_Str == "undefine")
				{
					if (preprocessorTokens.size() < 2)
					{
						SourcePoint point = directive.m_Span.m_End;
						++point.m_Column;
						addError(SourceSpan { point, point }, point, "Expected string or identifier");
						continue;
					}
					else if (preprocessorTokens.size() > 2)
					{
						addWarning(SourceSpan { preprocessorTokens[2].m_Span.m_Start, preprocessorTokens.rbegin()->m_Span.m_End }, preprocessorTokens[2].m_Span.m_Start, "Unused");
					}

					auto& name = preprocessorTokens[1];
					if (name.m_Class != ETokenClass::Identifier)
					{
						addError(name.m_Span, name.m_Span.m_Start, "Expected identifier");
						continue;
					}

					removeMacro(name.m_Str);
				}
				else
				{
					addWarning(directive.m_Span, directive.m_Span.m_Start, std::format("Unknown preprocessor directive \"{}\"", Utils::EscapeString(directive.m_Str)));
				}
			}
			else
			{
				++itr;
			}
		}

		return output;
	}

	void Preprocessor::addMacro(Utils::CopyMovable<std::string>&& name)
	{
		m_Macros.insert_or_assign(name.get(), std::vector<Token> {});
	}

	void Preprocessor::addMacro(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& value)
	{
		std::string str = value.get();
		m_Macros.insert_or_assign(name.get(), Tokenize(str, { 0, 0, 0, ~0ULL }));
	}

	void Preprocessor::addMacro(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::vector<Token>>&& value)
	{
		m_Macros.insert_or_assign(name.get(), value.get());
	}

	void Preprocessor::removeMacro(std::string_view name)
	{
		auto itr = m_Macros.begin();
		while (itr != m_Macros.end() && itr->first != name)
			++itr;
		if (itr != m_Macros.end())
			m_Macros.erase(itr);
	}

	bool Preprocessor::hasMacro(std::string_view name)
	{
		auto itr = m_Macros.begin();
		while (itr != m_Macros.end() && itr->first != name)
			++itr;
		return itr != m_Macros.end();
	}

	std::vector<Token>* Preprocessor::getMacro(std::string_view name)
	{
		auto itr = m_Macros.begin();
		while (itr != m_Macros.end() && itr->first != name)
			++itr;
		return itr != m_Macros.end() ? &itr->second : nullptr;
	}

	void Preprocessor::addWarning(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		m_Messages.emplace_back(EMessageType::Warning, span, point, message.get());
	}

	void Preprocessor::addError(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		m_Messages.emplace_back(EMessageType::Error, span, point, message.get());
	}

	bool Preprocessor::hasIncludedFile(std::string_view filename)
	{
		auto itr = m_IncludedFilenames.begin();
		while (itr != m_IncludedFilenames.end() && *itr != filename)
			++itr;
		return itr != m_IncludedFilenames.end();
	}
} // namespace Frertex
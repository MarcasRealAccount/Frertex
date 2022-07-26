#include "Frertex/Preprocessor.h"
#include "Frertex/Tokenizer.h"
#include "Frertex/Utils/Profiler.h"
#include "Frertex/Utils/Utils.h"

#include <fmt/format.h>

#include <unordered_map>

namespace Frertex
{
	Preprocessor::Preprocessor(Sources* sources, IncludeHandler includeHandler)
	    : m_Sources(sources), m_IncludeHandler(includeHandler) {}

	std::vector<Token> Preprocessor::process(Utils::CopyMovable<std::vector<Token>>&& tokens)
	{
		PROFILE_FUNC;

		// TODO(MarcasRealAccount): Implement macro replacement in other tokens ;)

		std::vector<Token> output = tokens.get();

		// Remove comments from the preprocessed tokens
		std::erase_if(
		    output,
		    [](Token& token) -> bool
		    {
			    return token.m_Class == ETokenClass::Comment || token.m_Class == ETokenClass::MultilineComment;
		    });

		for (auto itr = output.begin(); itr != output.end();)
		{
			if (itr->m_Class == ETokenClass::Preprocessor)
			{
				bool eraseDirective = true;
				do
				{
					std::vector<Token> preprocessorTokens = Tokenize(m_Sources->getSource(itr->m_SourceID), itr->m_Index + 1, itr->m_Length - 1);

					if (preprocessorTokens.empty())
						continue;

					auto& directive    = preprocessorTokens[0];
					auto  directiveStr = directive.getView(*m_Sources);
					if (directive.m_Class != ETokenClass::Identifier)
					{
						addError(directive, directive.m_Index, "Expected identifier");
						continue;
					}

					bool directiveHandled = false;

					if (directiveStr == "include")
					{
						directiveHandled = true;
						if (preprocessorTokens.size() < 2)
						{
							std::size_t point = directive.m_Index + directive.m_Length + 1;
							addError(directive.m_FileID, directive.m_SourceID, point, 1, point, "Expected string or identifier");
							continue;
						}

						auto&       input = preprocessorTokens[1];
						std::string include;

						switch (input.m_Class)
						{
						case ETokenClass::String:
							if (preprocessorTokens.size() > 2)
							{
								std::size_t start  = preprocessorTokens[2].m_Index;
								std::size_t length = preprocessorTokens.back().m_Length;
								addWarning(preprocessorTokens[2].m_FileID, preprocessorTokens[2].m_SourceID, start, length, start, "Unused");
							}
							include = input.getView(*m_Sources);
							break;
						case ETokenClass::Identifier:
						{
							auto macro = getMacro(input.getView(*m_Sources));
							if (!macro)
							{
								addError(input, input.m_Index, "Macro has not been defined");
								continue;
							}
							if (macro->empty())
							{
								addWarning(input, input.m_Index, "Macro is empty, skipping include");
								continue;
							}
							if ((*macro)[0].m_Class != ETokenClass::String)
							{
								addError(input, input.m_Index, "Macro requires first token to be a string (Future: Evaluate all tokens first)");
								continue;
							}
							include = (*macro)[0].getView(*m_Sources);
							break;
						}
						default:
							addError(input, input.m_Index, "Expected string or identifier");
							continue;
						}

						if (!hasIncludedFile(include))
						{
							auto result = m_IncludeHandler(include, m_Sources->getSourceName(input.m_SourceID));
							if (result.m_Status == EIncludeStatus::Failure)
							{
								addError(input, input.m_Index, fmt::format("File \"{}\" not found", Utils::EscapeString(include)));
								continue;
							}

							std::uint32_t      newFileID      = m_Sources->addSource(include, std::move(result.m_Source));
							std::vector<Token> includedTokens = Tokenize(m_Sources->getSource(newFileID));

							itr                     = output.erase(itr);
							eraseDirective          = false;
							std::size_t startOffset = itr - output.begin();
							output.insert(itr, includedTokens.begin(), includedTokens.end());
							itr = output.begin() + startOffset;
							m_IncludedFilenames.emplace_back(include);
						}
					}
					else if (directiveStr == "define")
					{
						directiveHandled = true;
						if (preprocessorTokens.size() < 2)
						{
							std::size_t point = directive.m_Index + directive.m_Length + 1;
							addError(directive.m_FileID, directive.m_SourceID, point, 1, point, "Expected identifier");
							continue;
						}

						auto& name = preprocessorTokens[1];
						if (name.m_Class != ETokenClass::Identifier)
						{
							addError(name, name.m_Index, "Expected identifier");
							continue;
						}

						if (preprocessorTokens.size() > 2)
						{
							bool macroFunction = false;
							if (preprocessorTokens.size() >= 4) // Safe to say it might be a macro function
							{
								if (preprocessorTokens[2].m_Class == ETokenClass::Symbol && preprocessorTokens[2].getView(*m_Sources) == "(")
								{
									std::size_t functionEnd = 3;
									while (functionEnd < preprocessorTokens.size())
									{
										auto& token = preprocessorTokens[functionEnd];
										if (token.m_Class == ETokenClass::Symbol && token.getView(*m_Sources) == ")")
											break;
										++functionEnd;
									}
									if (functionEnd == preprocessorTokens.size())
									{
										addError(preprocessorTokens[3], preprocessorTokens[3].m_Index, "Expected ')'");
										continue;
									}
									macroFunction = true;

									std::size_t start = preprocessorTokens[1].m_Index;
									std::size_t end   = preprocessorTokens[functionEnd].m_Index;
									addError(preprocessorTokens[1].m_FileID, preprocessorTokens[1].m_SourceID, start, end - start, preprocessorTokens[2].m_Index, "Macro functions not yet implemented, sorry :>");

									if (hasMacro(name.getView(*m_Sources)))
										addWarning(name, name.m_Index, "Macro already defined, redefining");

									/*std::vector<Token> valueTokens(preprocessorTokens.size() - functionEnd);
									for (std::size_t i = 0; i < valueTokens.size(); ++i)
										valueTokens[i] = std::move(preprocessorTokens[i + functionEnd]);
									addMacro(name.getView(*m_Sources), std::move(valueTokens));*/
								}
							}

							if (!macroFunction)
							{
								if (hasMacro(name.getView(*m_Sources)))
									addWarning(name, name.m_Index, "Macro already defined, redefining");

								std::vector<Token> valueTokens(preprocessorTokens.size() - 2);
								for (std::size_t i = 0; i < valueTokens.size(); ++i)
									valueTokens[i] = std::move(preprocessorTokens[i + 2]);
								addMacro(name.getView(*m_Sources), std::move(valueTokens));
							}
						}
						else // Plain macro
						{
							if (hasMacro(name.getView(*m_Sources)))
								addWarning(name, name.m_Index, "Macro already defined, redefining");
							addMacro(name.getView(*m_Sources));
						}
					}
					else if (directiveStr == "undefine")
					{
						directiveHandled = true;
						if (preprocessorTokens.size() < 2)
						{
							std::size_t point = directive.m_Index + directive.m_Length + 1;
							addError(directive.m_FileID, directive.m_SourceID, point, 1, point, "Expected identifier");
							continue;
						}
						else if (preprocessorTokens.size() > 2)
						{
							std::size_t start  = preprocessorTokens[2].m_Index;
							std::size_t length = preprocessorTokens.back().m_Length;
							addWarning(preprocessorTokens[2].m_FileID, preprocessorTokens[2].m_SourceID, start, length, start, "Unused");
						}

						auto& name = preprocessorTokens[1];
						if (name.m_Class != ETokenClass::Identifier)
						{
							addError(name, name.m_Index, "Expected identifier");
							continue;
						}

						removeMacro(name.getView(*m_Sources));
					}
					else if (directiveStr == "error")
					{
						directiveHandled = true;
						if (preprocessorTokens.size() < 2)
						{
							std::size_t point = directive.m_Index + directive.m_Length + 1;
							addError(directive.m_FileID, directive.m_SourceID, point, 1, point, "Expected string or identifier");
							continue;
						}
						else if (preprocessorTokens.size() > 2)
						{
							std::size_t start  = preprocessorTokens[2].m_Index;
							std::size_t length = preprocessorTokens.back().m_Length;
							addWarning(preprocessorTokens[2].m_FileID, preprocessorTokens[2].m_SourceID, start, length, start, "Unused");
						}

						auto&       input = preprocessorTokens[1];
						std::string error;

						switch (input.m_Class)
						{
						case ETokenClass::String:
							error = input.getView(*m_Sources);
							break;
						case ETokenClass::Identifier:
						{
							auto macro = getMacro(input.getView(*m_Sources));
							if (!macro)
							{
								addError(input, input.m_Index, "Macro has not been defined");
								continue;
							}
							if (macro->empty())
							{
								addWarning(input, input.m_Index, "Macro is empty, skipping include");
								continue;
							}
							if ((*macro)[0].m_Class != ETokenClass::String)
							{
								addError(input, input.m_Index, "Macro requires first token to be a string (Future: Evaluate all tokens first)");
								continue;
							}
							error = (*macro)[0].getView(*m_Sources);
							break;
						}
						default:
							addError(input, input.m_Index, "Expected string or identifier");
							continue;
						}

						addError(*itr, directive.m_Index, std::move(error));
					}
					else if (directiveStr == "warn")
					{
						directiveHandled = true;
						if (preprocessorTokens.size() < 2)
						{
							std::size_t point = directive.m_Index + directive.m_Length + 1;
							addError(directive.m_FileID, directive.m_SourceID, point, 1, point, "Expected string or identifier");
							continue;
						}
						else if (preprocessorTokens.size() > 2)
						{
							std::size_t start  = preprocessorTokens[2].m_Index;
							std::size_t length = preprocessorTokens.back().m_Length;
							addWarning(preprocessorTokens[2].m_FileID, preprocessorTokens[2].m_SourceID, start, length, start, "Unused");
						}

						auto&       input = preprocessorTokens[1];
						std::string warn;

						switch (input.m_Class)
						{
						case ETokenClass::String:
							warn = input.getView(*m_Sources);
							break;
						case ETokenClass::Identifier:
						{
							auto macro = getMacro(input.getView(*m_Sources));
							if (!macro)
							{
								addError(input, input.m_Index, "Macro has not been defined");
								continue;
							}
							if (macro->empty())
							{
								addWarning(input, input.m_Index, "Macro is empty, skipping include");
								continue;
							}
							if ((*macro)[0].m_Class != ETokenClass::String)
							{
								addError(input, input.m_Index, "Macro requires first token to be a string (Future: Evaluate all tokens first)");
								continue;
							}
							warn = (*macro)[0].getView(*m_Sources);
							break;
						}
						default:
							addError(input, input.m_Index, "Expected string or identifier");
							continue;
						}

						addWarning(*itr, directive.m_Index, std::move(warn));
					}
					/*else if (directiveStr == "line")
					{
						directiveHandled = true;
						if (preprocessorTokens.size() < 2)
						{
							std::size_t point = directive.m_Index + directive.m_Length + 1;
							addError(directive.m_FileID, directive.m_SourceID, point, 1, point, "Expected integer or identifier");
							continue;
						}

						auto&       lineNumber = preprocessorTokens[1];
						std::size_t ln         = 0;
						switch (lineNumber.m_Class)
						{
						case ETokenClass::Integer:
							ln = std::strtoull(lineNumber.getView(*m_Sources).data(), nullptr, 10);
							break;
						case ETokenClass::BinaryInteger:
							ln = std::strtoull(lineNumber.getView(*m_Sources).data() + 2, nullptr, 2);
							break;
						case ETokenClass::OctalInteger:
							ln = std::strtoull(lineNumber.getView(*m_Sources).data() + 2, nullptr, 8);
							break;
						case ETokenClass::HexInteger:
							ln = std::strtoull(lineNumber.getView(*m_Sources).data() + 2, nullptr, 16);
							break;
						case ETokenClass::Identifier:
						{
							auto macro = getMacro(lineNumber.getView(*m_Sources));
							if (!macro)
							{
								addError(lineNumber, lineNumber.m_Index, "Macro has not been defined");
								continue;
							}
							if (macro->empty())
							{
								addWarning(lineNumber, lineNumber.m_Index, "Macro is empty, skipping include");
								continue;
							}

							auto& lnn = (*macro)[0];
							switch (lnn.m_Class)
							{
							case ETokenClass::Integer:
								ln = std::strtoull(lineNumber.getView(*m_Sources).data(), nullptr, 10);
								break;
							case ETokenClass::BinaryInteger:
								ln = std::strtoull(lineNumber.getView(*m_Sources).data() + 2, nullptr, 2);
								break;
							case ETokenClass::OctalInteger:
								ln = std::strtoull(lineNumber.getView(*m_Sources).data() + 2, nullptr, 8);
								break;
							case ETokenClass::HexInteger:
								ln = std::strtoull(lineNumber.getView(*m_Sources).data() + 2, nullptr, 16);
								break;
							default:
								addError(lineNumber, lineNumber.m_Index, "Macro requires first token to be an integer (Future: Evaluate all tokens first)");
								continue;
							}
							break;
						}
						default:
							addError(lineNumber, lineNumber.m_Index, "Expected integer or identifier");
							continue;
						}

						std::uint32_t file = directive.m_FileID;

						if (preprocessorTokens.size() > 2)
						{
							auto& filenameToken = preprocessorTokens[2];

							std::string_view filenameStr;
							switch (filenameToken.m_Class)
							{
							case ETokenClass::String:
								if (preprocessorTokens.size() > 3)
								{
									std::size_t start  = preprocessorTokens[3].m_Index;
									std::size_t length = preprocessorTokens.back().m_Length;
									addWarning(preprocessorTokens[3].m_FileID, preprocessorTokens[3].m_SourceID, start, length, start, "Unused");
								}
								filenameStr = filenameToken.getView(*m_Sources);
								break;
							case ETokenClass::Identifier:
							{
								auto macro = getMacro(filenameToken.getView(*m_Sources));
								if (!macro)
								{
									addError(filenameToken, filenameToken.m_Index, "Macro has not been defined");
									continue;
								}
								if (macro->empty())
								{
									addWarning(filenameToken, filenameToken.m_Index, "Macro is empty, skipping include");
									continue;
								}
								if ((*macro)[0].m_Class != ETokenClass::String)
								{
									addError(filenameToken, filenameToken.m_Index, "Macro requires first token to be a string (Future: Evaluate all tokens first)");
									continue;
								}
								filenameStr = (*macro)[0].getView(*m_Sources);
								break;
							}
							default:
								addError(filenameToken, filenameToken.m_Index, "Expected string or identifier");
								continue;
							}

							file = m_Sources->getSourceID(filenameStr);
							if (!file)
								file = m_Sources->addSource(filenameStr, "");
						}

						std::uint32_t baseFile = directive.m_Span.m_Start.m_File;
						std::size_t   baseline = directive.m_Span.m_Start.m_Line + 2;

						for (auto itr2 = itr + 1; itr2 != output.end(); ++itr2)
						{
							if (itr2->m_Span.m_Start.m_File == baseFile)
							{
								itr2->m_Span.m_Start.m_Line -= baseline;
								itr2->m_Span.m_Start.m_Line += ln;
								itr2->m_Span.m_Start.m_File = file;
								itr2->m_Span.m_End.m_Line -= baseline;
								itr2->m_Span.m_End.m_Line += ln;
								itr2->m_Span.m_End.m_File = file;
							}
						}
					}*/
					else if (directiveStr == "elif")
					{
						directiveHandled = true;
						addError(directive, directive.m_Index, "Unexpected directive");
						continue;
					}
					else if (directiveStr == "elifdef")
					{
						directiveHandled = true;
						addError(directive, directive.m_Index, "Unexpected directive");
						continue;
					}
					else if (directiveStr == "elifndef")
					{
						directiveHandled = true;
						addError(directive, directive.m_Index, "Unexpected directive");
						continue;
					}
					else if (directiveStr == "else")
					{
						directiveHandled = true;
						addError(directive, directive.m_Index, "Unexpected directive");
						continue;
					}
					else if (directiveStr == "endif")
					{
						directiveHandled = true;
						addError(directive, directive.m_Index, "Unexpected directive");
						continue;
					}
					else if (directiveStr == "if" ||
					         directiveStr == "ifdef" ||
					         directiveStr == "ifndef")
					{
						eraseDirective          = false;
						directiveHandled        = true;
						std::size_t startOffset = itr - output.begin();
						std::size_t state       = 0;
						auto        start       = itr;
						auto        end         = itr;
						while (state < 3)
						{
							++end;
							while (end != output.end())
							{
								if (end->m_Class == ETokenClass::Preprocessor)
								{
									std::vector<Token> preprocessorTokens2 = Tokenize(m_Sources->getSource(end->m_SourceID), end->m_Index + 1, end->m_Length - 1);
									if (preprocessorTokens2.empty() ||
									    preprocessorTokens2[0].m_Class != ETokenClass::Identifier)
									{
										++end;
										continue;
									}

									auto directive2 = preprocessorTokens2[0].getView(*m_Sources);
									if (state == 0)
									{
										if (directive2 == "elif" ||
										    directive2 == "elifdef" ||
										    directive2 == "elifndef" ||
										    directive2 == "else" ||
										    directive2 == "endif")
											break;
									}
									else if (state == 1 ||
									         state == 2)
									{
										if (directive2 == "endif")
											break;
									}

									++end;
								}
							}
							if (end == output.end())
							{
								auto&       back  = output.back();
								std::size_t point = back.m_Index + back.m_Length;
								addWarning(back.m_FileID, back.m_SourceID, point, 1, point, state == 0 ? "Expected \"#elif\", \"#elifdef\", \"#elifndef\", \"#else\" or \"#endif\" directive" : (state == 1 ? "Expected \"#else\" or \"#endif\" directive" : "Expected \"#endif\" directive"));
								continue;
							}

							bool enable = false;

							do
							{
								std::vector<Token> preprocessorTokensStart = Tokenize(m_Sources->getSource(start->m_SourceID), start->m_Index + 1, start->m_Length - 1);
								if (preprocessorTokensStart.empty() ||
								    preprocessorTokensStart[0].m_Class != ETokenClass::Identifier)
								{
									addError(*start, start->m_Index, "Expected Identifier");
									continue;
								}

								auto& directiveStart    = preprocessorTokensStart[0];
								auto  directiveStartStr = directiveStart.getView(*m_Sources);

								if (directiveStartStr == "if" ||
								    directiveStartStr == "elif")
								{
									if (preprocessorTokensStart.size() < 2)
									{
										std::size_t point = directiveStart.m_Index + directiveStart.m_Length + 1;
										addError(directiveStart.m_FileID, directiveStart.m_SourceID, point, 1, point, "Expected bool statement");
										continue;
									}

									auto& statement    = preprocessorTokensStart[1];
									auto  statementStr = statement.getView(*m_Sources);
									if (statement.m_Class != ETokenClass::Identifier &&
									    statement.m_Class != ETokenClass::Integer &&
									    statement.m_Class != ETokenClass::BinaryInteger &&
									    statement.m_Class != ETokenClass::OctalInteger &&
									    statement.m_Class != ETokenClass::HexInteger)
									{
										addError(statement, statement.m_Index, "Expected bool statement");
										continue;
									}

									switch (statement.m_Class)
									{
									case ETokenClass::Identifier:
										if (statementStr == "false")
											enable = false;
										else if (statementStr == "true")
											enable = true;
										else
											addError(statement, statement.m_Index, "Expected bool statement");
										break;
									case ETokenClass::Integer:
									{
										char*        endStr;
										std::int64_t integer = std::strtoll(statementStr.data(), &endStr, 10);
										enable               = integer != 0;
										break;
									}
									case ETokenClass::BinaryInteger:
									{
										char*        endStr;
										std::int64_t integer = std::strtoll(statementStr.data() + 2, &endStr, 2);
										enable               = integer != 0;
										break;
									}
									case ETokenClass::OctalInteger:
									{
										char*        endStr;
										std::int64_t integer = std::strtoll(statementStr.data() + 2, &endStr, 8);
										enable               = integer != 0;
										break;
									}
									case ETokenClass::HexInteger:
									{
										char*        endStr;
										std::int64_t integer = std::strtoll(statementStr.data() + 2, &endStr, 16);
										enable               = integer != 0;
										break;
									}
									default:
										addError(statement, statement.m_Index, "Expected bool or integer");
										continue;
									}
								}
								else if (directiveStartStr == "ifdef" ||
								         directiveStartStr == "elifdef")
								{
									if (preprocessorTokensStart.size() < 2)
									{
										std::size_t point = directiveStart.m_Index + directiveStart.m_Length + 1;
										addError(directiveStart.m_FileID, directiveStart.m_SourceID, point, 1, point, "Expected identifier");
										continue;
									}
									else if (preprocessorTokensStart.size() > 2)
									{
										std::size_t start  = preprocessorTokens[2].m_Index;
										std::size_t length = preprocessorTokens.back().m_Length;
										addWarning(preprocessorTokens[2].m_FileID, preprocessorTokens[2].m_SourceID, start, length, start, "Unused");
									}

									auto& name = preprocessorTokensStart[1];
									if (name.m_Class != ETokenClass::Identifier)
									{
										addError(name, name.m_Index, "Expected identifier");
										continue;
									}

									enable = hasMacro(name.getView(*m_Sources));
								}
								else if (directiveStartStr == "ifndef" ||
								         directiveStartStr == "elifndef")
								{
									if (preprocessorTokensStart.size() < 2)
									{
										std::size_t point = directiveStart.m_Index + directiveStart.m_Length + 1;
										addError(directiveStart.m_FileID, directiveStart.m_SourceID, point, 1, point, "Expected identifier");
										continue;
									}
									else if (preprocessorTokensStart.size() > 2)
									{
										std::size_t start  = preprocessorTokens[2].m_Index;
										std::size_t length = preprocessorTokens.back().m_Length;
										addWarning(preprocessorTokens[2].m_FileID, preprocessorTokens[2].m_SourceID, start, length, start, "Unused");
									}

									auto& name = preprocessorTokensStart[1];
									if (name.m_Class != ETokenClass::Identifier)
									{
										addError(name, name.m_Index, "Expected identifier");
										continue;
									}

									enable = !hasMacro(name.getView(*m_Sources));
								}
								else if (directiveStartStr == "else")
								{
									enable = state != 2;
								}
							} while (false);

							do
							{
								std::vector<Token> preprocessorTokensEnd = Tokenize(m_Sources->getSource(end->m_SourceID), end->m_Index + 1, end->m_Length - 1);
								if (preprocessorTokensEnd.empty() ||
								    preprocessorTokensEnd[0].m_Class != ETokenClass::Identifier)
								{
									addError(*end, end->m_Index, "Expected Identifier");
									continue;
								}

								auto& directiveEnd    = preprocessorTokensEnd[0];
								auto  directiveEndStr = directiveEnd.getView(*m_Sources);
								if (directiveEndStr == "else")
									++state;
								else if (directiveEndStr == "endif")
									state = 3;
							} while (false);

							if (enable)
							{
								if (state < 2)
									state = 2;
								std::size_t offset = end - output.begin();
								output.erase(start);
								end = output.begin() + offset - 1;
								if (state >= 3)
									end = output.erase(end);
								start = end;
							}
							else
							{
								end = output.erase(start, end);
								if (state >= 3)
									end = output.erase(end);
								start = end;
							}
						}
						itr = output.begin() + startOffset;
					}

					if (!directiveHandled)
						addWarning(directive, directive.m_Index, fmt::format("Unknown preprocessor directive \"{}\"", Utils::EscapeString(directiveStr)));
				} while (false);

				if (eraseDirective)
					itr = output.erase(itr);
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
		PROFILE_FUNC;

		m_Macros.insert_or_assign(name.get(), std::vector<Token> {});
	}

	void Preprocessor::addMacro(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& value)
	{
		PROFILE_FUNC;

		auto id = m_Sources->addSource(name.get(), value.get());
		m_Macros.insert_or_assign(name.get(), Tokenize(m_Sources->getSource(id)));
	}

	void Preprocessor::addMacro(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::vector<Token>>&& value)
	{
		PROFILE_FUNC;

		m_Macros.insert_or_assign(name.get(), value.get());
	}

	void Preprocessor::removeMacro(std::string_view name)
	{
		PROFILE_FUNC;

		auto itr = m_Macros.begin();
		while (itr != m_Macros.end() && itr->first != name)
			++itr;
		if (itr != m_Macros.end())
			m_Macros.erase(itr);
	}

	bool Preprocessor::hasMacro(std::string_view name)
	{
		PROFILE_FUNC;

		auto itr = m_Macros.begin();
		while (itr != m_Macros.end() && itr->first != name)
			++itr;
		return itr != m_Macros.end();
	}

	std::vector<Token>* Preprocessor::getMacro(std::string_view name)
	{
		PROFILE_FUNC;

		auto itr = m_Macros.begin();
		while (itr != m_Macros.end() && itr->first != name)
			++itr;
		return itr != m_Macros.end() ? &itr->second : nullptr;
	}

	bool Preprocessor::hasIncludedFile(std::string_view filename)
	{
		PROFILE_FUNC;

		auto itr = m_IncludedFilenames.begin();
		while (itr != m_IncludedFilenames.end() && *itr != filename)
			++itr;
		return itr != m_IncludedFilenames.end();
	}

	void Preprocessor::addWarning(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		auto span             = m_Sources->getSpan(index, length, sourceID);
		auto pt               = m_Sources->getPoint(point, sourceID);
		span.m_Start.m_FileID = fileID;
		span.m_End.m_FileID   = fileID;
		pt.m_FileID           = fileID;
		m_Messages.emplace_back(EMessageType::Warning, span, pt, message.get());
	}

	void Preprocessor::addError(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		auto span             = m_Sources->getSpan(index, length, sourceID);
		auto pt               = m_Sources->getPoint(point, sourceID);
		span.m_Start.m_FileID = fileID;
		span.m_End.m_FileID   = fileID;
		pt.m_FileID           = fileID;
		m_Messages.emplace_back(EMessageType::Error, span, pt, message.get());
	}
} // namespace Frertex
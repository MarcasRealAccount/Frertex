#include "Frertex/Preprocessor.h"
#include "Frertex/Tokenizer.h"
#include "Frertex/Utils/Utils.h"

#include <fmt/format.h>

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
			if (itr->m_Class == ETokenClass::Comment ||
			    itr->m_Class == ETokenClass::MultilineComment)
			{
				// Remove comments from the preprocessed tokens
				itr = output.erase(itr);
			}
			else if (itr->m_Class == ETokenClass::Preprocessor)
			{
				bool eraseDirective = true;
				do
				{
					auto preprocessorSpan = itr->m_Span;

					std::vector<Token> preprocessorTokens = Tokenize(itr->m_Str.substr(1), preprocessorSpan.m_Start);

					if (preprocessorTokens.empty())
						continue;

					auto& directive = preprocessorTokens[0];
					if (directive.m_Class != ETokenClass::Identifier)
					{
						addError(directive.m_Span, directive.m_Span.m_Start, "Expected identifier");
						continue;
					}

					bool directiveHandled = false;

					if (directive.m_Str == "include")
					{
						directiveHandled = true;
						if (preprocessorTokens.size() < 2)
						{
							SourcePoint point = directive.m_Span.m_End;
							++point.m_Column;
							addError(SourceSpan { point, point }, point, "Expected string or identifier");
							continue;
						}

						auto&       input = preprocessorTokens[1];
						std::string include;

						switch (input.m_Class)
						{
						case ETokenClass::String:
							if (preprocessorTokens.size() > 2)
								addWarning(SourceSpan { preprocessorTokens[2].m_Span.m_Start, preprocessorTokens.rbegin()->m_Span.m_End }, preprocessorTokens[2].m_Span.m_Start, "Unused");
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
						default:
							addError(input.m_Span, input.m_Span.m_Start, "Expected string or identifier");
							continue;
						}

						if (!hasIncludedFile(include))
						{
							auto result = m_IncludeHandler(include);
							if (result.m_Status == EIncludeStatus::Failure)
							{
								addError(input.m_Span, input.m_Span.m_Start, fmt::format("File \"{}\" not found", Utils::EscapeString(include)));
								continue;
							}

							std::vector<Token> includedTokens = Tokenize(result.m_Source, { 0, 0, 0, m_IncludedFilenames.size(), m_IncludedFilenames.size() });

							itr                     = output.erase(itr);
							eraseDirective          = false;
							std::size_t startOffset = itr - output.begin();
							output.insert(itr, includedTokens.begin(), includedTokens.end());
							itr = output.begin() + startOffset;
							m_IncludedFilenames.emplace_back(std::move(include));
						}
					}
					else if (directive.m_Str == "define")
					{
						directiveHandled = true;
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
						directiveHandled = true;
						if (preprocessorTokens.size() < 2)
						{
							SourcePoint point = directive.m_Span.m_End;
							++point.m_Column;
							addError(SourceSpan { point, point }, point, "Expected identifier");
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
					else if (directive.m_Str == "error")
					{
						directiveHandled = true;
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
						std::string error;

						switch (input.m_Class)
						{
						case ETokenClass::String:
							error = input.m_Str;
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
							error = (*macro)[0].m_Str;
							break;
						}
						default:
							addError(input.m_Span, input.m_Span.m_Start, "Expected string or identifier");
							continue;
						}

						addError(preprocessorSpan, directive.m_Span.m_Start, std::move(error));
					}
					else if (directive.m_Str == "warn")
					{
						directiveHandled = true;
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
						std::string warn;

						switch (input.m_Class)
						{
						case ETokenClass::String:
							warn = input.m_Str;
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
							warn = (*macro)[0].m_Str;
							break;
						}
						default:
							addError(input.m_Span, input.m_Span.m_Start, "Expected string or identifier");
							continue;
						}

						addWarning(preprocessorSpan, directive.m_Span.m_Start, std::move(warn));
					}
					else if (directive.m_Str == "line")
					{
						directiveHandled = true;
						if (preprocessorTokens.size() < 2)
						{
							SourcePoint point = directive.m_Span.m_End;
							++point.m_Column;
							addError(SourceSpan { point, point }, point, "Expected integer or identifier");
							continue;
						}

						auto&       lineNumber = preprocessorTokens[1];
						std::size_t ln         = 0;
						switch (lineNumber.m_Class)
						{
						case ETokenClass::Integer:
							ln = std::strtoull(lineNumber.m_Str.c_str(), nullptr, 10);
							break;
						case ETokenClass::BinaryInteger:
							ln = std::strtoull(lineNumber.m_Str.c_str() + 2, nullptr, 2);
							break;
						case ETokenClass::OctalInteger:
							ln = std::strtoull(lineNumber.m_Str.c_str() + 2, nullptr, 8);
							break;
						case ETokenClass::HexInteger:
							ln = std::strtoull(lineNumber.m_Str.c_str() + 2, nullptr, 16);
							break;
						case ETokenClass::Identifier:
						{
							auto macro = getMacro(lineNumber.m_Str);
							if (!macro)
							{
								addError(lineNumber.m_Span, lineNumber.m_Span.m_Start, "Macro has not been defined");
								continue;
							}
							if (macro->empty())
							{
								addWarning(lineNumber.m_Span, lineNumber.m_Span.m_Start, "Macro is empty, skipping include");
								continue;
							}

							auto& lnn = (*macro)[0];
							switch (lnn.m_Class)
							{
							case ETokenClass::Integer:
								ln = std::strtoull(lineNumber.m_Str.c_str(), nullptr, 10);
								break;
							case ETokenClass::BinaryInteger:
								ln = std::strtoull(lineNumber.m_Str.c_str() + 2, nullptr, 2);
								break;
							case ETokenClass::OctalInteger:
								ln = std::strtoull(lineNumber.m_Str.c_str() + 2, nullptr, 8);
								break;
							case ETokenClass::HexInteger:
								ln = std::strtoull(lineNumber.m_Str.c_str() + 2, nullptr, 16);
								break;
							default:
								addError(lineNumber.m_Span, lineNumber.m_Span.m_Start, "Macro requires first token to be an integer (Future: Evaluate all tokens first)");
								continue;
							}
							break;
						}
						default:
							addError(lineNumber.m_Span, lineNumber.m_Span.m_Start, "Expected integer or identifier");
							continue;
						}

						std::size_t file = directive.m_Span.m_Start.m_File;

						if (preprocessorTokens.size() > 2)
						{
							auto& filename = preprocessorTokens[2];

							std::string_view filenameStr;
							switch (filename.m_Class)
							{
							case ETokenClass::String:
								if (preprocessorTokens.size() > 3)
									addWarning(SourceSpan { preprocessorTokens[3].m_Span.m_Start, preprocessorTokens.rbegin()->m_Span.m_End }, preprocessorTokens[3].m_Span.m_Start, "Unused");
								filenameStr = filename.m_Str;
								break;
							case ETokenClass::Identifier:
							{
								auto macro = getMacro(filename.m_Str);
								if (!macro)
								{
									addError(filename.m_Span, filename.m_Span.m_Start, "Macro has not been defined");
									continue;
								}
								if (macro->empty())
								{
									addWarning(filename.m_Span, filename.m_Span.m_Start, "Macro is empty, skipping include");
									continue;
								}
								if ((*macro)[0].m_Class != ETokenClass::String)
								{
									addError(filename.m_Span, filename.m_Span.m_Start, "Macro requires first token to be a string (Future: Evaluate all tokens first)");
									continue;
								}
								filenameStr = (*macro)[0].m_Str;
								break;
							}
							default:
								addError(filename.m_Span, filename.m_Span.m_Start, "Expected string or identifier");
								continue;
							}

							auto itr2 = m_IncludedFilenames.begin();
							while (itr2 != m_IncludedFilenames.end() && *itr2 != filenameStr)
								++itr2;
							if (itr2 == m_IncludedFilenames.end())
							{
								file = m_IncludedFilenames.size();
								m_IncludedFilenames.emplace_back(filenameStr);
								itr2 = m_IncludedFilenames.end() - 1;
							}
							file = itr2 - m_IncludedFilenames.begin();
						}

						std::size_t baseFile = directive.m_Span.m_Start.m_File;
						std::size_t baseline = directive.m_Span.m_Start.m_Line + 2;

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
					}
					else if (directive.m_Str == "elif")
					{
						directiveHandled = true;
						addError(directive.m_Span, directive.m_Span.m_Start, "Unexpected directive");
						continue;
					}
					else if (directive.m_Str == "elifdef")
					{
						directiveHandled = true;
						addError(directive.m_Span, directive.m_Span.m_Start, "Unexpected directive");
						continue;
					}
					else if (directive.m_Str == "elifndef")
					{
						directiveHandled = true;
						addError(directive.m_Span, directive.m_Span.m_Start, "Unexpected directive");
						continue;
					}
					else if (directive.m_Str == "else")
					{
						directiveHandled = true;
						addError(directive.m_Span, directive.m_Span.m_Start, "Unexpected directive");
						continue;
					}
					else if (directive.m_Str == "endif")
					{
						directiveHandled = true;
						addError(directive.m_Span, directive.m_Span.m_Start, "Unexpected directive");
						continue;
					}
					else if (directive.m_Str == "if" ||
					         directive.m_Str == "ifdef" ||
					         directive.m_Str == "ifndef")
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
									auto preprocessorSpan2 = end->m_Span;

									std::vector<Token> preprocessorTokens2 = Tokenize(end->m_Str.substr(1), preprocessorSpan2.m_Start);
									if (preprocessorTokens2.empty() ||
									    preprocessorTokens2[0].m_Class != ETokenClass::Identifier)
									{
										++end;
										continue;
									}

									auto& directive2 = preprocessorTokens2[0].m_Str;
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
								SourcePoint point = output.rbegin()->m_Span.m_End;
								++point.m_Line;
								point.m_Column = 0;
								addWarning(SourceSpan { point, point }, point, state == 0 ? "Expected \"#elif\", \"#elifdef\", \"#elifndef\", \"#else\" or \"#endif\" directive" : (state == 1 ? "Expected \"#else\" or \"#endif\" directive" : "Expected \"#endif\" directive"));
								continue;
							}

							bool enable = false;

							do
							{
								auto               preprocessorSpanStart   = start->m_Span;
								std::vector<Token> preprocessorTokensStart = Tokenize(start->m_Str.substr(1), preprocessorSpanStart.m_Start);
								if (preprocessorTokensStart.empty() ||
								    preprocessorTokensStart[0].m_Class != ETokenClass::Identifier)
								{
									addError(preprocessorSpanStart, preprocessorSpanStart.m_Start, "Expected Identifier");
									continue;
								}

								auto& directiveStart = preprocessorTokensStart[0];

								if (directiveStart.m_Str == "if" ||
								    directiveStart.m_Str == "elif")
								{
									if (preprocessorTokensStart.size() < 2)
									{
										SourcePoint point = directiveStart.m_Span.m_End;
										++point.m_Column;
										addError(SourceSpan { point, point }, point, "Expected bool statement");
										continue;
									}

									auto& statement = preprocessorTokensStart[1];
									if (statement.m_Class != ETokenClass::Identifier &&
									    statement.m_Class != ETokenClass::Integer &&
									    statement.m_Class != ETokenClass::BinaryInteger &&
									    statement.m_Class != ETokenClass::OctalInteger &&
									    statement.m_Class != ETokenClass::HexInteger)
									{
										addError(statement.m_Span, statement.m_Span.m_Start, "Expected bool statement");
										continue;
									}

									switch (statement.m_Class)
									{
									case ETokenClass::Identifier:
										if (statement.m_Str == "false")
											enable = false;
										else if (statement.m_Str == "true")
											enable = true;
										else
											addError(statement.m_Span, statement.m_Span.m_Start, "Expected bool statement");
										break;
									case ETokenClass::Integer:
									{
										char*        endStr;
										std::int64_t integer = std::strtoll(statement.m_Str.c_str(), &endStr, 10);
										enable               = integer != 0;
										break;
									}
									case ETokenClass::BinaryInteger:
									{
										char*        endStr;
										std::int64_t integer = std::strtoll(statement.m_Str.c_str() + 2, &endStr, 2);
										enable               = integer != 0;
										break;
									}
									case ETokenClass::OctalInteger:
									{
										char*        endStr;
										std::int64_t integer = std::strtoll(statement.m_Str.c_str() + 2, &endStr, 8);
										enable               = integer != 0;
										break;
									}
									case ETokenClass::HexInteger:
									{
										char*        endStr;
										std::int64_t integer = std::strtoll(statement.m_Str.c_str() + 2, &endStr, 16);
										enable               = integer != 0;
										break;
									}
									default:
										addError(statement.m_Span, statement.m_Span.m_Start, "Expected bool or integer");
										continue;
									}
								}
								else if (directiveStart.m_Str == "ifdef" ||
								         directiveStart.m_Str == "elifdef")
								{
									if (preprocessorTokensStart.size() < 2)
									{
										SourcePoint point = directiveStart.m_Span.m_End;
										++point.m_Column;
										addError(SourceSpan { point, point }, point, "Expected identifier");
										continue;
									}
									else if (preprocessorTokensStart.size() > 2)
									{
										addWarning(SourceSpan { preprocessorTokensStart[2].m_Span.m_Start, preprocessorTokensStart.rbegin()->m_Span.m_End }, preprocessorTokensStart[2].m_Span.m_Start, "Unused");
									}

									auto& name = preprocessorTokensStart[1];
									if (name.m_Class != ETokenClass::Identifier)
									{
										addError(name.m_Span, name.m_Span.m_Start, "Expected identifier");
										continue;
									}

									enable = hasMacro(name.m_Str);
								}
								else if (directiveStart.m_Str == "ifndef" ||
								         directiveStart.m_Str == "elifndef")
								{
									if (preprocessorTokensStart.size() < 2)
									{
										SourcePoint point = directiveStart.m_Span.m_End;
										++point.m_Column;
										addError(SourceSpan { point, point }, point, "Expected identifier");
										continue;
									}
									else if (preprocessorTokensStart.size() > 2)
									{
										addWarning(SourceSpan { preprocessorTokensStart[2].m_Span.m_Start, preprocessorTokensStart.rbegin()->m_Span.m_End }, preprocessorTokensStart[2].m_Span.m_Start, "Unused");
									}

									auto& name = preprocessorTokensStart[1];
									if (name.m_Class != ETokenClass::Identifier)
									{
										addError(name.m_Span, name.m_Span.m_Start, "Expected identifier");
										continue;
									}

									enable = !hasMacro(name.m_Str);
								}
								else if (directiveStart.m_Str == "else")
								{
									enable = state != 2;
								}
							} while (false);

							do
							{
								auto               preprocessorSpanEnd   = end->m_Span;
								std::vector<Token> preprocessorTokensEnd = Tokenize(end->m_Str.substr(1), preprocessorSpanEnd.m_Start);
								if (preprocessorTokensEnd.empty() ||
								    preprocessorTokensEnd[0].m_Class != ETokenClass::Identifier)
								{
									addError(preprocessorSpanEnd, preprocessorSpanEnd.m_Start, "Expected Identifier");
									continue;
								}

								auto& directiveEnd = preprocessorTokensEnd[0];
								if (directiveEnd.m_Str == "else")
									++state;
								else if (directiveEnd.m_Str == "endif")
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
						addWarning(directive.m_Span, directive.m_Span.m_Start, fmt::format("Unknown preprocessor directive \"{}\"", Utils::EscapeString(directive.m_Str)));
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

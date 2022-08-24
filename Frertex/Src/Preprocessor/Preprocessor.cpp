#include "Frertex/Preprocessor/Preprocessor.h"
#include "Frertex/Tokenizer/Tokenizer.h"
#include "Frertex/Utils/Profiler.h"

#include <fmt/format.h>

#include <filesystem>

namespace Frertex::Preprocessor
{
	State::State(Source::Sources*           sources,
	             Source::IncludePathHandler includePathHandler,
	             Source::IncludeHandler     includeHandler,
	             Message::MessageHandler    messageHandler)
	    : m_Sources(sources),
	      m_IncludePathHandler(includePathHandler),
	      m_IncludeHandler(includeHandler),
	      m_MessageHandler(messageHandler)
	{
	}

	std::vector<Tokenizer::Token> State::preprocess(Utils::CopyMovable<std::vector<Tokenizer::Token>>&& tokens)
	{
		PROFILE_FUNC;

		m_Tokens = tokens.get();

		for (auto itr = m_Tokens.begin(); itr != m_Tokens.end();)
		{
			std::size_t tokenIndex = itr - m_Tokens.begin();
			if (itr->m_Class == Tokenizer::ETokenClass::Preprocessor)
			{
				m_EraseDirective = true;

				do
				{
					std::vector<Tokenizer::Token> preprocessorTokens = Tokenizer::Tokenize(m_Sources->getSource(itr->m_SourceID), itr->m_Index + 1, itr->m_Index + itr->m_Length - 1);

					if (preprocessorTokens.empty())
						continue;

					auto& directive = preprocessorTokens[0];
					if (directive.m_Class != Tokenizer::ETokenClass::Identifier)
					{
						auto span = directive.getSpan(m_Sources);
						reportError(tokenIndex, tokenIndex, { span }, span.start(), "Expected identifier");
						continue;
					}

					auto directiveStr = directive.getView(m_Sources);
					if (directiveStr == "pragma")
						itr = handlePragma(itr, tokenIndex, preprocessorTokens);
					else if (directiveStr == "include")
						itr = handleInclude(itr, tokenIndex, preprocessorTokens);
					else if (directiveStr == "error")
						itr = handleError(itr, tokenIndex, preprocessorTokens);
					else if (directiveStr == "warn")
						itr = handleWarn(itr, tokenIndex, preprocessorTokens);
					else
						reportWarning(tokenIndex, tokenIndex, { directive.getSpan(m_Sources) }, directive.getStart(m_Sources), fmt::format("Unknown preprocessor directive \"{}\"", Utils::EscapeString(directiveStr)));
				} while (false);

				if (m_EraseDirective)
					itr = m_Tokens.erase(itr);
			}
			else if (itr->m_Class == Tokenizer::ETokenClass::Comment ||
			         itr->m_Class == Tokenizer::ETokenClass::MultilineComment)
			{
				itr = m_Tokens.erase(itr);
			}
			else
			{
				++itr;
			}
		}

		return std::move(m_Tokens);
	}

	State::TokenItr State::handlePragma(TokenItr itr, std::size_t tokenIndex, const std::vector<Tokenizer::Token>& tokens)
	{
		PROFILE_FUNC;

		if (tokens.size() < 2)
		{
			auto point = tokens.back().getEnd(m_Sources);
			reportError(tokenIndex, tokenIndex, std::vector<Source::SourceSpan> {}, point, "Expected string or identifier");
			return itr;
		}

		auto& option = tokens[1];
		if (option.m_Class != Tokenizer::ETokenClass::Identifier)
		{
			auto span = option.getSpan(m_Sources);
			reportError(tokenIndex, tokenIndex, { span }, span.start(), "Expected identifier");
			return itr;
		}

		auto optionStr = option.getView(m_Sources);
		if (optionStr == "once")
		{
			setIncludeOnce(m_Sources->getSourceName(itr->m_SourceID));
		}
		else
		{
			reportWarning(tokenIndex, tokenIndex, { option.getSpan(m_Sources) }, option.getStart(m_Sources), fmt::format("Unknown pragma directive \"{}\"", Utils::EscapeString(optionStr)));
		}

		return itr;
	}

	State::TokenItr State::handleInclude(TokenItr itr, std::size_t tokenIndex, const std::vector<Tokenizer::Token>& tokens)
	{
		PROFILE_FUNC;

		if (tokens.size() < 2)
		{
			auto point = tokens.back().getEnd(m_Sources);
			reportError(tokenIndex, tokenIndex, std::vector<Source::SourceSpan> {}, point, "Expected string");
			return itr;
		}
		if (tokens.size() > 2)
		{
			std::vector<Source::SourceSpan> spans;
			for (auto itr2 = tokens.begin() + 2; itr2 != tokens.end(); ++itr2)
				spans.emplace_back(itr2->getSpan(m_Sources));
			reportWarning(tokenIndex, tokenIndex, std::move(spans), tokens[2].getStart(m_Sources), "Unused");
		}

		auto& input = tokens[1];
		if (input.m_Class != Tokenizer::ETokenClass::String)
		{
			auto span = input.getSpan(m_Sources);
			reportError(tokenIndex, tokenIndex, { span }, span.start(), "Expected string or identifier");
			return itr;
		}

		auto include  = input.getView(m_Sources);
		auto filepath = m_IncludePathHandler(include, std::filesystem::path { m_Sources->getSourceName(itr->m_SourceID) }.parent_path().string());
		if (filepath.empty())
		{
			auto span = input.getSpan(m_Sources);
			reportError(tokenIndex, tokenIndex, { span }, span.start(), fmt::format("File \"{}\" was unreable or not found", Utils::EscapeString(include)));
			return itr;
		}
		else if (!onlyIncludeOnce(filepath))
		{
			auto newSource = m_Sources->getSource(filepath.string());
			if (!newSource)
			{
				auto result = m_IncludeHandler(filepath);
				if (result.m_Status == Source::EIncludeStatus::Failure)
				{
					auto span = input.getSpan(m_Sources);
					reportError(tokenIndex, tokenIndex, { span }, span.start(), fmt::format("File \"{}\" was unreable or not found", Utils::EscapeString(include)));
					return itr;
				}

				auto newFileID = m_Sources->addSource(std::move(result));
				newSource      = m_Sources->getSource(newFileID);
			}
			auto includedTokens = Tokenizer::Tokenize(newSource);

			m_EraseDirective        = false;
			itr                     = m_Tokens.erase(itr);
			std::size_t startOffset = itr - m_Tokens.begin();
			m_Tokens.insert(itr, includedTokens.begin(), includedTokens.end());
			itr = m_Tokens.begin() + startOffset;
		}

		return itr;
	}

	State::TokenItr State::handleError(TokenItr itr, std::size_t tokenIndex, const std::vector<Tokenizer::Token>& tokens)
	{
		PROFILE_FUNC;

		if (tokens.size() < 2)
		{
			auto point = tokens.back().getEnd(m_Sources);
			reportError(tokenIndex, tokenIndex, std::vector<Source::SourceSpan> {}, point, "Expected string");
			return itr;
		}
		else if (tokens.size() > 2)
		{
			std::vector<Source::SourceSpan> spans;
			for (auto itr2 = tokens.begin() + 2; itr2 != tokens.end(); ++itr2)
				spans.emplace_back(itr2->getSpan(m_Sources));
			reportWarning(tokenIndex, tokenIndex, std::move(spans), tokens[2].getStart(m_Sources), "Unused");
		}

		auto& error = tokens[1];
		if (error.m_Class != Tokenizer::ETokenClass::String)
		{
			auto span = error.getSpan(m_Sources);
			reportError(tokenIndex, tokenIndex, { span }, span.start(), "Expected string");
			return itr;
		}

		auto message = error.getView(m_Sources);
		reportError(tokenIndex, tokenIndex, { error.getSpan(m_Sources) }, error.getStart(m_Sources), message);
		return itr;
	}

	State::TokenItr State::handleWarn(TokenItr itr, std::size_t tokenIndex, const std::vector<Tokenizer::Token>& tokens)
	{
		PROFILE_FUNC;

		if (tokens.size() < 2)
		{
			auto point = tokens.back().getEnd(m_Sources);
			reportError(tokenIndex, tokenIndex, std::vector<Source::SourceSpan> {}, point, "Expected string");
			return itr;
		}
		else if (tokens.size() > 2)
		{
			std::vector<Source::SourceSpan> spans;
			for (auto itr2 = tokens.begin() + 2; itr2 != tokens.end(); ++itr2)
				spans.emplace_back(itr2->getSpan(m_Sources));
			reportWarning(tokenIndex, tokenIndex, std::move(spans), tokens[2].getStart(m_Sources), "Unused");
		}

		auto& warning = tokens[1];
		if (warning.m_Class != Tokenizer::ETokenClass::String)
		{
			auto span = warning.getSpan(m_Sources);
			reportError(tokenIndex, tokenIndex, { span }, span.start(), "Expected string");
			return itr;
		}

		auto message = warning.getView(m_Sources);
		reportWarning(tokenIndex, tokenIndex, { warning.getSpan(m_Sources) }, warning.getStart(m_Sources), message);
		return itr;
	}

	void State::reportMessage(Message::EMessageType type, std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		m_MessageHandler(m_Sources, m_Tokens, { type, startToken, endToken + 1, spans.get(), point, message.get() });
	}

	bool State::onlyIncludeOnce(const std::filesystem::path& filepath) const
	{
		for (auto& path : m_IncludeOnceFiles)
			if (path == filepath)
				return true;
		return false;
	}

	void State::setIncludeOnce(const std::filesystem::path& filepath)
	{
		m_IncludeOnceFiles.emplace_back(filepath);
	}
} // namespace Frertex::Preprocessor
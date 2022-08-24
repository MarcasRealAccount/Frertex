#pragma once

#include "Frertex/Message.h"
#include "Frertex/Source.h"
#include "Frertex/Tokenizer/Token.h"

#include <filesystem>
#include <vector>

namespace Frertex::Preprocessor
{
	class State
	{
	public:
		using TokenItr = std::vector<Tokenizer::Token>::iterator;

	public:
		State(Source::Sources* sources, Source::IncludePathHandler includePathHandler, Source::IncludeHandler includeHandler, Message::MessageHandler messageHandler);

		std::vector<Tokenizer::Token> preprocess(Utils::CopyMovable<std::vector<Tokenizer::Token>>&& tokens);

		auto getSources() const { return m_Sources; }
		auto errored() const { return m_Errored; }

	private:
		TokenItr handlePragma(TokenItr itr, std::size_t tokenIndex, const std::vector<Tokenizer::Token>& tokens);
		TokenItr handleInclude(TokenItr itr, std::size_t tokenIndex, const std::vector<Tokenizer::Token>& tokens);
		TokenItr handleError(TokenItr itr, std::size_t tokenIndex, const std::vector<Tokenizer::Token>& tokens);
		TokenItr handleWarn(TokenItr itr, std::size_t tokenIndex, const std::vector<Tokenizer::Token>& tokens);

		void reportMessage(Message::EMessageType type, std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message);
		void reportWarning(std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message) { reportMessage(Message::EMessageType::Warning, startToken, endToken, spans.get(), point, message.get()); }
		void reportError(std::size_t startToken, std::size_t endToken, Utils::CopyMovable<std::vector<Source::SourceSpan>>&& spans, Source::SourcePoint point, Utils::CopyMovable<std::string>&& message)
		{
			m_Errored = true;
			reportMessage(Message::EMessageType::Error, startToken, endToken, spans.get(), point, message.get());
		}

		bool onlyIncludeOnce(const std::filesystem::path& filepath) const;
		void setIncludeOnce(const std::filesystem::path& filepath);

	private:
		Source::Sources* m_Sources;

		Source::IncludePathHandler m_IncludePathHandler;
		Source::IncludeHandler     m_IncludeHandler;

		std::vector<std::filesystem::path> m_IncludeOnceFiles;

		Message::MessageHandler m_MessageHandler;
		bool                    m_Errored = false;

		std::vector<Tokenizer::Token> m_Tokens;
		bool                          m_EraseDirective = false;
	};
} // namespace Frertex::Preprocessor
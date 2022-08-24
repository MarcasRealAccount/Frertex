#pragma once

#include "Message.h"
#include "Token.h"
#include "Utils/Utils.h"

#include <unordered_map>
#include <vector>

namespace Frertex
{
	class Preprocessor
	{
	public:
		Preprocessor(Sources* sources, IncludeHandler includeHandler);

		std::vector<Token> process(Utils::CopyMovable<std::vector<Token>>&& tokens);

		void addMacro(Utils::CopyMovable<std::string>&& name);
		void addMacro(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& value);
		void addMacro(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::vector<Token>>&& value);
		void removeMacro(std::string_view name);

		bool                hasMacro(std::string_view name);
		std::vector<Token>* getMacro(std::string_view name);

		bool hasIncludedFile(std::string_view filename);

		void addWarning(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message);
		void addError(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message);
		void addWarning(const Token& token, std::size_t point, Utils::CopyMovable<std::string>&& message)
		{
			addWarning(token.m_FileID, token.m_SourceID, token.m_Index, token.m_Length, point, message.get());
		}
		void addError(const Token& token, std::size_t point, Utils::CopyMovable<std::string>&& message)
		{
			addError(token.m_FileID, token.m_SourceID, token.m_Index, token.m_Length, point, message.get());
		}

		auto  getSources() const { return m_Sources; }
		auto& getMacros() const { return m_Macros; }
		auto& getMessages() const { return m_Messages; }

	private:
		Sources* m_Sources;

		std::unordered_map<std::string, std::vector<Token>> m_Macros;

		std::vector<std::string> m_IncludedFilenames;
		IncludeHandler           m_IncludeHandler;

		std::vector<Message> m_Messages;
	};
} // namespace Frertex
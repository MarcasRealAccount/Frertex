#pragma once

#include "Message.h"
#include "Token.h"
#include "Utils/Utils.h"

#include <unordered_map>
#include <vector>

namespace Frertex
{
	enum class EIncludeStatus
	{
		Failure = 0,
		Success
	};

	struct IncludeData
	{
		EIncludeStatus m_Status;
		std::string    m_Source;
	};

	using IncludeHandler = IncludeData (*)(std::string_view filename);

	class Preprocessor
	{
	public:
		Preprocessor(IncludeHandler includeHandler);

		std::vector<Token> process(Utils::CopyMovable<std::vector<Token>>&& tokens, std::string_view filename);

		void addMacro(Utils::CopyMovable<std::string>&& name);
		void addMacro(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::string>&& value);
		void addMacro(Utils::CopyMovable<std::string>&& name, Utils::CopyMovable<std::vector<Token>>&& value);
		void removeMacro(std::string_view name);

		bool                hasMacro(std::string_view name);
		std::vector<Token>* getMacro(std::string_view name);

		void addWarning(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message);
		void addError(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message);

		bool hasIncludedFile(std::string_view filename);

		auto& getMacros() const { return m_Macros; }
		auto& getIncludeFilenames() const { return m_IncludedFilenames; }
		auto& getMessages() const { return m_Messages; }

	private:
		std::unordered_map<std::string, std::vector<Token>> m_Macros;

		std::vector<std::string> m_IncludedFilenames;
		IncludeHandler           m_IncludeHandler;

		std::vector<Message> m_Messages;
	};
} // namespace Frertex
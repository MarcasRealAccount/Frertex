#include "Utils/Core.h"

#include <Frertex/Preprocessor.h>
#include <Frertex/Tokenizer.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Frertex::IncludeData ReadIncludedFile(std::string_view filename)
{
	std::ifstream file { std::string { filename }, std::ios::ate };
	if (file)
	{
		std::string source;
		source.resize(file.tellg());
		file.seekg(0);
		file.read(source.data(), source.size());
		file.close();
		return { Frertex::EIncludeStatus::Success, std::move(source) };
	}
	return {};
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	auto                  tokens = Frertex::Tokenize(ReadIncludedFile("Test.frer").m_Source);
	Frertex::Preprocessor preprocessor { &ReadIncludedFile };
	tokens                  = preprocessor.process(std::move(tokens), "Test.frer");
	auto& messages          = preprocessor.getMessages();
	auto& includedFilenames = preprocessor.getIncludeFilenames();
	bool  errored           = false;
	if (!messages.empty())
	{
		for (auto& message : messages)
		{
			std::string output = Frertex::FormatMessage(
			    message,
			    includedFilenames,
			    [](std::string_view filename, Frertex::SourcePoint line) -> std::string
			    {
				    auto result = ReadIncludedFile(filename);
				    if (result.m_Status == Frertex::EIncludeStatus::Failure)
					    return {};
				    std::size_t lineStart = line.m_Index;
				    while (lineStart > 0 && result.m_Source[lineStart - 1] != '\n')
					    --lineStart;
				    std::size_t lineEnd = line.m_Index;
				    while (lineEnd < result.m_Source.size() && result.m_Source[lineEnd] != '\n')
					    ++lineEnd;
				    return result.m_Source.substr(lineStart, lineEnd - lineStart);
			    });
			switch (message.m_Type)
			{
			case Frertex::EMessageType::Warning:
				std::cout << output << '\n';
				break;
			case Frertex::EMessageType::Error:
				errored = true;
				std::cerr << output << '\n';
				break;
			}
		}
		if (errored)
			return 1;
	}
	for (auto& token : tokens)
		std::cout << token << '\n';

	std::ostringstream   str;
	Frertex::ETokenClass previousClass = Frertex::ETokenClass::Unknown;
	std::size_t          indents       = 0;
	for (auto& token : tokens)
	{
		if (previousClass == Frertex::ETokenClass::Identifier)
		{
			if (token.m_Class == Frertex::ETokenClass::Symbol)
			{
				if (token.m_Str == "_" ||
				    token.m_Str == "{" ||
				    token.m_Str == "=")
					str << ' ';
			}
			else
			{
				str << ' ';
			}
		}
		else if (token.m_Class == Frertex::ETokenClass::Symbol)
		{
			if (token.m_Str == "}")
			{
				--indents;
				str.seekp(-1, std::ios::cur);
			}
		}

		str << token.m_Str;

		if (token.m_Class == Frertex::ETokenClass::Symbol)
		{
			if (token.m_Str == "," ||
			    token.m_Str == ":" ||
			    token.m_Str == "=")
			{
				str << ' ';
			}
			else if (token.m_Str == ";" ||
			         token.m_Str == "}")
			{
				str << '\n'
				    << std::string(indents, '\t');
			}
			else if (token.m_Str == "{")
			{
				++indents;
				str << '\n'
				    << std::string(indents, '\t');
			}
		}
		else if (token.m_Class == Frertex::ETokenClass::Preprocessor)
		{
			str << '\n'
			    << std::string(indents, '\t');
		}

		previousClass = token.m_Class;
	}
	std::cout << str.str() << '\n';
}
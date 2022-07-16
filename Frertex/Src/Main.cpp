#include "Utils/Core.h"

#include <Frertex/Preprocessor.h>
#include <Frertex/Tokenizer.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string ReadIncludedFile(std::string_view filename)
{
	std::string   source;
	std::ifstream file { std::string { filename }, std::ios::ate };
	if (file)
	{
		source.resize(file.tellg());
		file.seekg(0);
		file.read(source.data(), source.size());
		file.close();
	}
	return source;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	std::vector<Frertex::Message> messages;
	std::vector<std::string>      includedFilenames;

	auto tokens = Frertex::Tokenize(ReadIncludedFile("Test.frer"));
	tokens      = Frertex::Preprocess(std::move(tokens), &ReadIncludedFile, "Test.frer", includedFilenames, messages);
	if (!messages.empty())
	{
		for (auto& message : messages)
		{
			std::string output = Frertex::FormatMessage(
			    message,
			    includedFilenames,
			    [](std::string_view filename, Frertex::SourcePoint line) -> std::string
			    {
				    std::string str       = ReadIncludedFile(filename);
				    std::size_t lineStart = line.m_Index;
				    while (lineStart > 0 && str[lineStart - 1] != '\n')
					    --lineStart;
				    std::size_t lineEnd = line.m_Index;
				    while (lineEnd < str.size() && str[lineEnd] != '\n')
					    ++lineEnd;
				    return str.substr(lineStart, lineEnd - lineStart);
			    });
			switch (message.m_Type)
			{
			case Frertex::EMessageType::Warning:
				std::cout << output << '\n';
				break;
			case Frertex::EMessageType::Error:
				std::cerr << output << '\n';
				break;
			}
		}
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
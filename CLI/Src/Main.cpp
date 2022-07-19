#include "Utils/Core.h"

#include <Frertex/Compiler.h>
#include <Frertex/FIL.h>
#include <Frertex/Lexer.h>
#include <Frertex/Preprocessor.h>
#include <Frertex/Tokenizer.h>
#include <Frertex/Utils/Profiler.h>
#include <Frertex/Utils/Utils.h>

#include <fmt/format.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#if BUILD_IS_SYSTEM_WINDOWS
#include <Windows.h>
#undef FormatMessage
#endif

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

std::string ReadFileLine(std::string_view filename, Frertex::SourcePoint line)
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
}

std::size_t UTF8Codepoints(const std::string& str)
{
	auto itr = str.begin();
	auto end = str.end();

	std::size_t count = 0;
	while (itr != end)
	{
		std::uint8_t c = *itr;
		if (c & 0b1000'0000U)
		{
			// UTF8 extending
			if ((c & 0b0111'0000U) == 0b0111'0000U)
				itr += 4;
			else if ((c & 0b0110'0000U) == 0b0110'0000U)
				itr += 3;
			else
				itr += 2;
		}
		else
		{
			// ASCII
			++itr;
		}
		++count;
	}
	return count;
}

void PrintASTNode(const Frertex::ASTNode& node, std::vector<std::vector<std::string>>& lines, std::vector<bool>& layers, bool end = true)
{
	{
		std::vector<std::string> line;
		std::ostringstream       str;
		for (auto layer : layers)
		{
			if (layer)
				str << "\xE2\x94\x82 ";
			else
				str << "  ";
		}

		if (end)
			str << "\xE2\x94\x94\xE2\x94\x80";
		else
			str << "\xE2\x94\x9C\xE2\x94\x80";

		layers.emplace_back(!end);

		str << Frertex::ASTNodeTypeToString(node.getType());
		line.emplace_back(str.str());
		str         = {};
		auto& token = node.getToken();
		str << '(' << token.m_Span << ')';
		line.emplace_back(str.str());
		str = {};
		if (token.m_Span.m_Start.m_Line == token.m_Span.m_End.m_Line)
		{
			if (token.m_Str.find_first_of('\n') >= token.m_Str.size())
			{
				str << "= \"" << Frertex::Utils::EscapeString(token.m_Str) << '"';
				line.emplace_back(str.str());
			}
		}
		lines.emplace_back(std::move(line));
	}

	auto& children = node.getChildren();
	for (std::size_t i = 0; i < children.size(); ++i)
		PrintASTNode(children[i], lines, layers, i >= children.size() - 1);

	layers.pop_back();
}

void PrintAST(const Frertex::AST& ast)
{
	std::vector<std::vector<std::string>> lines;
	std::vector<bool>                     layers;
	PrintASTNode(*ast.getRoot(), lines, layers);

	std::vector<std::size_t> sizes;
	for (auto& line : lines)
	{
		if ((line.size() - 1) > sizes.size())
			sizes.resize(line.size() - 1, 0);

		for (std::size_t i = 0; i < line.size() - 1; ++i)
		{
			auto&       column     = line[i];
			std::size_t codepoints = UTF8Codepoints(column);
			if (codepoints > sizes[i])
				sizes[i] = codepoints;
		}
	}

	std::ostringstream str;
	for (auto& line : lines)
	{
		for (std::size_t i = 0; i < line.size(); ++i)
		{
			auto& column = line[i];
			str << column;
			if (i < line.size() - 1)
				str << std::string(sizes[i] - UTF8Codepoints(column) + 1, ' ');
		}
		str << '\n';
	}
	std::cout << str.str() << '\n';
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
#if BUILD_IS_SYSTEM_WINDOWS
	UINT defaultCP = GetConsoleOutputCP();
	SetConsoleOutputCP(65001);
#endif

	auto                  tokens = Frertex::Tokenize(ReadIncludedFile("Test.frer").m_Source);
	Frertex::Preprocessor preprocessor { &ReadIncludedFile };
	tokens = preprocessor.process(std::move(tokens), "Test.frer");

	bool errored = false;
	if (!preprocessor.getMessages().empty())
	{
		auto& includedFilenames = preprocessor.getIncludeFilenames();

		for (auto& message : preprocessor.getMessages())
		{
			std::string output = Frertex::FormatMessage(
			    message,
			    includedFilenames,
			    &ReadFileLine);
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
		{
#if BUILD_IS_SYSTEM_WINDOWS
			SetConsoleOutputCP(defaultCP);
#endif
			return 1;
		}
	}
	//for (auto& token : tokens)
	//	std::cout << token << '\n';
	//std::cout << '\n';

	//std::ostringstream   str;
	//Frertex::ETokenClass previousClass = Frertex::ETokenClass::Unknown;
	//std::size_t          indents       = 0;
	//for (auto& token : tokens)
	//{
	//	if (previousClass == Frertex::ETokenClass::Identifier)
	//	{
	//		if (token.m_Class == Frertex::ETokenClass::Symbol)
	//		{
	//			if (token.m_Str == "_" ||
	//			    token.m_Str == "{" ||
	//			    token.m_Str == "=")
	//				str << ' ';
	//		}
	//		else
	//		{
	//			str << ' ';
	//		}
	//	}
	//	else if (token.m_Class == Frertex::ETokenClass::Symbol)
	//	{
	//		if (token.m_Str == "}")
	//		{
	//			--indents;
	//			str.seekp(-1, std::ios::cur);
	//		}
	//	}

	//	str << token.m_Str;

	//	if (token.m_Class == Frertex::ETokenClass::Symbol)
	//	{
	//		if (token.m_Str == "," ||
	//		    token.m_Str == ":" ||
	//		    token.m_Str == "=")
	//		{
	//			str << ' ';
	//		}
	//		else if (token.m_Str == ";" ||
	//		         token.m_Str == "}")
	//		{
	//			str << '\n'
	//			    << std::string(indents, '\t');
	//		}
	//		else if (token.m_Str == "{")
	//		{
	//			++indents;
	//			str << '\n'
	//			    << std::string(indents, '\t');
	//		}
	//	}
	//	else if (token.m_Class == Frertex::ETokenClass::Preprocessor)
	//	{
	//		str << '\n'
	//		    << std::string(indents, '\t');
	//	}

	//	previousClass = token.m_Class;
	//}
	//std::cout << str.str() << "\n";

	Frertex::Lexer lexer {};
	auto           ast = lexer.lex(std::move(tokens));
	if (!lexer.getMessages().empty())
	{
		auto& includedFilenames = preprocessor.getIncludeFilenames();

		for (auto& message : lexer.getMessages())
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
		{
#if BUILD_IS_SYSTEM_WINDOWS
			SetConsoleOutputCP(defaultCP);
#endif
			return 2;
		}
	}
	//if (!ast.getRoot())
	//	std::cout << "No Root\n";
	//else
	//	PrintAST(ast);
	//std::cout << '\n';

	Frertex::Compiler compiler { preprocessor.getIncludeFilenames() };
	auto              fil = compiler.compile(std::move(ast));
	if (!compiler.getMessages().empty())
	{
		auto& includedFilenames = compiler.getIncludedFilenames();

		for (auto& message : compiler.getMessages())
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
		{
#if BUILD_IS_SYSTEM_WINDOWS
			SetConsoleOutputCP(defaultCP);
#endif
			return 2;
		}
	}
	Frertex::WriteFILToFile("output.fil", fil);

	std::cout << Frertex::Utils::ProfilerToString();

#if BUILD_IS_SYSTEM_WINDOWS
	SetConsoleOutputCP(defaultCP);
#endif
}
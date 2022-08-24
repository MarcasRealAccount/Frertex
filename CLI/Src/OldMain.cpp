#include "Utils/Core.h"

#include <Frertex/Compiler.h>
#include <Frertex/FIL.h>
#include <Frertex/Lexer.h>
#include <Frertex/Preprocessor.h>
#include <Frertex/Tokenizer.h>
#include <Frertex/Transpilers/SPIRVTranspiler.h>
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

Frertex::IncludeData ReadIncludedFile(std::string_view filename, std::string_view originalFilename = ".")
{
	std::filesystem::path path = std::filesystem::relative(filename, originalFilename);
	std::ifstream         file { path, std::ios::ate };
	if (file)
	{
		std::string source;
		source.resize(file.tellg());
		file.seekg(0);
		file.read(source.data(), source.size());
		file.close();
		return { Frertex::EIncludeStatus::Success, std::move(source), std::filesystem::absolute(path).string() };
	}
	return {};
}

std::string ReadFileLine(Frertex::SourcePoint line, Frertex::Sources* sources, [[maybe_unused]] void* userData)
{
	if (!sources)
		return {};
	Frertex::Source* source = sources->getSource(line.m_SourceID);
	if (!source)
		return {};

	auto&       str       = source->getStr();
	std::size_t lineStart = str.find_last_of('\n', line.m_Index);
	if (lineStart >= str.size())
		lineStart = 0;
	std::size_t lineEnd = str.find_first_of('\n', line.m_Index);
	if (lineEnd >= str.size())
		lineEnd = str.size();
	return str.substr(lineStart, lineEnd - lineStart);
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

void PrintASTNode(const Frertex::ASTNode& node, Frertex::Sources* sources, std::vector<std::vector<std::string>>& lines, std::vector<bool>& layers, bool end = true)
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
		auto  span  = token.getSpan(*sources);
		str << '(' << span << ')';
		line.emplace_back(str.str());
		str = {};
		if (span.m_Start.m_Line == span.m_End.m_Line)
		{
			auto tokenStr = token.getView(*sources);
			if (tokenStr.find_first_of('\n') >= tokenStr.size())
			{
				str << "= \"" << Frertex::Utils::EscapeString(tokenStr) << '"';
				line.emplace_back(str.str());
			}
		}
		lines.emplace_back(std::move(line));
	}

	auto& children = node.getChildren();
	for (std::size_t i = 0; i < children.size(); ++i)
		PrintASTNode(children[i], sources, lines, layers, i >= children.size() - 1);

	layers.pop_back();
}

void PrintAST(const Frertex::AST& ast, Frertex::Sources* sources)
{
	std::vector<std::vector<std::string>> lines;
	std::vector<bool>                     layers;
	PrintASTNode(*ast.getRoot(), sources, lines, layers);

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

struct Timer
{
	using Clock = std::chrono::high_resolution_clock;

	void begin()
	{
		m_Start = Clock::now();
	}

	void end()
	{
		m_End = Clock::now();
	}

	float getTime() const
	{
		return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(m_End - m_Start).count();
	}

	std::string formatTime(Frertex::Utils::CopyMovable<std::string>&& name) const
	{
		return fmt::format("{} finished in {} ms\n", name.get(), getTime());
	}

	Clock::time_point m_Start, m_End;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
#if BUILD_IS_SYSTEM_WINDOWS
	UINT defaultCP = GetConsoleOutputCP();
	SetConsoleOutputCP(65001);
#endif

	Timer              timer {};
	std::ostringstream times;

	Frertex::Sources sources;

	auto mainSource = sources.addSource(ReadIncludedFile("Test.frer"));

	timer.begin();
	auto tokens = Frertex::Tokenize(sources.getSource(mainSource));
	timer.end();
	times << timer.formatTime("Tokenizer");

	timer.begin();
	Frertex::Preprocessor preprocessor { &sources, &ReadIncludedFile };
	tokens = preprocessor.process(std::move(tokens));
	timer.end();
	times << timer.formatTime("Preprocessor");

	bool errored = false;
	if (!preprocessor.getMessages().empty())
	{
		for (auto& message : preprocessor.getMessages())
		{
			std::string output = Frertex::FormatMessage(
			    message,
			    &ReadFileLine,
			    &sources);
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

	timer.begin();
	Frertex::Lexer lexer { &sources };
	auto           ast = lexer.lex(std::move(tokens));
	timer.end();
	times << timer.formatTime("Lexer");

	if (!lexer.getMessages().empty())
	{
		for (auto& message : lexer.getMessages())
		{
			std::string output = Frertex::FormatMessage(
			    message,
			    &ReadFileLine,
			    &sources);
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
	if (!ast.getRoot())
		std::cout << "No Root\n";
	else
		PrintAST(ast, &sources);
	std::cout << '\n';

	timer.begin();
	Frertex::Compiler compiler { &sources };
	auto              fil = compiler.compile(std::move(ast));
	timer.end();
	times << timer.formatTime("Compiler");

	if (!compiler.getMessages().empty())
	{
		for (auto& message : compiler.getMessages())
		{
			std::string output = Frertex::FormatMessage(
			    message,
			    &ReadFileLine,
			    &sources);
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
			return 3;
		}
	}
	Frertex::WriteFILToFile("Output.fil", fil);

	timer.begin();
	Frertex::Transpilers::SPIRV::SPIRVTranspiler transpiler { &sources };

	auto spirv = transpiler.transpile(std::move(fil));
	timer.end();
	times << timer.formatTime("SPIR-V Transpiler");

	if (!transpiler.getMessages().empty())
	{
		for (auto& message : transpiler.getMessages())
		{
			std::string output = Frertex::FormatMessage(
			    message,
			    &ReadFileLine,
			    &sources);
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
			return 4;
		}
	}
	Frertex::Transpilers::SPIRV::WriteSPIRVToFile("Output.spv", spirv);

	std::cout << Frertex::Utils::ProfilerToString();

	std::cout << times.str();

#if BUILD_IS_SYSTEM_WINDOWS
	SetConsoleOutputCP(defaultCP);
#endif
}

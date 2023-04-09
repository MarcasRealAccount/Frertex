#include "Utils/Core.h"

#include <Frertex/Compiler/Compiler.h>
#include <Frertex/Parser/Parser.h>
#include <Frertex/Preprocessor/Preprocessor.h>
#include <Frertex/Tokenizer/Tokenizer.h>
#include <Frertex/Transpilers/SPIRV/SPIRV.h>
#include <Frertex/Utils/Profiler.h>
#include <Frertex/Utils/Utils.h>

#include <fmt/format.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#if BUILD_IS_SYSTEM_WINDOWS
#include <Windows.h>
#undef FormatMessage
#endif

struct ConsoleOutputCPSetter
{
public:
	ConsoleOutputCPSetter()
	{
#if BUILD_IS_SYSTEM_WINDOWS
		m_DefaultCP = GetConsoleOutputCP();
		SetConsoleOutputCP(65001);
#endif
	}

	~ConsoleOutputCPSetter()
	{
#if BUILD_IS_SYSTEM_WINDOWS
		SetConsoleOutputCP(m_DefaultCP);
#endif
	}

private:
#if BUILD_IS_SYSTEM_WINDOWS
	UINT m_DefaultCP;
#endif
};

struct Timer
{
	using Clock = std::chrono::high_resolution_clock;

	void begin() { m_Start = Clock::now(); }
	void end() { m_End = Clock::now(); }

	float getTime() const
	{
		return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(m_End - m_Start).count();
	}

	std::string formatTime(Frertex::Utils::CopyMovable<std::string>&& name) const
	{
		return fmt::format("{} finished in {} ms\n", name.get(), getTime());
	}

private:
	Clock::time_point m_Start, m_End;
};

std::filesystem::path GetIncludedFilepath(std::string_view filename, const std::filesystem::path& originalFilepath = ".")
{
	PROFILE_FUNC;

	return std::filesystem::absolute(std::filesystem::relative(filename, originalFilepath));
}

Frertex::Source::IncludeData ReadIncludedFile(const std::filesystem::path& filepath)
{
	PROFILE_FUNC;

	std::ifstream file { filepath, std::ios::ate };
	if (file)
	{
		std::string source;
		source.resize(file.tellg());
		file.seekg(0);
		file.read(source.data(), source.size());
		file.close();
		return { Frertex::Source::EIncludeStatus::Success, std::move(source), std::filesystem::canonical(filepath) };
	}
	return {};
}

void MessageHandler(const Frertex::Source::Sources* sources, const std::vector<Frertex::Tokenizer::Token>& tokens, const Frertex::Message::Message& message)
{
	PROFILE_FUNC;

	std::string msg = message.format(sources, tokens);
	switch (message.m_Type)
	{
	case Frertex::Message::EMessageType::Warning:
		std::cout << msg << '\n';
		break;
	case Frertex::Message::EMessageType::Error:
		std::cerr << msg << '\n';
		break;
	}
}

void TranspilerMessageHandler(const Frertex::FIL::Binary& fil, const Frertex::Message::TranspilerMessage& message)
{
	PROFILE_FUNC;

	std::string msg = message.format(fil);
	switch (message.m_Type)
	{
	case Frertex::Message::EMessageType::Warning:
		std::cout << msg << '\n';
		break;
	case Frertex::Message::EMessageType::Error:
		std::cerr << msg << '\n';
		break;
	}
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

void PrintASTNode(const Frertex::AST::Node& node, const Frertex::Source::Sources* sources, std::vector<std::vector<std::string>>& lines, std::vector<bool>& layers, bool end = true)
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


		str << Frertex::AST::TypeToString(node.getType());
		line.emplace_back(str.str());
		str         = {};
		auto& token = node.getToken();
		auto  span  = token.getSpan(sources);
		str << '(' << Frertex::Source::SourceSpanToString(span) << ')';
		line.emplace_back(str.str());
		str = {};
		if (span.m_StartLine == span.m_EndLine)
		{
			auto tokenStr = token.getView(sources);
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

void PrintAST(const Frertex::AST::Node& ast, const Frertex::Source::Sources* sources)
{
	std::vector<std::vector<std::string>> lines;
	std::vector<bool>                     layers;
	PrintASTNode(ast, sources, lines, layers);

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
	ConsoleOutputCPSetter _consoleOutputCPSetter {};

	Timer              timer {};
	std::ostringstream times;

	Frertex::Source::Sources sources;

	auto mainSource = sources.addSource(ReadIncludedFile("Test.frer"));

	timer.begin();
	auto tokens = Frertex::Tokenizer::Tokenize(sources.getSource(mainSource));
	timer.end();
	times << timer.formatTime("Tokenizer");

	Frertex::Preprocessor::State preprocessorState { &sources, &GetIncludedFilepath, &ReadIncludedFile, &MessageHandler };
	timer.begin();
	tokens = preprocessorState.preprocess(std::move(tokens));
	timer.end();
	times << timer.formatTime("Preprocessor");
	if (preprocessorState.errored())
	{
		std::cout << Frertex::Utils::ProfilerToString();
		std::cout << times.str();
		return 1;
	}

	Frertex::Parser::State parserState { &sources, &MessageHandler };
	timer.begin();
	auto ast = parserState.parse(std::move(tokens));
	timer.end();
	times << timer.formatTime("Parser");
	if (parserState.errored())
	{
		std::cout << Frertex::Utils::ProfilerToString();
		std::cout << times.str();
		return 2;
	}

	Frertex::Compiler::State compilerState { &sources, &MessageHandler, parserState.moveTokens() };
	timer.begin();
	auto fil = compilerState.compile(std::move(ast));
	timer.end();
	times << timer.formatTime("Compiler");
	if (compilerState.errored())
	{
		std::cout << Frertex::Utils::ProfilerToString();
		std::cout << times.str();
		return 3;
	}
	Frertex::FIL::WriteToFile(fil, "Output.fil");

	Frertex::Transpilers::SPIRV::State spirvState { &TranspilerMessageHandler };
	timer.begin();
	auto spirv = spirvState.transpile(std::move(fil));
	timer.end();
	times << timer.formatTime("SPIR-V Transpiler");
	if (spirvState.errored())
	{
		std::cout << Frertex::Utils::ProfilerToString();
		std::cout << times.str();
		return 4;
	}
	Frertex::Transpilers::SPIRV::WriteToFile(spirv, "Output.spv");

	std::cout << Frertex::Utils::ProfilerToString();
	std::cout << times.str();
	return 0;
}
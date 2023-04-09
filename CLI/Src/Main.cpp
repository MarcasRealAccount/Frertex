#include <Frertex/Parser/Parser.h>
#include <Frertex/Tokenizer/Tokenizer.h>

#include <chrono>
#include <iostream>
#include <string>

#include <Build.h>

#include <fmt/format.h>

#if BUILD_IS_SYSTEM_WINDOWS
	#include <Windows.h>
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

std::string Escape(std::string_view escape)
{
	std::string result {};
	std::size_t offset      = 0;
	std::size_t nextQuote   = escape.find_first_of('\'', offset);
	std::size_t nextNewline = escape.find_first_of('\n', offset);
	while ((nextQuote != std::string_view::npos || nextNewline != std::string_view::npos) && offset < escape.size())
	{
		std::size_t closest = std::min(nextQuote, nextNewline);
		result              += escape.substr(offset, closest - offset);
		offset              = closest + 1;
		if (closest == nextQuote)
		{
			result    += "\\'";
			nextQuote = escape.find_first_of('\'', offset);
		}
		else
		{
			result      += "\\n";
			nextNewline = escape.find_first_of('\n', offset);
		}
	}
	if (offset < escape.size())
		result += escape.substr(offset);
	return result;
}

std::int32_t TimeRescale(std::int32_t scale)
{
	if (scale < -30)
		return -30;
	if (scale > 30)
		return 30;
	return static_cast<std::int32_t>(std::floorf(static_cast<float>(scale) / 3) * 3);
}

std::string_view TimeSuffix(std::int32_t scale)
{
	switch (scale)
	{
	case -30: return "qs";
	case -27: return "rs";
	case -24: return "ys";
	case -21: return "zs";
	case -18: return "as";
	case -15: return "fs";
	case -12: return "ps";
	case -9: return "ns";
	case -6: return "\xc2\xb5s";
	case -3: return "ms";
	case 0: return "s";
	case 3: return "ks";
	case 6: return "Ms";
	case 9: return "Gs";
	case 12: return "Ts";
	case 15: return "Ps";
	case 18: return "Es";
	case 21: return "Zs";
	case 24: return "Ys";
	case 27: return "Rs";
	case 30: return "Qs";
	default: return "?s";
	}
}

template <class Rep, class Period>
std::string PrettyDuration(std::chrono::duration<Rep, Period> duration)
{
	using Duration     = std::chrono::duration<double>;
	double       dur   = std::chrono::duration_cast<Duration>(duration).count();
	std::int32_t scale = TimeRescale(static_cast<std::int32_t>(std::floor(std::log10(dur))));
	dur                *= std::pow(10.0, -scale);
	if (dur > 999)
		return fmt::format("inf {:<2}", TimeSuffix(scale));
	else if (dur < 0.001)
		return fmt::format("000.000 {:<2}", TimeSuffix(scale));
	return fmt::format("{:>7.3f} {:<2}", dur, TimeSuffix(scale));
}

void PrintASTNode(const Frertex::AST::Node& node, std::string_view source, std::uint32_t depth = 0)
{
	if (depth > 0)
		std::cout << std::string(depth * 2, ' ');
	std::cout << Frertex::AST::TypeToString(node.Type) << "(" << node.Token.Start << " -> " << (node.Token.Start + node.Token.Length) << "): '" << Escape(source.substr(node.Token.Start, node.Token.Length)) << "'\n";
	for (auto& child : node.Children)
		PrintASTNode(child, source, depth + 1);
}

int main(int argc, char** argv)
{
	auto __cocps = ConsoleOutputCPSetter();

	std::string test = R"([[VertexShader]]
void Vert(in float4 inPosition,
          in float4 inNormal,
          [[Position]] out float4 outPosition,
          out float2 outUV)
{
}

[[FragmentShader]]
void Frag(in float2 inUV,
          out float4 outColor)
{
}
)";
	/*for (std::size_t i = 0; i < 16; ++i)
		test += test;*/

	using Clock    = std::chrono::high_resolution_clock;
	using Duration = std::chrono::duration<double>;

	std::cout << "Input (" << test.size() << "):\n";
	std::cout << "-- Tokenizer --\n";
	auto start = Clock::now();

	std::vector<Frertex::Tokenizer::Token> tokens;
	Frertex::Tokenizer::Tokenize(test.c_str(), test.size(), tokens);

	auto end = Clock::now();
	std::cout << "Total time:         " << PrettyDuration(end - start) << "\n";
	std::cout << "Avg time per char:  " << PrettyDuration(std::chrono::duration_cast<Duration>(end - start) / test.size()) << "\n";
	std::cout << "Avg time per token: " << PrettyDuration(std::chrono::duration_cast<Duration>(end - start) / tokens.size()) << "\n";
	std::cout << "Tokens (" << tokens.size() << "):\n";
	/*for (std::size_t i = 0; i < tokens.size(); ++i)
	{
		auto token = tokens[i];
		std::cout << Frertex::Tokenizer::TokenClassToString(token.Class) << " (" << token.Start << " -> " << (token.Start + token.Length - 1) << "): '" << Escape(test.substr(token.Start, token.Length)) << "'\n";
	}*/
	std::cout << "---------------\n";

	std::cout << "-- Parser --\n";
	start = Clock::now();

	Frertex::Parser::State parser { test };
	Frertex::AST::Node     rootASTNode = parser.Parse(tokens);

	end = Clock::now();
	std::cout << "Total time:         " << PrettyDuration(end - start) << "\n";
	std::cout << "Avg time per char:  " << PrettyDuration(std::chrono::duration_cast<Duration>(end - start) / test.size()) << "\n";
	// PrintASTNode(rootASTNode, test);
	std::cout << "------------\n";
}
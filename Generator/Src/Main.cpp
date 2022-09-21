//#include "SPIRVGenerator.h"
//#include "TokenizerLUTGenerator.h"
#include "Template.h"

#include <chrono>
#include <iostream>

int main()
{
	//GenerateSPIRV();
	//GenerateTokenizerLUT();

	TemplateEngine engine;

	TemplateEnvironment environment;

	auto arguments = environment.assignArrayMacro("ARGUMENTS");
	{
		auto index0 = arguments->assignStructMacro(0);
		index0->assignTextMacro("NAME", "executionModel");
		index0->assignTextMacro("TYPE", "EExecutionModel");
		index0->assignTextMacro("KIND", "ExecutionModel");
		index0->assignTextMacro("CATEGORY", "enum");
		index0->assignTextMacro("OPTIONAL", "");

		auto index1 = arguments->assignStructMacro(1);
		index1->assignTextMacro("NAME", "entryPoint");
		index1->assignTextMacro("TYPE", "std::uint32_t");
		index1->assignTextMacro("KIND", "IdRef");
		index1->assignTextMacro("CATEGORY", "id");
		index1->assignTextMacro("OPTIONAL", "");

		auto index2 = arguments->assignStructMacro(2);
		index2->assignTextMacro("NAME", "name");
		index2->assignTextMacro("TYPE", "std::string_view");
		index2->assignTextMacro("KIND", "LiteralString");
		index2->assignTextMacro("CATEGORY", "string");
		index2->assignTextMacro("OPTIONAL", "");

		auto index3 = arguments->assignStructMacro(3);
		index3->assignTextMacro("NAME", "interface");
		index3->assignTextMacro("TYPE", "std::uint32_t");
		index3->assignTextMacro("KIND", "IdRef");
		index3->assignTextMacro("CATEGORY", "id");
		index3->assignTextMacro("OPTIONAL", "*");
	}

	auto capabilities = environment.assignArrayMacro("CAPABILITIES");
	{
		capabilities->assignTextMacro(0, "A");
		capabilities->assignTextMacro(1, "B");
		capabilities->assignTextMacro(2, "C");
		capabilities->assignTextMacro(3, "D");
	}

	auto extensions = environment.assignArrayMacro("EXTENSIONS");
	{
		extensions->assignTextMacro(0, "A");
		extensions->assignTextMacro(1, "B");
		extensions->assignTextMacro(2, "C");
		extensions->assignTextMacro(3, "D");
	}

	environment.assignTextMacro("NAME", "OpEntryPoint");
	environment.assignUIntMacro("OPCODE", 15);

	std::cout << engine.executeTemplate("InstructionImpl.h", environment) << '\n';

	return 0;
}
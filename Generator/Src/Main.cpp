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

	std::cout << engine.executeTemplate("SPIRVTemplates.tmpl", environment) << '\n';

	return 0;
}
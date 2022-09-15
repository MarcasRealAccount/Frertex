#include "SPIRVGenerator.h"

#include <json/json.h>

#include <fstream>
#include <iostream>
#include <string>

struct SPIRVTag
{
	std::string m_Name;
	std::string m_DisplayName;
};

struct SPIRVOperand
{
	std::string m_Kind;
	std::string m_Name;
	std::string m_Quantifier;
};

struct SPIRVInstruction
{
	std::string               m_Name;
	std::uint32_t             m_OpCode;
	std::string               m_Tag;
	std::vector<SPIRVOperand> m_Operands;
	std::vector<std::string>  m_Capabilities;
	std::vector<std::string>  m_Extensions;
};

struct SPIRVEnumerantParameter
{
	std::string m_Kind;
};

struct SPIRVEnumerant
{
	std::string                          m_Enumerant;
	std::uint32_t                        m_Value;
	std::vector<std::string>             m_Capabilities;
	std::vector<std::string>             m_Extensions;
	std::vector<SPIRVEnumerantParameter> m_Parameters;
};

struct SPIRVOperandKind
{
	std::string                 m_Category;
	std::string                 m_Kind;
	std::vector<SPIRVEnumerant> m_Enumerants;
};

std::string EscapeString(std::string str)
{
	std::string output = str;

	for (std::size_t i = 0; i < output.size(); ++i)
	{
		switch (output[i])
		{
		case '\t':
			output[i] = 't';
			output.insert(output.begin() + i, '\\');
			++i;
			break;
		case '\n':
			output[i] = 'n';
			output.insert(output.begin() + i, '\\');
			++i;
			break;
		case '\v':
			output[i] = 'v';
			output.insert(output.begin() + i, '\\');
			++i;
			break;
		case '\f':
			output[i] = 'f';
			output.insert(output.begin() + i, '\\');
			++i;
			break;
		case '\r':
			output[i] = 'r';
			output.insert(output.begin() + i, '\\');
			++i;
			break;
		case '"':
			output[i] = '"';
			output.insert(output.begin() + i, '\\');
			++i;
			break;
		case '\\':
			output[i] = '\\';
			output.insert(output.begin() + i, '\\');
			++i;
			break;
		}
	}

	return output;
}

void GenerateSPIRV()
{
	// TODO(MarcasRealAccount): Download spirv.core.grammar.json from https://github.com/KhronosGroup/SPIRV-Headers/blob/master/include/spirv/unified1/spirv.core.grammar.json

	Json::Value root;
	{
		std::ifstream spirvSpec("spirv.core.grammar.json");
		if (!spirvSpec)
		{
			std::cerr << "Failed to open 'spirv.core.grammar.json'\n";
			return;
		}
		spirvSpec >> root;
	}

	std::uint32_t majorVersion, minorVersion, revision;
	majorVersion = root["major_version"].asUInt();
	minorVersion = root["minor_version"].asUInt();
	revision     = root["revision"].asUInt();

	std::cout << "Generating for: SPV" << majorVersion << "." << minorVersion << "." << revision << "\n";

	std::vector<SPIRVTag> tags;
	tags.reserve(root["instruction_printing_class"].size());
	for (const auto& entry : root["instruction_printing_class"])
	{
		auto& tag         = tags.emplace_back();
		tag.m_Name        = entry["tag"].asString();
		tag.m_DisplayName = entry["heading"].asString();
	}

	std::vector<SPIRVInstruction> instructions;
	instructions.reserve(root["instructions"].size());
	for (const auto& entry : root["instructions"])
	{
		auto& instruction    = instructions.emplace_back();
		instruction.m_Name   = entry["opname"].asString();
		instruction.m_OpCode = entry["opcode"].asUInt();
		instruction.m_Tag    = entry["class"].asString();
		instruction.m_Operands.reserve(entry["operands"].size());
		for (const auto& operand : entry["operands"])
		{
			auto& rand        = instruction.m_Operands.emplace_back();
			rand.m_Kind       = operand["kind"].asString();
			rand.m_Name       = operand["name"].asString();
			rand.m_Quantifier = operand["quantifier"].asString();
		}
		instruction.m_Capabilities.reserve(entry["capabilities"].size());
		for (const auto& cap : entry["capabilities"])
			instruction.m_Capabilities.emplace_back(cap.asString());
		instruction.m_Extensions.reserve(entry["extensions"].size());
		for (const auto& ext : entry["extensions"])
			instruction.m_Extensions.emplace_back(ext.asString());
	}

	std::vector<SPIRVOperandKind> operandKinds;
	operandKinds.reserve(root["operand_kinds"].size());
	for (const auto& entry : root["operand_kinds"])
	{
		auto& kind      = operandKinds.emplace_back();
		kind.m_Category = entry["category"].asString();
		kind.m_Kind     = entry["kind"].asString();
		kind.m_Enumerants.reserve(entry["enumerants"].size());
		for (const auto& enumerant : entry["enumerants"])
		{
			auto& en          = kind.m_Enumerants.emplace_back();
			en.m_Enumerant    = enumerant["enumerant"].asString();
			const auto& value = enumerant["value"];
			if (value.isIntegral())
				en.m_Value = value.asUInt();
			else
				en.m_Value = static_cast<std::uint32_t>(std::atoll(value.asCString()));
			en.m_Capabilities.reserve(enumerant["capabilities"].size());
			for (const auto& cap : entry["capabilities"])
				en.m_Capabilities.emplace_back(cap.asString());
			en.m_Extensions.reserve(enumerant["extensions"].size());
			for (const auto& cap : entry["extensions"])
				en.m_Extensions.emplace_back(cap.asString());
			en.m_Parameters.reserve(enumerant["parameters"].size());
			for (const auto& param : enumerant["parameters"])
			{
				auto& parameter  = en.m_Parameters.emplace_back();
				parameter.m_Kind = param["kind"].asString();
			}
		}
	}

	{
		std::ofstream enumerationsHeader("Inc/Frertex/Transpilers/SPIRV/Enumerations.h");
		std::ofstream enumerationsSource("Src/Frertex/Transpilers/SPIRV/Enumerations.cpp");
		if (enumerationsHeader && enumerationsSource)
		{
			enumerationsHeader << "#pragma once\n\n#include <cstdint>\n\nnamespace Frertex::Transpilers::SPIRV\n{\n";
			enumerationsSource << "#include \"Frertex/Transpilers/SPIRV/Enumerations.h\"\n\nnamespace Frertex::Transpilers::SPIRV\n{\n";
			bool addGap = false;
			for (auto& operandKind : operandKinds)
			{
				if (operandKind.m_Category != "ValueEnum")
					continue;

				if (addGap)
				{
					enumerationsHeader << "\n";
					enumerationsSource << "\n";
				}
				addGap = true;

				enumerationsHeader << "\tenum class E" << operandKind.m_Kind << " : std::uint32_t\n\t{";
				enumerationsSource << "\tvoid RequireCapExt" << operandKind.m_Kind << "(E" << operandKind.m_Kind << " value, std::vector<ECapability>& capabilities, std::vector<std::string>& extensions)\n\t{\n\t\tswitch (value)\n\t\t{\n";
				bool addComma = false;
				for (auto& enumerant : operandKind.m_Enumerants)
				{
					if (addComma)
						enumerationsHeader << ',';
					addComma = true;

					enumerationsHeader << "\n\t\t" << enumerant.m_Enumerant << " = " << enumerant.m_Value;
					if (!enumerant.m_Capabilities.empty() || !enumerant.m_Extensions.empty())
					{
						enumerationsSource << "\t\t\tcase E" << operandKind.m_Kind << "::" << enumerant.m_Enumerant << ":\n";
						for (auto& cap : enumerant.m_Capabilities)
							enumerationsSource << "\t\t\t\tcapabilities.emplace_back(ECapability::" << cap << ");\n";
						for (auto& ext : enumerant.m_Extensions)
							enumerationsSource << "\t\t\t\textensions.emplace_back(\"" << EscapeString(ext) << "\");\n";
						enumerationsSource << "\t\t\t\tbreak;\n";
					}
				}
				enumerationsHeader << "\t}\n\n\tvoid RequireCapExt" << operandKind.m_Kind << "(E" << operandKind.m_Kind << " value, std::vector<ECapability>& capabilities, std::vector<std::string>& extensions);\n";
				enumerationsSource << "\t\tdefault: break;\n\t}\n";
			}
			enumerationsHeader << "} // namespace Frertex::Transpilers::SPIRV";
			enumerationsSource << "} // namespace Frertex::Transpilers::SPIRV";
		}

		enumerationsHeader.close();
		enumerationsSource.close();
	}
}
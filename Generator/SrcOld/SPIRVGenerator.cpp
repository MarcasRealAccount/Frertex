#include "SPIRVGenerator.h"

#include <json/json.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

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

std::string SafeEnumerant(std::string&& str)
{
	std::string output = std::move(str);
	if (output.empty())
		return output;

	char f = output[0];
	if (f >= '0' && f <= '9')
		output = '_' + output;
	return output;
}

std::string SafeOperandName(std::string&& str, bool isMultiple)
{
	std::string output = std::move(str);

	std::erase_if(output,
	              [](char v) -> bool
	              {
		              return std::isspace(v) || v == '\'' || v == ',' || v == '+' || v == '.' || v == '~' || v == '<' || v == '>';
	              });

	if (isMultiple)
	{
		std::size_t firstIndex = 0;
		for (; !std::isdigit(output[firstIndex]) && firstIndex < output.size(); ++firstIndex)
			;
		if (firstIndex < output.size())
			output.erase(firstIndex);
		if (output.back() != 's')
			output += "s";
	}

	if (output[0] >= '0' && output[0] <= '9')
		output = "_" + output;

	if (output[0] >= 'A' && output[0] <= 'Z')
		output[0] -= 'A' - 'a';

	if (output == "default")
		output = "def";

	return output;
}

void HexRep(char (&arr)[5], std::uint16_t v)
{
	static constexpr char HexValues[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	arr[0] = HexValues[(v & 0xF000) >> 12];
	arr[1] = HexValues[(v & 0x0F00) >> 8];
	arr[2] = HexValues[(v & 0x00F0) >> 4];
	arr[3] = HexValues[v & 0x000F];
	arr[4] = '\0';
}

void HexRep(char (&arr)[10], std::uint32_t v)
{
	static constexpr char HexValues[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	arr[0] = HexValues[(v & 0xF000'0000) >> 28];
	arr[1] = HexValues[(v & 0x0F00'0000) >> 24];
	arr[2] = HexValues[(v & 0x00F0'0000) >> 20];
	arr[3] = HexValues[(v & 0x000F'0000) >> 16];
	arr[4] = '\'';
	arr[5] = HexValues[(v & 0x0000'F000) >> 12];
	arr[6] = HexValues[(v & 0x0000'0F00) >> 8];
	arr[7] = HexValues[(v & 0x0000'00F0) >> 4];
	arr[8] = HexValues[v & 0x0000'000F];
	arr[9] = '\0';
}

std::vector<SPIRVTag>         m_Tags;
std::vector<SPIRVInstruction> m_Instructions;
std::vector<SPIRVOperandKind> m_OperandKinds;

SPIRVOperandKind* GetKind(std::string_view kind)
{
	for (auto& operandKind : m_OperandKinds)
		if (operandKind.m_Kind == kind)
			return &operandKind;
	return nullptr;
}

SPIRVEnumerant* GetEnumerant(SPIRVOperandKind& kind, std::uint32_t value)
{
	for (auto& enumerant : kind.m_Enumerants)
		if (enumerant.m_Value == value)
			return &enumerant;
	return nullptr;
}

SPIRVEnumerant* GetEnumerant(SPIRVOperandKind& kind, std::string_view enumerant)
{
	for (auto& en : kind.m_Enumerants)
		if (en.m_Enumerant == enumerant)
			return &en;
	return nullptr;
}

void EnsureDirectories(const std::filesystem::path& path)
{
	if (path.has_extension())
		std::filesystem::create_directories(path.parent_path());
	else
		std::filesystem::create_directories(path);
}

void ReplaceEnumerants(std::string_view a, std::string_view b)
{
	for (auto& instruction : m_Instructions)
		for (auto& capability : instruction.m_Capabilities)
			if (capability == a)
				capability = b;

	for (auto& operandKind : m_OperandKinds)
		for (auto& enumerant : operandKind.m_Enumerants)
			for (auto& capability : enumerant.m_Capabilities)
				if (capability == a)
					capability = b;
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

	m_Tags.reserve(root["instruction_printing_class"].size());
	for (const auto& entry : root["instruction_printing_class"])
	{
		auto& tag         = m_Tags.emplace_back();
		tag.m_Name        = entry["tag"].asString();
		tag.m_DisplayName = entry["heading"].asString();
	}

	m_Instructions.reserve(root["instructions"].size());
	for (const auto& entry : root["instructions"])
	{
		auto& instruction    = m_Instructions.emplace_back();
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

	m_OperandKinds.reserve(root["operand_kinds"].size());
	for (const auto& entry : root["operand_kinds"])
	{
		auto& kind      = m_OperandKinds.emplace_back();
		kind.m_Category = entry["category"].asString();
		kind.m_Kind     = entry["kind"].asString();
		kind.m_Enumerants.reserve(entry["enumerants"].size());
		for (const auto& enumerant : entry["enumerants"])
		{
			auto& en       = kind.m_Enumerants.emplace_back();
			en.m_Enumerant = SafeEnumerant(enumerant["enumerant"].asString());

			const auto& value = enumerant["value"];
			if (value.isIntegral())
			{
				en.m_Value = value.asUInt();
			}
			else
			{
				const char* str = value.asCString();
				if (str[0] == '0' && str[1] == 'x')
					en.m_Value = static_cast<std::uint32_t>(std::strtoull(str + 2, nullptr, 16));
				else
					en.m_Value = static_cast<std::uint32_t>(std::strtoull(str, nullptr, 10));
			}

			en.m_Capabilities.reserve(enumerant["capabilities"].size());
			for (const auto& cap : enumerant["capabilities"])
				en.m_Capabilities.emplace_back(cap.asString());

			en.m_Extensions.reserve(enumerant["extensions"].size());
			for (const auto& cap : enumerant["extensions"])
				en.m_Extensions.emplace_back(cap.asString());

			en.m_Parameters.reserve(enumerant["parameters"].size());
			for (const auto& param : enumerant["parameters"])
			{
				auto& parameter  = en.m_Parameters.emplace_back();
				parameter.m_Kind = param["kind"].asString();
			}
		}
	}

	// Remove unnecessary stuff
	std::unordered_map<std::uint32_t, std::uint32_t> seenValues;
	for (auto itr = m_Instructions.begin(); itr != m_Instructions.end();)
	{
		const auto&   opname      = itr->m_Name;
		std::uint32_t implementer = 0;
		if (opname.ends_with("KHR"))
			implementer = 1;
		else if (opname.ends_with("EXT"))
			implementer = 2;
		else if (opname.ends_with("NV"))
			implementer = 3;
		else if (opname.ends_with("AMD"))
			implementer = 4;
		else if (opname.ends_with("INTEL"))
			implementer = 5;

		auto seenItr = seenValues.find(itr->m_OpCode);
		if (seenItr == seenValues.end())
		{
			seenValues.insert({ itr->m_OpCode, implementer });
			++itr;
			continue;
		}

		if (seenItr->second < implementer)
		{
			itr = m_Instructions.erase(itr);
		}
		else
		{
			std::size_t offset = itr - m_Instructions.begin();
			for (auto itr2 = m_Instructions.begin(); itr2 != m_Instructions.end(); ++itr2)
			{
				if (itr2->m_OpCode == itr->m_OpCode)
				{
					if (itr2 < itr)
						--offset;
					m_Instructions.erase(itr2);
					break;
				}
			}
			itr = m_Instructions.begin() + offset;
		}
	}

	for (auto& operandKind : m_OperandKinds)
	{
		std::unordered_map<std::uint32_t, std::uint32_t> seenValues;

		for (auto itr = operandKind.m_Enumerants.begin(); itr != operandKind.m_Enumerants.end();)
		{
			const auto&   enumerant   = itr->m_Enumerant;
			std::uint32_t implementer = 0;
			if (enumerant.ends_with("KHR"))
				implementer = 1;
			else if (enumerant.ends_with("EXT"))
				implementer = 2;
			else if (enumerant.ends_with("NV"))
				implementer = 3;
			else if (enumerant.ends_with("AMD"))
				implementer = 4;
			else if (enumerant.ends_with("INTEL"))
				implementer = 5;

			auto seenItr = seenValues.find(itr->m_Value);
			if (seenItr == seenValues.end())
			{
				seenValues.insert({ itr->m_Value, implementer });
				++itr;
				continue;
			}

			if (seenItr->second < implementer)
			{
				for (auto itr2 = operandKind.m_Enumerants.begin(); itr2 != operandKind.m_Enumerants.end(); ++itr2)
				{
					if (itr2->m_Value == itr->m_Value)
					{
						ReplaceEnumerants(itr->m_Enumerant, itr2->m_Enumerant);
						break;
					}
				}
				itr = operandKind.m_Enumerants.erase(itr);
			}
			else
			{
				std::size_t offset = itr - operandKind.m_Enumerants.begin();
				for (auto itr2 = operandKind.m_Enumerants.begin(); itr2 != operandKind.m_Enumerants.end(); ++itr2)
				{
					if (itr2->m_Value == itr->m_Value)
					{
						if (itr2 < itr)
							--offset;
						ReplaceEnumerants(itr2->m_Enumerant, itr->m_Enumerant);
						operandKind.m_Enumerants.erase(itr2);
						break;
					}
				}
				itr = operandKind.m_Enumerants.begin() + offset;
			}
		}
	}

	{
		EnsureDirectories("Inc/Frertex/Transpilers/SPIRV/");
		EnsureDirectories("Src/Transpilers/SPIRV/");
		std::ofstream enumerationsHeader("Inc/Frertex/Transpilers/SPIRV/Enumerations.h");
		std::ofstream enumerationsSource("Src/Transpilers/SPIRV/Enumerations.cpp");
		if (enumerationsHeader && enumerationsSource)
		{
			enumerationsHeader << "#pragma once\n\n#include <cstdint>\n\n#include <set>\n#include <string>\n\nnamespace Frertex::Transpilers::SPIRV\n{\n";
			enumerationsSource << "#include \"Frertex/Transpilers/SPIRV/Enumerations.h\"\n\nnamespace Frertex::Transpilers::SPIRV\n{\n";

			std::string requireFuncs;

			bool addGap = false;
			for (auto& operandKind : m_OperandKinds)
			{
				if (operandKind.m_Category != "ValueEnum" && operandKind.m_Category != "BitEnum")
					continue;

				if (addGap)
				{
					enumerationsHeader << "\n";
					enumerationsSource << "\n";
				}
				addGap = true;

				enumerationsHeader << "\tenum class E" << operandKind.m_Kind << " : std::uint32_t\n\t{";
				enumerationsSource << "\tvoid RequireCapExt" << operandKind.m_Kind << "([[maybe_unused]] E" << operandKind.m_Kind << " value, [[maybe_unused]] std::set<ECapability>& capabilities, [[maybe_unused]] std::set<std::string>& extensions)\n\t{\n";
				bool requiresSomething = false;
				bool addComma          = false;
				for (auto& enumerant : operandKind.m_Enumerants)
				{
					if (addComma)
						enumerationsHeader << ',';
					addComma = true;

					enumerationsHeader << "\n\t\t" << enumerant.m_Enumerant << " = ";
					if (operandKind.m_Category == "BitEnum")
					{
						char hexStr[10];
						HexRep(hexStr, enumerant.m_Value);
						enumerationsHeader << "0x" << hexStr;
					}
					else
					{
						enumerationsHeader << enumerant.m_Value;
					}
					if (!enumerant.m_Capabilities.empty() || !enumerant.m_Extensions.empty())
					{
						if (!requiresSomething)
							enumerationsSource << "\t\tswitch (value)\n\t\t{\n";
						requiresSomething = true;
						enumerationsSource << "\t\tcase E" << operandKind.m_Kind << "::" << enumerant.m_Enumerant << ":\n";
						for (auto& cap : enumerant.m_Capabilities)
							enumerationsSource << "\t\t\tcapabilities.insert(ECapability::" << cap << ");\n";
						for (auto& ext : enumerant.m_Extensions)
							enumerationsSource << "\t\t\textensions.insert(\"" << ext << "\");\n";
						enumerationsSource << "\t\t\tbreak;\n";
					}
				}
				enumerationsHeader << "\n\t};\n";
				requireFuncs += "\n\tvoid RequireCapExt" + operandKind.m_Kind + "(E" + operandKind.m_Kind + " value, std::set<ECapability>& capabilities, std::set<std::string>& extensions);";
				if (requiresSomething)
					enumerationsSource << "\t\tdefault: break;\n\t\t}\n\t}\n";
				else
					enumerationsSource << "\t}\n";
			}
			enumerationsHeader << requireFuncs << '\n';

			enumerationsHeader << "} // namespace Frertex::Transpilers::SPIRV";
			enumerationsSource << "} // namespace Frertex::Transpilers::SPIRV";
		}

		enumerationsHeader.close();
		enumerationsSource.close();
	}

	{
		EnsureDirectories("Inc/Frertex/Transpilers/SPIRV/");
		EnsureDirectories("Src/Transpilers/SPIRV/");
		std::ofstream codeBufferHeader("Inc/Frertex/Transpilers/SPIRV/CodeBuffer.h");
		std::ofstream codeBufferSource("Src/Transpilers/SPIRV/CodeBuffer.cpp");
		if (codeBufferHeader && codeBufferSource)
		{
			codeBufferHeader << "#pragma once\n\n#include \"Enumerations.h\"\n#include \"Frertex/Utils/Buffer.h\"\n\n#include <cstdint>\n\n#include <optional>\n#include <set>\n#include <string_view>\n#include <vector>\n\nnamespace Frertex::Transpilers::SPIRV\n{\n\tstruct CodeBuffer\n\t{\n\tpublic:";
			codeBufferSource << "#include \"Frertex/Transpilers/SPIRV/CodeBuffer.h\"\n#include \"Frertex/Utils/Profiler.h\"\n\nnamespace Frertex::Transpilers::SPIRV\n{";

			for (auto& instruction : m_Instructions)
			{
				codeBufferHeader << "\n\t\tvoid " << instruction.m_Name << "(";
				codeBufferSource << "\n\tvoid CodeBuffer::" << instruction.m_Name << "(";
				bool          addComma          = false;
				std::uint32_t operandIndex      = 1;
				std::uint32_t smallestWordCount = 1;
				std::string   extraWordCalc;
				std::string   operandValidations;
				std::string   operandRequires;
				std::string   operandPushes;

				std::set<std::string> usedNames;

				for (auto& operand : instruction.m_Operands)
				{
					std::uint32_t operandSize = 1;

					if (addComma)
					{
						codeBufferHeader << ", ";
						codeBufferSource << ", ";
					}
					addComma = true;

					bool isObj    = false;
					bool isOptMul = false;
					bool isMul    = false;
					bool isVector = false;
					bool isString = false;
					bool isEnum   = false;
					bool isPair   = false;
					bool isID     = false;

					if (operand.m_Quantifier == "?") // Optional
					{
						isOptMul = true;
					}
					else if (operand.m_Quantifier == "*") // Multiple
					{
						isOptMul = true;
						isMul    = true;
						isObj    = true;
					}

					std::string type;
					std::string safeName    = operand.m_Name;
					auto        operandKind = GetKind(operand.m_Kind);
					if (!operandKind->m_Enumerants.empty())
						isEnum = true;
					if (safeName.empty())
					{
						if (operand.m_Kind == "IdResultType")
						{
							safeName = "resultTypeID";
						}
						else if (operand.m_Kind == "IdResult")
						{
							safeName = "resultID";
						}
						else if (!operandKind->m_Enumerants.empty())
						{
							safeName = SafeOperandName(std::string { operand.m_Kind }, isMul);
						}
						else
						{
							safeName = "_operand" + std::to_string(operandIndex);
						}
					}
					else
						safeName = SafeOperandName(safeName.substr(1, safeName.size() - 2), isMul);

					if (operandKind->m_Category == "Id")
					{
						if (safeName.starts_with("id"))
							safeName = safeName.substr(2);
						if (!safeName.ends_with("ID"))
							safeName += "ID";
					}

					{
						std::string newSafeName = safeName;
						std::size_t id          = 0;
						while (usedNames.contains(newSafeName))
							newSafeName = safeName + std::to_string(id);
						safeName = newSafeName;
						usedNames.insert(safeName);
					}

					bool canBeOptMul = true;
					if (operandKind->m_Category == "Id")
					{
						type = "std::uint32_t";
						isID = true;
					}
					else if (operandKind->m_Category == "Literal")
					{
						if (operand.m_Kind == "LiteralInteger")
						{
							type = "std::uint32_t";
						}
						else if (operand.m_Kind == "LiteralString")
						{
							type     = "std::string_view";
							isObj    = true;
							isString = true;
							if (!extraWordCalc.empty())
								extraWordCalc += " + ";
							if (isOptMul && !isMul)
								extraWordCalc += "(!" + safeName + ".empty() ? literalStringLength(" + safeName + ") : 0)";
							else
								extraWordCalc += "literalStringLength(" + safeName + ")";
							operandSize = 0;
							if (isOptMul && !isMul)
								operandPushes += "\n\t\tif (!" + safeName + ".empty())\n\t\t\tpushLiteralString(" + safeName + ");";
							else
								operandPushes += "\n\t\tpushLiteralString(" + safeName + ");";
						}
						else if (operand.m_Kind == "LiteralContextDependentNumber")
						{
							type        = "const std::vector<std::uint32_t>&";
							canBeOptMul = false;
							isObj       = true;
							isVector    = true;
							if (!extraWordCalc.empty())
								extraWordCalc += " + ";
							extraWordCalc += safeName + ".size()";
							operandSize = 0;
						}
						else if (operand.m_Kind == "LiteralExtInstInteger")
						{
							type = "std::uint32_t";
						}
						else if (operand.m_Kind == "LiteralSpecConstantOpInteger")
						{
							type = "std::uint32_t";
						}
					}
					else if (operandKind->m_Category == "Composite")
					{
						if (operand.m_Kind == "PairLiteralIntegerIdRef")
						{
							type        = "std::pair<std::uint32_t, std::uint32_t>";
							isObj       = true;
							isPair      = true;
							operandSize = 2;
						}
						else if (operand.m_Kind == "PairIdRefLiteralInteger")
						{
							type        = "std::pair<std::uint32_t, std::uint32_t>";
							isObj       = true;
							isPair      = true;
							operandSize = 2;
						}
						else if (operand.m_Kind == "PairIdRefIdRef")
						{
							type        = "std::pair<std::uint32_t, std::uint32_t>";
							isObj       = true;
							isPair      = true;
							operandSize = 2;
						}
					}
					else
					{
						type = "E" + std::string { operand.m_Kind };
					}

					SPIRVEnumerant* enumerant = nullptr;
					if (isEnum)
						enumerant = GetEnumerant(*operandKind, "None");

					if (isMul && canBeOptMul)
					{
						type = "const std::vector<" + type + ">&";
						if (isPair)
						{
							if (!extraWordCalc.empty())
								extraWordCalc += " + ";
							extraWordCalc += safeName + ".size() * 2";
							operandPushes += "\n\t\tm_Buffer.reserve(m_Buffer.size() + " + safeName + ".size() * 2);\n\t\tfor (auto& v : " + safeName + ")\n\t\t{\n\t\t\tm_Buffer.emplace_back(v.first);\n\t\t\tm_Buffer.emplace_back(v.second);\n\t\t}";
						}
						else
						{
							if (!extraWordCalc.empty())
								extraWordCalc += " + ";
							extraWordCalc += safeName + ".size()";
							operandPushes += "\n\t\tm_Buffer.insert(m_Buffer.end(), " + safeName + ".begin(), " + safeName + ".end());";
						}
					}
					else if (!isMul && isOptMul && isEnum && !enumerant)
					{
						type  = "std::optional<" + type + ">";
						isObj = true;
						if (!extraWordCalc.empty())
							extraWordCalc += " + ";
						extraWordCalc += "(" + safeName + " ? " + std::to_string(operandSize) + " : 0)";
						operandPushes += "\n\t\tif (" + safeName + ")\n\t\t\tm_Buffer.emplace_back(static_cast<std::uint32_t>(*" + safeName + "));";
					}

					codeBufferHeader << type << ' ' << safeName;
					codeBufferSource << type << ' ' << safeName;
					if (isOptMul)
					{
						if (isObj)
						{
							codeBufferHeader << " = {}";
						}
						else if (isEnum)
						{
							codeBufferHeader << " = E" << operand.m_Kind << "::" << SafeEnumerant(std::string { enumerant->m_Enumerant });
							operandRequires += "\n\t\tif (" + safeName + " != E" + operand.m_Kind + "::" + SafeEnumerant(std::string { enumerant->m_Enumerant }) + ")\n\t\t\tRequireCapExt" + operand.m_Kind + "(" + safeName + ", m_Capabilities, m_Extensions);";
							if (!extraWordCalc.empty())
								extraWordCalc += " + ";
							extraWordCalc += "(" + safeName + " != E" + operand.m_Kind + "::" + SafeEnumerant(std::string { enumerant->m_Enumerant }) + " ? 1 : 0)";
							operandPushes += "\n\t\tif (" + safeName + " != E" + operand.m_Kind + "::" + SafeEnumerant(std::string { enumerant->m_Enumerant }) + ")\n\t\t\tm_Buffer.emplace_back(static_cast<std::uint32_t>(" + safeName + "));";
						}
						else
						{
							codeBufferHeader << " = 0";
							if (!extraWordCalc.empty())
								extraWordCalc += " + ";
							extraWordCalc += "!!" + safeName;
							operandPushes += "\n\t\tif (" + safeName + ")\n\t\t\tm_Buffer.emplace_back(" + safeName + ");";
						}
					}
					else
					{
						smallestWordCount += operandSize;

						if (isPair)
						{
							operandPushes += "\n\t\tm_Buffer.emplace_back(" + safeName + ".first);\n\t\tm_Buffer.emplace_back(" + safeName + ".second);";
						}
						else if (isEnum)
						{
							operandRequires += "\n\t\tRequireCapExt" + operand.m_Kind + "(" + safeName + ", m_Capabilities, m_Extensions);";
							operandPushes += "\n\t\tm_Buffer.emplace_back(static_cast<std::uint32_t>(" + safeName + "));";
						}
						else if (isVector)
						{
							if (isPair)
							{
								operandPushes += "\n\t\tm_Buffer.reserve(m_Buffer.size() + " + safeName + ".size() * 2);\n\t\tfor (auto& v : " + safeName + ")\n\t\t{\n\t\t\tm_Buffer.emplace_back(v.first);\n\t\t\tm_Buffer.emplace_back(v.second);\n\t\t}";
							}
							else
							{
								operandPushes += "\n\t\tm_Buffer.insert(m_Buffer.end(), " + safeName + ".begin(), " + safeName + ".end());";
							}
						}
						else if (!isString)
						{
							operandPushes += "\n\t\tm_Buffer.emplace_back(" + safeName + ");";
						}

						if (isPair)
						{
							if (!operandValidations.empty())
								operandValidations += " || ";
							operandValidations += safeName + ".first == 0 || " + safeName + ".second == 0";
						}
						else if (isID)
						{
							if (!operandValidations.empty())
								operandValidations += " || ";
							operandValidations += safeName + " == 0";
						}
					}

					++operandIndex;
				}
				codeBufferHeader << "); // " << instruction.m_OpCode;
				codeBufferSource << ")\n\t{\n\t\tPROFILE_FUNC;";

				if (!operandValidations.empty())
					codeBufferSource << "\n\n\t\tif (" << operandValidations << ")\n\t\t\treturn;";

				if (!instruction.m_Capabilities.empty() || !instruction.m_Extensions.empty() || !operandRequires.empty())
					codeBufferSource << "\n";
				for (auto& capability : instruction.m_Capabilities)
					codeBufferSource << "\n\t\trequireCapability(ECapability::" << capability << ");";
				for (auto& extension : instruction.m_Extensions)
					codeBufferSource << "\n\t\trequireExtension(\"" << extension << "\");";
				codeBufferSource << operandRequires;

				char smallestWordCountStr[5];
				char opcodeStr[5];
				HexRep(smallestWordCountStr, static_cast<std::uint16_t>(smallestWordCount));
				HexRep(opcodeStr, static_cast<std::uint16_t>(instruction.m_OpCode));
				codeBufferSource << "\n\n\t\tm_Buffer.emplace_back(0x" << smallestWordCountStr << '\'' << opcodeStr;
				if (!extraWordCalc.empty())
					codeBufferSource << " + static_cast<std::uint32_t>((" + extraWordCalc + ") << 16)";
				codeBufferSource << ");" << operandPushes;

				codeBufferSource << "\n\t}\n";
			}

			codeBufferHeader << "\n\n\t\tauto begin() { return m_Buffer.begin(); }\n\t\tauto end() { return m_Buffer.end(); }\n\t\tauto begin() const { return m_Buffer.begin(); }\n\t\tauto end() const { return m_Buffer.end(); }\n\t\tauto cbegin() const { return m_Buffer.cbegin(); }\n\t\tauto cend() const { return m_Buffer.cend(); }\n\t\tauto rbegin() { return m_Buffer.rbegin(); }\n\t\tauto rend() { return m_Buffer.rend(); }\n\t\tauto rbegin() const { return m_Buffer.rbegin(); }\n\t\tauto rend() const { return m_Buffer.rend(); }\n\t\tauto crbegin() const { return m_Buffer.crbegin(); }\n\t\tauto crend() const { return m_Buffer.crend(); }\n\n\t\tstd::vector<std::uint32_t> get() { return std::move(m_Buffer); }\n\t\tauto& getCapabilities() const { return m_Capabilities; }\n\t\tauto& getExtensions() const { return m_Extensions; }\n\n\tprivate:\n\t\tvoid          pushLiteralString(std::string_view name);\n\t\tstd::uint64_t literalStringLength(std::string_view name);\n\n\t\tvoid requireCapability(ECapability capability) { m_Capabilities.insert(capability); }\n\t\tvoid requireExtension(std::string_view extension) { m_Extensions.insert(std::string { extension }); }\n\n\tprivate:\n\t\tstd::vector<std::uint32_t> m_Buffer;\n\t\tstd::set<ECapability>      m_Capabilities;\n\t\tstd::set<std::string>      m_Extensions;\n\t};\n} // namespace Frertex::Transpilers::SPIRV";
			codeBufferSource << "\n\tvoid CodeBuffer::pushLiteralString(std::string_view name)\n\t{\n\t\tPROFILE_FUNC;\n\n\t\tm_Buffer.reserve(m_Buffer.size() + literalStringLength(name));\n\t\tstd::size_t size = (name.size() >> 2) << 2;\n\t\tfor (std::size_t i = 0; i < size; i += 4)\n\t\t{\n\t\t\tstd::uint32_t v = name[i] | (name[i + 1] << 8) | (name[i + 2] << 16) | (name[i + 3] << 24);\n\t\t\tm_Buffer.emplace_back(v);\n\t\t}\n\t\tstd::uint32_t last = 0;\n\t\tfor (std::size_t i = 0, j = size; j < name.size(); ++i, ++j)\n\t\t\tlast += name[j] << (i * 8);\n\t\tm_Buffer.emplace_back(last);\n\t}\n\n\tstd::uint64_t CodeBuffer::literalStringLength(std::string_view name)\n\t{\n\t\tPROFILE_FUNC;\n\t\treturn 1 + (name.size() >> 2);\n\t}\n} // namespace Frertex::Transpilers::SPIRV";
		}

		codeBufferHeader.close();
		codeBufferSource.close();
	}
}
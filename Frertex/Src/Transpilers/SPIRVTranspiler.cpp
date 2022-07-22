#include "Frertex/Transpilers/SPIRVTranspiler.h"
#include "Frertex/Utils/Buffer.h"
#include "Frertex/Utils/Profiler.h"

#include <fmt/format.h>
#include <fstream>
#include <set>

namespace Frertex::Transpilers::SPIRV
{
	std::vector<std::uint8_t> GetSPIRVBinary(const SPIRV& spirv)
	{
		PROFILE_FUNC;

		auto code = spirv.m_Code.getCode();

		Utils::Buffer binary;
		binary.reserve(20 + code.size() * 4);
		binary.pushU32(0x07230203);
		binary.pushU32(0x00010600);
		binary.pushU32(0x00000000);
		binary.pushU32(spirv.m_IDBound);
		binary.pushU32(0x00000000);
		binary.pushU32s(code.begin(), code.end());
		return binary.getBuffer();
	}

	void WriteSPIRVToFile(const std::filesystem::path& file, const SPIRV& spirv)
	{
		PROFILE_FUNC;

		auto binary = GetSPIRVBinary(spirv);

		std::ofstream stream { file, std::ios::binary };
		if (stream)
		{
			stream.write(reinterpret_cast<const char*>(binary.data()), binary.size());
			stream.close();
		}
	}

	SPIRVTranspiler::SPIRVTranspiler()
	{
	}

	SPIRV SPIRVTranspiler::transpile(Utils::CopyMovable<FIL>&& fil)
	{
		PROFILE_FUNC;

		FIL input = fil.get();

		SPIRV spirv;
		auto& code = spirv.m_Code;

		// Figure out required capabilities
		std::set<ECapability> capabilities;
		capabilities.insert(ECapability::Shader); // For now to get EMemoryModel::GLSL450
		{
			for (auto& entrypoint : input.m_Entrypoints)
			{
				switch (entrypoint.m_Type)
				{
				case EEntrypointType::VertexShader:
					capabilities.insert(ECapability::Shader);
					break;
				case EEntrypointType::TessellationControlShader:
				case EEntrypointType::TessellationEvaluationShader:
					capabilities.insert(ECapability::Tessellation);
					break;
				case EEntrypointType::GeometryShader:
					capabilities.insert(ECapability::Geometry);
					break;
				case EEntrypointType::FragmentShader:
					capabilities.insert(ECapability::Shader);
					break;
				case EEntrypointType::ComputeShader:
					capabilities.insert(ECapability::Kernel);
					break;
				case EEntrypointType::RTRayGenShader:
				case EEntrypointType::RTAnyHitShader:
				case EEntrypointType::RTClosestHitShader:
				case EEntrypointType::RTMissShader:
				case EEntrypointType::RTIntersectionShader:
				case EEntrypointType::RTCallableShader:
					capabilities.insert(ECapability::RayTracingKHR);
					break;
				case EEntrypointType::NVTaskShader:
				case EEntrypointType::NVMeshShader:
					capabilities.insert(ECapability::MeshShadingNV);
					break;
				default:
					break;
				}
			}
		}
		for (auto itr = capabilities.begin(); itr != capabilities.end(); ++itr)
			code.pushOpCapability(*itr);

		code.pushOpMemoryModel(EAddressingMode::Logical, EMemoryModel::GLSL450);

		struct CodeInfo
		{
			std::uint32_t m_ResultID;
			CodeBuffer    m_Code;
		};

		std::unordered_map<std::string, CodeInfo> codes;

		if (!input.m_Entrypoints.empty())
		{
			if (!codes.contains("void"))
			{
				CodeInfo type;
				type.m_ResultID = spirv.m_IDBound++;
				type.m_Code.pushOpTypeVoid(type.m_ResultID);
				codes.insert({ "void", type });
			}

			CodeInfo func;
			func.m_ResultID = spirv.m_IDBound++;
			func.m_Code.pushOpTypeFunction(func.m_ResultID, codes["void"].m_ResultID, {});
			codes.insert({ "void()", func });
		}

		struct EntrypointInfo
		{
			std::uint32_t              m_FunctionID;
			std::uint32_t              m_BaseLabelID;
			std::vector<std::uint32_t> m_InputIDs;
			std::vector<std::uint32_t> m_OutputIDs;
		};

		std::vector<EntrypointInfo> entrypointInfos;
		entrypointInfos.resize(input.m_Entrypoints.size());
		for (std::size_t i = 0; i < input.m_Entrypoints.size(); ++i)
		{
			auto&           info           = entrypointInfos[i];
			auto&           entrypoint     = input.m_Entrypoints[i];
			EExecutionModel executionModel = EExecutionModel::Vertex;

			switch (entrypoint.m_Type)
			{
			case EEntrypointType::VertexShader:
				executionModel = EExecutionModel::Vertex;
				break;
			case EEntrypointType::TessellationControlShader:
				executionModel = EExecutionModel::TessellationControl;
				break;
			case EEntrypointType::TessellationEvaluationShader:
				executionModel = EExecutionModel::TessellationEvaluation;
				break;
			case EEntrypointType::GeometryShader:
				executionModel = EExecutionModel::Geometry;
				break;
			case EEntrypointType::FragmentShader:
				executionModel = EExecutionModel::Fragment;
				break;
			case EEntrypointType::ComputeShader:
				executionModel = EExecutionModel::Kernel;
				break;
			case EEntrypointType::RTRayGenShader:
				executionModel = EExecutionModel::RayGenerationKHR;
				break;
			case EEntrypointType::RTAnyHitShader:
				executionModel = EExecutionModel::AnyHitKHR;
				break;
			case EEntrypointType::RTClosestHitShader:
				executionModel = EExecutionModel::ClosestHitKHR;
				break;
			case EEntrypointType::RTMissShader:
				executionModel = EExecutionModel::MissKHR;
				break;
			case EEntrypointType::RTIntersectionShader:
				executionModel = EExecutionModel::IntersectionKHR;
				break;
			case EEntrypointType::RTCallableShader:
				executionModel = EExecutionModel::CallableKHR;
				break;
			case EEntrypointType::NVTaskShader:
				executionModel = EExecutionModel::TaskNV;
				break;
			case EEntrypointType::NVMeshShader:
				executionModel = EExecutionModel::MeshNV;
				break;
			default:
				break;
			}

			auto& label = input.m_Labels[entrypoint.m_LabelID];
			auto  name  = std::string_view(reinterpret_cast<const char*>(input.m_Strings.data() + label.m_NameOffset), label.m_NameLength);

			info.m_FunctionID  = spirv.m_IDBound++;
			info.m_BaseLabelID = spirv.m_IDBound++;
			info.m_InputIDs.resize(entrypoint.m_Inputs.size());
			for (std::size_t j = 0; j < entrypoint.m_Inputs.size(); ++j)
			{
				auto& input = entrypoint.m_Inputs[j];

				// TODO(MarcasRealAccount): Optimize ASAP
				std::string typeName = TypeIDToString(static_cast<ETypeIDs>(input.m_TypeID));

				if (!codes.contains(typeName))
				{
					std::uint64_t rows    = ((input.m_TypeID >> 20) & 3) + 1;
					std::uint64_t columns = ((input.m_TypeID >> 22) & 3) + 1;

					CodeInfo type;
					if (rows == 1)
					{
						if (columns != 1)
						{
							ETypeIDs    baseTypeID = static_cast<ETypeIDs>(((input.m_TypeID & 0xFFFF) / columns) | (input.m_TypeID & ~0xF0'FFFF));
							std::string baseName   = TypeIDToString(baseTypeID);
							if (!codes.contains(baseName))
							{
								CodeInfo base;
								base.m_ResultID = spirv.m_IDBound++;
								switch (baseTypeID)
								{
								case ETypeIDs::Bool:
									base.m_Code.pushOpTypeBool(base.m_ResultID);
									break;
								case ETypeIDs::Byte:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 1);
									break;
								case ETypeIDs::UByte:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 0);
									break;
								case ETypeIDs::Short:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 1);
									break;
								case ETypeIDs::UShort:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 0);
									break;
								case ETypeIDs::Int:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 1);
									break;
								case ETypeIDs::UInt:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 0);
									break;
								case ETypeIDs::Long:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 1);
									break;
								case ETypeIDs::ULong:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 46, 0);
									break;
								case ETypeIDs::Half:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 16);
									break;
								case ETypeIDs::Float:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 32);
									break;
								case ETypeIDs::Double:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 64);
									break;
								}
								codes.insert({ baseName, base });
							}

							type.m_ResultID = spirv.m_IDBound++;
							type.m_Code.pushOpTypeVector(type.m_ResultID, codes[baseName].m_ResultID, static_cast<std::uint32_t>(columns));
						}
						else
						{
							type.m_ResultID = spirv.m_IDBound++;
							switch (static_cast<ETypeIDs>(input.m_TypeID & ~0xF0'0000))
							{
							case ETypeIDs::Bool:
								type.m_Code.pushOpTypeBool(type.m_ResultID);
								break;
							case ETypeIDs::Byte:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 8, 1);
								break;
							case ETypeIDs::UByte:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 8, 0);
								break;
							case ETypeIDs::Short:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 16, 1);
								break;
							case ETypeIDs::UShort:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 16, 0);
								break;
							case ETypeIDs::Int:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 32, 1);
								break;
							case ETypeIDs::UInt:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 32, 0);
								break;
							case ETypeIDs::Long:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 64, 1);
								break;
							case ETypeIDs::ULong:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 64, 0);
								break;
							case ETypeIDs::Half:
								type.m_Code.pushOpTypeFloat(type.m_ResultID, 16);
								break;
							case ETypeIDs::Float:
								type.m_Code.pushOpTypeFloat(type.m_ResultID, 32);
								break;
							case ETypeIDs::Double:
								type.m_Code.pushOpTypeFloat(type.m_ResultID, 64);
								break;
							}
						}
					}
					else if (columns != 1)
					{
						std::string vectorName = TypeIDToString(static_cast<ETypeIDs>(((input.m_TypeID & 0xFFFF) / columns) | (input.m_TypeID & ~0xC0'FFFF)));

						if (!codes.contains(vectorName))
						{
							ETypeIDs    baseTypeID = static_cast<ETypeIDs>(((input.m_TypeID & 0xFFFF) / (rows * columns)) | (input.m_TypeID & ~0xF0'FFFF));
							std::string baseName   = TypeIDToString(baseTypeID);
							if (!codes.contains(baseName))
							{
								CodeInfo base;
								base.m_ResultID = spirv.m_IDBound++;
								switch (baseTypeID)
								{
								case ETypeIDs::Bool:
									base.m_Code.pushOpTypeBool(base.m_ResultID);
									break;
								case ETypeIDs::Byte:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 1);
									break;
								case ETypeIDs::UByte:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 0);
									break;
								case ETypeIDs::Short:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 1);
									break;
								case ETypeIDs::UShort:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 0);
									break;
								case ETypeIDs::Int:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 1);
									break;
								case ETypeIDs::UInt:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 0);
									break;
								case ETypeIDs::Long:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 1);
									break;
								case ETypeIDs::ULong:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 0);
									break;
								case ETypeIDs::Half:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 16);
									break;
								case ETypeIDs::Float:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 32);
									break;
								case ETypeIDs::Double:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 64);
									break;
								}
								codes.insert({ baseName, base });
							}

							CodeInfo vector;
							vector.m_ResultID = spirv.m_IDBound++;
							vector.m_Code.pushOpTypeVector(vector.m_ResultID, codes[baseName].m_ResultID, static_cast<std::uint32_t>(rows));
							codes.insert({ vectorName, vector });
						}
						type.m_ResultID = spirv.m_IDBound++;
						type.m_Code.pushOpTypeMatrix(type.m_ResultID, codes[vectorName].m_ResultID, static_cast<std::uint32_t>(columns));
					}
					else if (rows != 1)
					{
						ETypeIDs    baseTypeID = static_cast<ETypeIDs>(((input.m_TypeID & 0xFFFF) / rows) | (input.m_TypeID & ~0xF0'FFFF));
						std::string baseName   = TypeIDToString(baseTypeID);
						if (!codes.contains(baseName))
						{
							CodeInfo base;
							base.m_ResultID = spirv.m_IDBound++;
							switch (baseTypeID)
							{
							case ETypeIDs::Bool:
								base.m_Code.pushOpTypeBool(base.m_ResultID);
								break;
							case ETypeIDs::Byte:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 1);
								break;
							case ETypeIDs::UByte:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 0);
								break;
							case ETypeIDs::Short:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 1);
								break;
							case ETypeIDs::UShort:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 0);
								break;
							case ETypeIDs::Int:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 1);
								break;
							case ETypeIDs::UInt:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 0);
								break;
							case ETypeIDs::Long:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 1);
								break;
							case ETypeIDs::ULong:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 0);
								break;
							case ETypeIDs::Half:
								base.m_Code.pushOpTypeFloat(base.m_ResultID, 16);
								break;
							case ETypeIDs::Float:
								base.m_Code.pushOpTypeFloat(base.m_ResultID, 32);
								break;
							case ETypeIDs::Double:
								base.m_Code.pushOpTypeFloat(base.m_ResultID, 64);
								break;
							}
							codes.insert({ baseName, base });
						}

						type.m_ResultID = spirv.m_IDBound++;
						type.m_Code.pushOpTypeVector(type.m_ResultID, codes[baseName].m_ResultID, static_cast<std::uint32_t>(rows));
					}

					codes.insert({ typeName, type });
				}

				std::string pointer = "_ptr_Input_" + typeName;
				if (!codes.contains(pointer))
				{
					CodeInfo ptr;
					ptr.m_ResultID = spirv.m_IDBound++;
					ptr.m_Code.pushOpTypePointer(ptr.m_ResultID, EStorageClass::Input, codes[typeName].m_ResultID);
					codes.insert({ pointer, ptr });
				}

				info.m_InputIDs[j] = spirv.m_IDBound++;
				CodeInfo variable;
				variable.m_ResultID = info.m_InputIDs[j];
				variable.m_Code.pushOpVariable(codes[pointer].m_ResultID, variable.m_ResultID, EStorageClass::Input);
				codes.insert({ fmt::format("{}_in{}", name, j), variable });
			}
			info.m_OutputIDs.resize(entrypoint.m_Outputs.size());
			for (std::size_t j = 0; j < entrypoint.m_Outputs.size(); ++j)
			{
				auto& output = entrypoint.m_Outputs[j];

				std::string typeName = TypeIDToString(static_cast<ETypeIDs>(output.m_TypeID));

				if (!codes.contains(typeName))
				{
					std::uint64_t rows    = ((output.m_TypeID >> 20) & 3) + 1;
					std::uint64_t columns = ((output.m_TypeID >> 22) & 3) + 1;

					CodeInfo type;
					if (rows == 1)
					{
						if (columns != 1)
						{
							ETypeIDs    baseTypeID = static_cast<ETypeIDs>(((output.m_TypeID & 0xFFFF) / columns) | (output.m_TypeID & ~0xF0'FFFF));
							std::string baseName   = TypeIDToString(baseTypeID);
							if (!codes.contains(baseName))
							{
								CodeInfo base;
								base.m_ResultID = spirv.m_IDBound++;
								switch (baseTypeID)
								{
								case ETypeIDs::Bool:
									base.m_Code.pushOpTypeBool(base.m_ResultID);
									break;
								case ETypeIDs::Byte:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 1);
									break;
								case ETypeIDs::UByte:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 0);
									break;
								case ETypeIDs::Short:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 1);
									break;
								case ETypeIDs::UShort:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 0);
									break;
								case ETypeIDs::Int:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 1);
									break;
								case ETypeIDs::UInt:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 0);
									break;
								case ETypeIDs::Long:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 1);
									break;
								case ETypeIDs::ULong:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 0);
									break;
								case ETypeIDs::Half:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 16);
									break;
								case ETypeIDs::Float:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 32);
									break;
								case ETypeIDs::Double:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 64);
									break;
								}
								codes.insert({ baseName, base });
							}

							type.m_ResultID = spirv.m_IDBound++;
							type.m_Code.pushOpTypeVector(type.m_ResultID, codes[baseName].m_ResultID, static_cast<std::uint32_t>(columns));
						}
						else
						{
							type.m_ResultID = spirv.m_IDBound++;
							switch (static_cast<ETypeIDs>(output.m_TypeID & ~0xF0'0000))
							{
							case ETypeIDs::Bool:
								type.m_Code.pushOpTypeBool(type.m_ResultID);
								break;
							case ETypeIDs::Byte:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 8, 1);
								break;
							case ETypeIDs::UByte:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 8, 0);
								break;
							case ETypeIDs::Short:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 16, 1);
								break;
							case ETypeIDs::UShort:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 16, 0);
								break;
							case ETypeIDs::Int:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 32, 1);
								break;
							case ETypeIDs::UInt:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 32, 0);
								break;
							case ETypeIDs::Long:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 64, 1);
								break;
							case ETypeIDs::ULong:
								type.m_Code.pushOpTypeInt(type.m_ResultID, 64, 0);
								break;
							case ETypeIDs::Half:
								type.m_Code.pushOpTypeFloat(type.m_ResultID, 16);
								break;
							case ETypeIDs::Float:
								type.m_Code.pushOpTypeFloat(type.m_ResultID, 32);
								break;
							case ETypeIDs::Double:
								type.m_Code.pushOpTypeFloat(type.m_ResultID, 64);
								break;
							}
						}
					}
					else if (columns != 1)
					{
						std::string vectorName = TypeIDToString(static_cast<ETypeIDs>(((output.m_TypeID & 0xFFFF) / columns) | (output.m_TypeID & ~0xC0'FFFF)));

						if (!codes.contains(vectorName))
						{
							ETypeIDs    baseTypeID = static_cast<ETypeIDs>(((output.m_TypeID & 0xFFFF) / (rows * columns)) | (output.m_TypeID & ~0xF0'FFFF));
							std::string baseName   = TypeIDToString(baseTypeID);
							if (!codes.contains(baseName))
							{
								CodeInfo base;
								base.m_ResultID = spirv.m_IDBound++;
								switch (baseTypeID)
								{
								case ETypeIDs::Bool:
									base.m_Code.pushOpTypeBool(base.m_ResultID);
									break;
								case ETypeIDs::Byte:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 1);
									break;
								case ETypeIDs::UByte:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 0);
									break;
								case ETypeIDs::Short:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 1);
									break;
								case ETypeIDs::UShort:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 0);
									break;
								case ETypeIDs::Int:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 1);
									break;
								case ETypeIDs::UInt:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 0);
									break;
								case ETypeIDs::Long:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 1);
									break;
								case ETypeIDs::ULong:
									base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 0);
									break;
								case ETypeIDs::Half:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 16);
									break;
								case ETypeIDs::Float:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 32);
									break;
								case ETypeIDs::Double:
									base.m_Code.pushOpTypeFloat(base.m_ResultID, 64);
									break;
								}
								codes.insert({ baseName, base });
							}

							CodeInfo vector;
							vector.m_ResultID = spirv.m_IDBound++;
							vector.m_Code.pushOpTypeVector(vector.m_ResultID, codes[baseName].m_ResultID, static_cast<std::uint32_t>(rows));
							codes.insert({ vectorName, vector });
						}
						type.m_ResultID = spirv.m_IDBound++;
						type.m_Code.pushOpTypeMatrix(type.m_ResultID, codes[vectorName].m_ResultID, static_cast<std::uint32_t>(columns));
					}
					else if (rows != 1)
					{
						ETypeIDs    baseTypeID = static_cast<ETypeIDs>(((output.m_TypeID & 0xFFFF) / rows) | (output.m_TypeID & ~0xF0'FFFF));
						std::string baseName   = TypeIDToString(baseTypeID);
						if (!codes.contains(baseName))
						{
							CodeInfo base;
							base.m_ResultID = spirv.m_IDBound++;
							switch (baseTypeID)
							{
							case ETypeIDs::Bool:
								base.m_Code.pushOpTypeBool(base.m_ResultID);
								break;
							case ETypeIDs::Byte:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 1);
								break;
							case ETypeIDs::UByte:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 8, 0);
								break;
							case ETypeIDs::Short:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 1);
								break;
							case ETypeIDs::UShort:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 16, 0);
								break;
							case ETypeIDs::Int:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 1);
								break;
							case ETypeIDs::UInt:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 32, 0);
								break;
							case ETypeIDs::Long:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 1);
								break;
							case ETypeIDs::ULong:
								base.m_Code.pushOpTypeInt(base.m_ResultID, 64, 0);
								break;
							case ETypeIDs::Half:
								base.m_Code.pushOpTypeFloat(base.m_ResultID, 16);
								break;
							case ETypeIDs::Float:
								base.m_Code.pushOpTypeFloat(base.m_ResultID, 32);
								break;
							case ETypeIDs::Double:
								base.m_Code.pushOpTypeFloat(base.m_ResultID, 64);
								break;
							}
							codes.insert({ baseName, base });
						}

						type.m_ResultID = spirv.m_IDBound++;
						type.m_Code.pushOpTypeVector(type.m_ResultID, codes[baseName].m_ResultID, static_cast<std::uint32_t>(rows));
					}

					codes.insert({ typeName, type });
				}

				std::string pointer = "_ptr_Output_" + typeName;
				if (!codes.contains(pointer))
				{
					CodeInfo ptr;
					ptr.m_ResultID = spirv.m_IDBound++;
					ptr.m_Code.pushOpTypePointer(ptr.m_ResultID, EStorageClass::Output, codes[typeName].m_ResultID);
					codes.insert({ pointer, ptr });
				}

				info.m_OutputIDs[j] = spirv.m_IDBound++;
				CodeInfo variable;
				variable.m_ResultID = info.m_OutputIDs[j];
				variable.m_Code.pushOpVariable(codes[pointer].m_ResultID, variable.m_ResultID, EStorageClass::Output);
				codes.insert({ fmt::format("{}_out{}", name, j), variable });
			}

			std::vector<std::uint32_t> interface;
			interface.reserve(info.m_InputIDs.size() + info.m_OutputIDs.size());
			interface.insert(interface.end(), info.m_InputIDs.begin(), info.m_InputIDs.end());
			interface.insert(interface.end(), info.m_OutputIDs.begin(), info.m_OutputIDs.end());
			code.pushOpEntryPoint(executionModel, info.m_FunctionID, name, interface);
		}

		// TODO(MarcasRealAccount): Implement debug info
		// code.pushOpSource(0x0000'0008, 0x0000'0000, 0x0000'0000);

		{
			// Push sorted codes

			std::vector<CodeInfo> sortedTypeInfos;
			for (auto itr = codes.begin(); itr != codes.end(); ++itr)
			{
				bool added = false;
				for (auto itr2 = sortedTypeInfos.begin(); itr2 != sortedTypeInfos.end(); ++itr2)
				{
					if (itr2->m_ResultID > itr->second.m_ResultID)
					{
						added = true;
						sortedTypeInfos.insert(itr2, itr->second);
						break;
					}
				}

				if (!added)
					sortedTypeInfos.emplace_back(itr->second);
			}

			for (auto& type : sortedTypeInfos)
				code.pushCodeBuffer(type.m_Code);
		}

		for (std::size_t i = 0; i < input.m_Entrypoints.size(); ++i)
		{
			auto&                  info       = entrypointInfos[i];
			[[maybe_unused]] auto& entrypoint = input.m_Entrypoints[i];

			code.pushOpFunction(codes["void"].m_ResultID, info.m_FunctionID, EFunctionControl::None, codes["void()"].m_ResultID);
			code.pushOpLabel(info.m_BaseLabelID);

			code.pushOpReturn();
			code.pushOpFunctionEnd();
		}

		return spirv;
	}

	void SPIRVTranspiler::addWarning(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_Messages.emplace_back(EMessageType::Warning, span, point, message.get());
	}

	void SPIRVTranspiler::addError(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		m_Messages.emplace_back(EMessageType::Error, span, point, message.get());
	}
} // namespace Frertex::Transpilers::SPIRV
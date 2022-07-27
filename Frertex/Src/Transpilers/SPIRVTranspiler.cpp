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

	SPIRVTranspiler::SPIRVTranspiler(Sources* sources)
	    : m_Sources(sources) {}

	SPIRV SPIRVTranspiler::transpile(Utils::CopyMovable<FIL>&& fil)
	{
		PROFILE_FUNC;

		FIL input = fil.get();

		SPIRV spirv;
		auto& code = spirv.m_Code;

		CodeBuffer entrypointsCode;
		CodeBuffer executionModesCode;
		CodeBuffer decorationsCode;
		CodeBuffer typesCode;
		CodeBuffer variablesCode;
		CodeBuffer constantsCode;
		CodeBuffer functionsCode;

		requiresCapability(ECapability::Shader); // For now to get EMemoryModel::GLSL450
		{
			// Figure out required entrypoint capabilities
			for (auto& entrypoint : input.m_Entrypoints)
			{
				switch (entrypoint.m_Type)
				{
				case EEntrypointType::VertexShader:
					requiresCapability(ECapability::Shader);
					break;
				case EEntrypointType::TessellationControlShader:
				case EEntrypointType::TessellationEvaluationShader:
					requiresCapability(ECapability::Tessellation);
					break;
				case EEntrypointType::GeometryShader:
					requiresCapability(ECapability::Geometry);
					break;
				case EEntrypointType::FragmentShader:
					requiresCapability(ECapability::Shader);
					break;
				case EEntrypointType::ComputeShader:
					requiresCapability(ECapability::Kernel);
					break;
				case EEntrypointType::RTRayGenShader:
				case EEntrypointType::RTAnyHitShader:
				case EEntrypointType::RTClosestHitShader:
				case EEntrypointType::RTMissShader:
				case EEntrypointType::RTIntersectionShader:
				case EEntrypointType::RTCallableShader:
					requiresCapability(ECapability::RayTracingKHR);
					break;
				case EEntrypointType::NVTaskShader:
				case EEntrypointType::NVMeshShader:
					requiresCapability(ECapability::MeshShadingNV);
					break;
				default:
					break;
				}
			}
		}

		struct EntrypointInfo
		{
			struct ArgInfo
			{
				std::uint32_t m_ID;
				bool          m_IsBuiltin = false;
			};

			std::uint32_t m_FunctionID;
			std::uint32_t m_BaseLabelID;

			std::vector<ArgInfo> m_InputIDs;
			std::vector<ArgInfo> m_OutputIDs;

			std::uint32_t m_BuiltinInputID;
			std::uint32_t m_BuiltinOutputID;
		};

		std::vector<EntrypointInfo> entrypointInfos;
		entrypointInfos.resize(input.m_Entrypoints.size());
		for (std::size_t i = 0; i < input.m_Entrypoints.size(); ++i)
		{
			auto&           info           = entrypointInfos[i];
			auto&           entrypoint     = input.m_Entrypoints[i];
			EExecutionModel executionModel = EntrypointTypeToExecutionModel(entrypoint.m_Type);

			auto& label = input.m_Labels[entrypoint.m_LabelID];
			auto  name  = std::string_view(reinterpret_cast<const char*>(input.m_Strings.data() + label.m_NameOffset), label.m_NameLength);

			bool containsBuiltinInputs  = false;
			bool containsBuiltinOutputs = false;

			std::vector<std::uint32_t> interface;
			interface.reserve(entrypoint.m_Inputs.size() + entrypoint.m_Outputs.size());
			info.m_InputIDs.resize(entrypoint.m_Inputs.size());
			for (std::size_t j = 0; j < entrypoint.m_Inputs.size(); ++j)
			{
				auto& param        = entrypoint.m_Inputs[j];
				info.m_InputIDs[j] = {
					getOrAddResultID(
					    fmt::format("{}_in{}", name, j),
					    [&]() -> std::uint32_t
					    {
					        PROFILE_FUNC;

					        if (TypeIDIsUserDefined(static_cast<ETypeIDs>(param.m_TypeID)))
					        {
						        // TODO(MarcasRealAccount): Implement user defined types
						        addError(0, 0, 0, 0, 0, "SPIR-V Transpiler doesn't support user defined types yet");
						        return 0;
					        }
					        else if (TypeIDIsBuiltIn(static_cast<ETypeIDs>(param.m_TypeID)))
					        {
						        containsBuiltinInputs = true;
						        switch (static_cast<ETypeIDs>(param.m_TypeID))
						        {
						        default:
							        addError(0, 0, 0, 0, 0, fmt::format("SPIR-V Transpiler doesn't support builtin input '{}'", TypeIDToString(static_cast<ETypeIDs>(param.m_TypeID))));
							        return ~0U;
						        }
					        }
					        else
					        {
						        std::uint32_t pointerType = getOrAddPointerType(
						            EStorageClass::Input,
						            spirv,
						            typesCode,
						            [&]() -> std::pair<std::string, std::uint32_t>
						            {
							            PROFILE_FUNC;

							            return getOrAddBuiltinTypeN(static_cast<ETypeIDs>(param.m_TypeID), spirv, typesCode);
						            });

						        std::uint32_t resultID = spirv.m_IDBound++;
						        variablesCode.pushOpVariable(pointerType, resultID, EStorageClass::Input);
						        decorationsCode.pushOpDecorate(resultID, EDecoration::Location, { static_cast<std::uint32_t>(param.m_Location) });
						        return resultID;
					        }
					    }),
					TypeIDIsBuiltIn(static_cast<ETypeIDs>(param.m_TypeID))
				};
				if (!info.m_InputIDs[j].m_IsBuiltin)
					interface.emplace_back(info.m_InputIDs[j].m_ID);
			}
			info.m_OutputIDs.resize(entrypoint.m_Outputs.size());
			for (std::size_t j = 0; j < entrypoint.m_Outputs.size(); ++j)
			{
				auto& param         = entrypoint.m_Outputs[j];
				info.m_OutputIDs[j] = {
					getOrAddResultID(
					    fmt::format("{}_out{}", name, j),
					    [&]() -> std::uint32_t
					    {
					        PROFILE_FUNC;

					        if (TypeIDIsUserDefined(static_cast<ETypeIDs>(param.m_TypeID)))
					        {
						        // TODO(MarcasRealAccount): Implement user defined types
						        addError(0, 0, 0, 0, 0, "SPIR-V Transpiler doesn't support user defined types yet");
						        return 0;
					        }
					        else if (TypeIDIsBuiltIn(static_cast<ETypeIDs>(param.m_TypeID)))
					        {
						        containsBuiltinOutputs = true;
						        switch (static_cast<ETypeIDs>(param.m_TypeID))
						        {
						        case ETypeIDs::BuiltinPosition: return 0;
						        case ETypeIDs::BuiltinPointSize: return 1;
							    //case ETypeIDs::BuiltinClipDistance: return 2;
							    //case ETypeIDs::BuiltinCullDistance: return 3;
						        default:
							        addError(0, 0, 0, 0, 0, fmt::format("SPIR-V Transpiler doesn't support builtin input '{}'", TypeIDToString(static_cast<ETypeIDs>(param.m_TypeID))));
							        return ~0U;
						        }
					        }
					        else
					        {
						        std::uint32_t pointerType = getOrAddPointerType(
						            EStorageClass::Output,
						            spirv,
						            typesCode,
						            [&]() -> std::pair<std::string, std::uint32_t>
						            {
							            PROFILE_FUNC;

							            return getOrAddBuiltinTypeN(static_cast<ETypeIDs>(param.m_TypeID), spirv, typesCode);
						            });

						        std::uint32_t resultID = spirv.m_IDBound++;
						        variablesCode.pushOpVariable(pointerType, resultID, EStorageClass::Output);
						        decorationsCode.pushOpDecorate(resultID, EDecoration::Location, { static_cast<std::uint32_t>(param.m_Location) });
						        return resultID;
					        }
					    }),
					TypeIDIsBuiltIn(static_cast<ETypeIDs>(param.m_TypeID))
				};
				if (!info.m_OutputIDs[j].m_IsBuiltin)
					interface.emplace_back(info.m_OutputIDs[j].m_ID);
			}
			info.m_FunctionID  = spirv.m_IDBound++;
			info.m_BaseLabelID = spirv.m_IDBound++;

			if (containsBuiltinInputs)
			{
				switch (entrypoint.m_Type)
				{
				default:
					containsBuiltinInputs = false;
					addError(0, 0, 0, 0, 0, fmt::format("SPIR-V Transpiler doesn't support builtin inputs for entrypoint type '{}'", EntrypointTypeToString(entrypoint.m_Type)));
					break;
				}
			}
			if (containsBuiltinOutputs)
			{
				switch (entrypoint.m_Type)
				{
				case EEntrypointType::VertexShader:
				{
					info.m_BuiltinOutputID = getOrAddResultID(
					    "_gl_PerVertex_Var",
					    [&]() -> std::uint32_t
					    {
						    PROFILE_FUNC;

						    std::uint32_t pointerType = getOrAddPointerType(
						        EStorageClass::Output,
						        spirv,
						        typesCode,
						        [&]() -> std::pair<std::string, std::uint32_t>
						        {
							        PROFILE_FUNC;

							        std::uint32_t floatArrUInt_1 = getOrAddArrayType(
							            spirv,
							            typesCode,
							            getOrAddResultID(
							                "uint_1",
							                [&]() -> std::uint32_t
							                {
								                PROFILE_FUNC;

								                std::uint32_t elementType = getOrAddBuiltinType(ETypeIDs::UInt, spirv, typesCode);
								                std::uint32_t resultID    = spirv.m_IDBound++;
								                typesCode.pushOpConstant(elementType, resultID, { 1 });
								                return resultID;
							                }),
							            [&]() -> std::pair<std::string, std::uint32_t>
							            {
								            PROFILE_FUNC;

								            return getOrAddBuiltinTypeN(ETypeIDs::Float, spirv, typesCode);
							            });

							        std::vector<std::uint32_t> members {
								        getOrAddBuiltinType(ETypeIDs::Float4, spirv, typesCode),
								        getOrAddBuiltinType(ETypeIDs::Float, spirv, typesCode),
								        floatArrUInt_1,
								        floatArrUInt_1
							        };

							        std::uint32_t resultID = spirv.m_IDBound++;
							        typesCode.pushOpTypeStruct(resultID, members);
							        decorationsCode.pushOpMemberDecorate(resultID, 0, EDecoration::BuiltIn, { static_cast<std::uint32_t>(EBuiltIn::Position) });
							        decorationsCode.pushOpMemberDecorate(resultID, 1, EDecoration::BuiltIn, { static_cast<std::uint32_t>(EBuiltIn::PointSize) });
							        decorationsCode.pushOpMemberDecorate(resultID, 2, EDecoration::BuiltIn, { static_cast<std::uint32_t>(EBuiltIn::ClipDistance) });
							        decorationsCode.pushOpMemberDecorate(resultID, 3, EDecoration::BuiltIn, { static_cast<std::uint32_t>(EBuiltIn::CullDistance) });
							        decorationsCode.pushOpDecorate(resultID, EDecoration::Block);
							        m_NamedResultIDs.insert({ "_gl_PerVertex", resultID });
							        return { "_gl_PerVertex", resultID };
						        });

						    std::uint32_t resultID = spirv.m_IDBound++;
						    variablesCode.pushOpVariable(pointerType, resultID, EStorageClass::Output);
						    return resultID;
					    });
					interface.insert(interface.begin(), info.m_BuiltinOutputID);
					break;
				}
				default:
					containsBuiltinOutputs = false;
					addError(0, 0, 0, 0, 0, fmt::format("SPIR-V Transpiler doesn't support builtin outputs for entrypoint type '{}'", EntrypointTypeToString(entrypoint.m_Type)));
					break;
				}
			}

			entrypointsCode.pushOpEntryPoint(executionModel, info.m_FunctionID, name, interface);

			switch (entrypoint.m_Type)
			{
			case EEntrypointType::FragmentShader:
				executionModesCode.pushOpExecutionMode(info.m_FunctionID, EExecutionMode::OriginUpperLeft);
				break;
			default:
				break;
			}

			functionsCode.pushOpFunction(
			    getOrAddBuiltinType(ETypeIDs::Void, spirv, typesCode),
			    info.m_FunctionID,
			    EFunctionControl::None,
			    getOrAddFunctionType(
			        spirv,
			        typesCode,
			        [&]() -> std::pair<std::string, std::uint32_t>
			        {
				        PROFILE_FUNC;

				        return getOrAddBuiltinTypeN(ETypeIDs::Void, spirv, typesCode);
			        }));
			{
				functionsCode.pushOpLabel(info.m_BaseLabelID);

				//std::uint32_t iVarTypeID = getOrAddPointerType(
				//    EStorageClass::Function,
				//    spirv,
				//    typesCode,
				//    [&]() -> std::pair<std::string, std::uint32_t>
				//    {
				//	    PROFILE_FUNC;
				//
				//	    return getOrAddBuiltinTypeN(ETypeIDs::UInt, spirv, typesCode);
				//    });
				//std::uint32_t iVarID = spirv.m_IDBound++;
				//functionsCode.pushOpVariable(iVarTypeID, iVarID, EStorageClass::Function);
				//
				//{
				//	std::uint32_t loopID          = spirv.m_IDBound++;
				//	std::uint32_t mergeID         = spirv.m_IDBound++;
				//	std::uint32_t continuationID  = spirv.m_IDBound++;
				//	std::uint32_t loopConditionID = spirv.m_IDBound++;
				//	std::uint32_t loopBodyID      = spirv.m_IDBound++;
				//	// Loop header
				//	functionsCode.pushOpBranch(loopID);
				//	functionsCode.pushOpLabel(loopID);
				//	functionsCode.pushOpLoopMerge(mergeID, continuationID, ELoopControl::None);
				//	functionsCode.pushOpBranch(loopConditionID);
				//
				//	// Loop condition
				//	{
				//		functionsCode.pushOpLabel(loopConditionID);
				//		std::uint32_t loadID = spirv.m_IDBound++;
				//		functionsCode.pushOpLoad(getOrAddBuiltinType(ETypeIDs::UInt, spirv, typesCode), loadID, iVarID);
				//		std::uint32_t conditionID = spirv.m_IDBound++;
				//		functionsCode.pushOpULessThan(
				//		    getOrAddBuiltinType(ETypeIDs::Bool, spirv, typesCode),
				//		    conditionID,
				//		    loadID,
				//		    getOrAddResultID(
				//		        "uint_1024",
				//		        [&]() -> std::uint32_t
				//		        {
				//			        PROFILE_FUNC;
				//
				//			        std::uint32_t elementType = getOrAddBuiltinType(ETypeIDs::UInt, spirv, typesCode);
				//			        std::uint32_t resultID    = spirv.m_IDBound++;
				//			        typesCode.pushOpConstant(elementType, resultID, { 1024 });
				//			        return resultID;
				//		        }));
				//		functionsCode.pushOpBranchConditional(conditionID, loopBodyID, mergeID);
				//	}
				//
				//	// Loop body
				//	{
				//		functionsCode.pushOpLabel(loopBodyID);
				//
				//		functionsCode.pushOpBranch(continuationID);
				//	}
				//
				//	// Loop continuation
				//	{
				//		functionsCode.pushOpLabel(continuationID);
				//		std::uint32_t loadID   = spirv.m_IDBound++;
				//		std::uint32_t resultID = spirv.m_IDBound++;
				//		functionsCode.pushOpLoad(getOrAddBuiltinType(ETypeIDs::UInt, spirv, typesCode), loadID, iVarID);
				//		functionsCode.pushOpIAdd(
				//		    getOrAddBuiltinType(ETypeIDs::UInt, spirv, typesCode),
				//		    resultID,
				//		    loadID,
				//		    getOrAddResultID(
				//		        "uint_1",
				//		        [&]() -> std::uint32_t
				//		        {
				//			        PROFILE_FUNC;
				//
				//			        std::uint32_t elementType = getOrAddBuiltinType(ETypeIDs::UInt, spirv, typesCode);
				//			        std::uint32_t resultID    = spirv.m_IDBound++;
				//			        typesCode.pushOpConstant(elementType, resultID, { 1 });
				//			        return resultID;
				//		        }));
				//		functionsCode.pushOpStore(iVarID, resultID);
				//		functionsCode.pushOpBranch(loopID);
				//	}
				//
				//	functionsCode.pushOpLabel(mergeID);
				//}

				functionsCode.pushOpReturn();
			}
			functionsCode.pushOpFunctionEnd();
		}

		for (auto itr = m_Capabilities.begin(); itr != m_Capabilities.end(); ++itr)
			code.pushOpCapability(*itr);

		code.pushOpMemoryModel(EAddressingMode::Logical, EMemoryModel::GLSL450);
		code.pushCodeBuffer(entrypointsCode);
		code.pushCodeBuffer(executionModesCode);
		code.pushCodeBuffer(decorationsCode);
		code.pushCodeBuffer(typesCode);
		code.pushCodeBuffer(variablesCode);
		code.pushCodeBuffer(constantsCode);
		code.pushCodeBuffer(functionsCode);

		// TODO(MarcasRealAccount): Implement debug info
		// code.pushOpSource(0x0000'0008, 0x0000'0000, 0x0000'0000);

		return spirv;
	}

	std::uint32_t SPIRVTranspiler::getOrAddBuiltinType(ETypeIDs type, SPIRV& spirv, CodeBuffer& typesCode)
	{
		PROFILE_FUNC;

		std::string typeName = TypeIDToString(type);
		return getOrAddResultID(
		    typeName,
		    [&]() -> std::uint32_t
		    {
			    PROFILE_FUNC;

			    return addBuiltinType(type, spirv, typesCode);
		    });
	}

	std::pair<std::string, std::uint32_t> SPIRVTranspiler::getOrAddBuiltinTypeN(ETypeIDs type, SPIRV& spirv, CodeBuffer& typesCode)
	{
		std::string typeName = TypeIDToString(type);
		return getOrAddResultIDN(
		    typeName,
		    [&]() -> std::uint32_t
		    {
			    PROFILE_FUNC;

			    return addBuiltinType(type, spirv, typesCode);
		    });
	}

	std::uint32_t SPIRVTranspiler::addBuiltinType(ETypeIDs type, SPIRV& spirv, CodeBuffer& typesCode)
	{
		PROFILE_FUNC;

		ETypeIDs baseType = TypeIDGetBase(type);
		if (baseType == type)
		{
			// A scalar
			std::uint32_t resultID = spirv.m_IDBound++;
			switch (type)
			{
			case ETypeIDs::Void:
				typesCode.pushOpTypeVoid(resultID);
				break;
			case ETypeIDs::Bool:
				typesCode.pushOpTypeBool(resultID);
				break;
			case ETypeIDs::Byte:
				requiresCapability(ECapability::Int8);
				typesCode.pushOpTypeInt(resultID, 8, 1);
				break;
			case ETypeIDs::UByte:
				requiresCapability(ECapability::Int8);
				typesCode.pushOpTypeInt(resultID, 8, 0);
				break;
			case ETypeIDs::Short:
				requiresCapability(ECapability::Int16);
				typesCode.pushOpTypeInt(resultID, 16, 1);
				break;
			case ETypeIDs::UShort:
				requiresCapability(ECapability::Int16);
				typesCode.pushOpTypeInt(resultID, 16, 0);
				break;
			case ETypeIDs::Int:
				typesCode.pushOpTypeInt(resultID, 32, 1);
				break;
			case ETypeIDs::UInt:
				typesCode.pushOpTypeInt(resultID, 32, 0);
				break;
			case ETypeIDs::Long:
				requiresCapability(ECapability::Int64);
				typesCode.pushOpTypeInt(resultID, 64, 1);
				break;
			case ETypeIDs::ULong:
				requiresCapability(ECapability::Int64);
				typesCode.pushOpTypeInt(resultID, 46, 0);
				break;
			case ETypeIDs::Half:
				requiresCapability(ECapability::Float16);
				typesCode.pushOpTypeFloat(resultID, 16);
				break;
			case ETypeIDs::Float:
				typesCode.pushOpTypeFloat(resultID, 32);
				break;
			case ETypeIDs::Double:
				requiresCapability(ECapability::Float64);
				typesCode.pushOpTypeFloat(resultID, 64);
				break;
			}
			return resultID;
		}

		std::uint32_t baseResultID = getOrAddBuiltinType(baseType, spirv, typesCode);
		std::uint32_t resultID     = spirv.m_IDBound++;

		std::uint64_t typeID  = static_cast<std::uint64_t>(type);
		std::uint64_t rows    = (typeID >> 20) & 0b11;
		std::uint64_t columns = (typeID >> 22) & 0b11;

		if (columns == 0)
		{
			typesCode.pushOpTypeVector(resultID, baseResultID, static_cast<std::uint32_t>(rows + 1));
		}
		else if (rows == 0)
		{
			typesCode.pushOpTypeVector(resultID, baseResultID, static_cast<std::uint32_t>(columns + 1));
		}
		else
		{
			requiresCapability(ECapability::Matrix);
			typesCode.pushOpTypeMatrix(resultID, baseResultID, static_cast<std::uint32_t>(columns + 1));
		}

		return resultID;
	}

	void SPIRVTranspiler::addWarning(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		auto span             = m_Sources->getSpan(index, length, sourceID);
		auto pt               = m_Sources->getPoint(point, sourceID);
		span.m_Start.m_FileID = fileID;
		span.m_End.m_FileID   = fileID;
		pt.m_FileID           = fileID;
		m_Messages.emplace_back(EMessageType::Warning, span, pt, message.get());
	}

	void SPIRVTranspiler::addError(std::uint32_t fileID, std::uint32_t sourceID, std::size_t index, std::size_t length, std::size_t point, Utils::CopyMovable<std::string>&& message)
	{
		PROFILE_FUNC;

		auto span             = m_Sources->getSpan(index, length, sourceID);
		auto pt               = m_Sources->getPoint(point, sourceID);
		span.m_Start.m_FileID = fileID;
		span.m_End.m_FileID   = fileID;
		pt.m_FileID           = fileID;
		m_Messages.emplace_back(EMessageType::Error, span, pt, message.get());
	}
} // namespace Frertex::Transpilers::SPIRV
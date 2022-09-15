#include "Frertex/Transpilers/SPIRV/CodeBuffer.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Transpilers::SPIRV
{
	void CodeBuffer::OpNop()
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'0000);
	}

	void CodeBuffer::OpUndef(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0001);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSourceContinued(std::string_view continuedSource)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'0002 + static_cast<std::uint32_t>((literalStringLength(continuedSource)) << 16));
		pushLiteralString(continuedSource);
	}

	void CodeBuffer::OpSource(ESourceLanguage sourceLanguage, std::uint32_t version, std::uint32_t fileID, std::string_view source)
	{
		PROFILE_FUNC;

		RequireCapExtSourceLanguage(sourceLanguage, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'0003 + static_cast<std::uint32_t>((!!fileID + (!source.empty() ? literalStringLength(source) : 0)) << 16));
		m_Buffer.emplace_back(static_cast<std::uint32_t>(sourceLanguage));
		m_Buffer.emplace_back(version);
		if (fileID)
			m_Buffer.emplace_back(fileID);
		if (!source.empty())
			pushLiteralString(source);
	}

	void CodeBuffer::OpSourceExtension(std::string_view extension)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'0004 + static_cast<std::uint32_t>((literalStringLength(extension)) << 16));
		pushLiteralString(extension);
	}

	void CodeBuffer::OpName(std::uint32_t targetID, std::string_view name)
	{
		PROFILE_FUNC;

		if (targetID == 0)
			return;

		m_Buffer.emplace_back(0x0002'0005 + static_cast<std::uint32_t>((literalStringLength(name)) << 16));
		m_Buffer.emplace_back(targetID);
		pushLiteralString(name);
	}

	void CodeBuffer::OpMemberName(std::uint32_t typeID, std::uint32_t member, std::string_view name)
	{
		PROFILE_FUNC;

		if (typeID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0006 + static_cast<std::uint32_t>((literalStringLength(name)) << 16));
		m_Buffer.emplace_back(typeID);
		m_Buffer.emplace_back(member);
		pushLiteralString(name);
	}

	void CodeBuffer::OpString(std::uint32_t resultID, std::string_view string)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0002'0007 + static_cast<std::uint32_t>((literalStringLength(string)) << 16));
		m_Buffer.emplace_back(resultID);
		pushLiteralString(string);
	}

	void CodeBuffer::OpLine(std::uint32_t fileID, std::uint32_t line, std::uint32_t column)
	{
		PROFILE_FUNC;

		if (fileID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0008);
		m_Buffer.emplace_back(fileID);
		m_Buffer.emplace_back(line);
		m_Buffer.emplace_back(column);
	}

	void CodeBuffer::OpExtension(std::string_view name)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'000A + static_cast<std::uint32_t>((literalStringLength(name)) << 16));
		pushLiteralString(name);
	}

	void CodeBuffer::OpExtInstImport(std::uint32_t resultID, std::string_view name)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0002'000B + static_cast<std::uint32_t>((literalStringLength(name)) << 16));
		m_Buffer.emplace_back(resultID);
		pushLiteralString(name);
	}

	void CodeBuffer::OpExtInst(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t setID, std::uint32_t instruction, const std::vector<std::uint32_t>& operandsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || setID == 0)
			return;

		m_Buffer.emplace_back(0x0005'000C + static_cast<std::uint32_t>((operandsID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(setID);
		m_Buffer.emplace_back(instruction);
		m_Buffer.insert(m_Buffer.end(), operandsID.begin(), operandsID.end());
	}

	void CodeBuffer::OpMemoryModel(EAddressingModel addressingModel, EMemoryModel memoryModel)
	{
		PROFILE_FUNC;

		RequireCapExtAddressingModel(addressingModel, m_Capabilities, m_Extensions);
		RequireCapExtMemoryModel(memoryModel, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'000E);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(addressingModel));
		m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryModel));
	}

	void CodeBuffer::OpEntryPoint(EExecutionModel executionModel, std::uint32_t entryPointID, std::string_view name, const std::vector<std::uint32_t>& interfacesID)
	{
		PROFILE_FUNC;

		if (entryPointID == 0)
			return;

		RequireCapExtExecutionModel(executionModel, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'000F + static_cast<std::uint32_t>((literalStringLength(name) + interfacesID.size()) << 16));
		m_Buffer.emplace_back(static_cast<std::uint32_t>(executionModel));
		m_Buffer.emplace_back(entryPointID);
		pushLiteralString(name);
		m_Buffer.insert(m_Buffer.end(), interfacesID.begin(), interfacesID.end());
	}

	void CodeBuffer::OpExecutionMode(std::uint32_t entryPointID, EExecutionMode mode)
	{
		PROFILE_FUNC;

		if (entryPointID == 0)
			return;

		RequireCapExtExecutionMode(mode, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'0010);
		m_Buffer.emplace_back(entryPointID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(mode));
	}

	void CodeBuffer::OpCapability(ECapability capability)
	{
		PROFILE_FUNC;

		RequireCapExtCapability(capability, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0002'0011);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(capability));
	}

	void CodeBuffer::OpTypeVoid(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0002'0013);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeBool(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0002'0014);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeInt(std::uint32_t resultID, std::uint32_t width, std::uint32_t signedness)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0015);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(width);
		m_Buffer.emplace_back(signedness);
	}

	void CodeBuffer::OpTypeFloat(std::uint32_t resultID, std::uint32_t width)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0016);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(width);
	}

	void CodeBuffer::OpTypeVector(std::uint32_t resultID, std::uint32_t componentTypeID, std::uint32_t componentCount)
	{
		PROFILE_FUNC;

		if (resultID == 0 || componentTypeID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0017);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(componentTypeID);
		m_Buffer.emplace_back(componentCount);
	}

	void CodeBuffer::OpTypeMatrix(std::uint32_t resultID, std::uint32_t columnTypeID, std::uint32_t columnCount)
	{
		PROFILE_FUNC;

		if (resultID == 0 || columnTypeID == 0)
			return;

		requireCapability(ECapability::Matrix);

		m_Buffer.emplace_back(0x0004'0018);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(columnTypeID);
		m_Buffer.emplace_back(columnCount);
	}

	void CodeBuffer::OpTypeImage(std::uint32_t resultID, std::uint32_t sampledTypeID, EDim dim, std::uint32_t depth, std::uint32_t arrayed, std::uint32_t mS, std::uint32_t sampled, EImageFormat imageFormat, std::optional<EAccessQualifier> accessQualifier)
	{
		PROFILE_FUNC;

		if (resultID == 0 || sampledTypeID == 0)
			return;

		RequireCapExtDim(dim, m_Capabilities, m_Extensions);
		RequireCapExtImageFormat(imageFormat, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0009'0019 + static_cast<std::uint32_t>(((accessQualifier ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledTypeID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(dim));
		m_Buffer.emplace_back(depth);
		m_Buffer.emplace_back(arrayed);
		m_Buffer.emplace_back(mS);
		m_Buffer.emplace_back(sampled);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageFormat));
		if (accessQualifier)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(*accessQualifier));
	}

	void CodeBuffer::OpTypeSampler(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0002'001A);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeSampledImage(std::uint32_t resultID, std::uint32_t imageTypeID)
	{
		PROFILE_FUNC;

		if (resultID == 0 || imageTypeID == 0)
			return;

		m_Buffer.emplace_back(0x0003'001B);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageTypeID);
	}

	void CodeBuffer::OpTypeArray(std::uint32_t resultID, std::uint32_t elementTypeID, std::uint32_t lengthID)
	{
		PROFILE_FUNC;

		if (resultID == 0 || elementTypeID == 0 || lengthID == 0)
			return;

		m_Buffer.emplace_back(0x0004'001C);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(elementTypeID);
		m_Buffer.emplace_back(lengthID);
	}

	void CodeBuffer::OpTypeRuntimeArray(std::uint32_t resultID, std::uint32_t elementTypeID)
	{
		PROFILE_FUNC;

		if (resultID == 0 || elementTypeID == 0)
			return;

		requireCapability(ECapability::Shader);

		m_Buffer.emplace_back(0x0003'001D);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(elementTypeID);
	}

	void CodeBuffer::OpTypeStruct(std::uint32_t resultID, const std::vector<std::uint32_t>& membersID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0002'001E + static_cast<std::uint32_t>((membersID.size()) << 16));
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), membersID.begin(), membersID.end());
	}

	void CodeBuffer::OpTypeOpaque(std::uint32_t resultID, std::string_view henameoftheopaquetype)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0002'001F + static_cast<std::uint32_t>((literalStringLength(henameoftheopaquetype)) << 16));
		m_Buffer.emplace_back(resultID);
		pushLiteralString(henameoftheopaquetype);
	}

	void CodeBuffer::OpTypePointer(std::uint32_t resultID, EStorageClass storageClass, std::uint32_t typeID)
	{
		PROFILE_FUNC;

		if (resultID == 0 || typeID == 0)
			return;

		RequireCapExtStorageClass(storageClass, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0004'0020);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(storageClass));
		m_Buffer.emplace_back(typeID);
	}

	void CodeBuffer::OpTypeFunction(std::uint32_t resultID, std::uint32_t returnTypeID, const std::vector<std::uint32_t>& parametersID)
	{
		PROFILE_FUNC;

		if (resultID == 0 || returnTypeID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0021 + static_cast<std::uint32_t>((parametersID.size()) << 16));
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(returnTypeID);
		m_Buffer.insert(m_Buffer.end(), parametersID.begin(), parametersID.end());
	}

	void CodeBuffer::OpTypeEvent(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0002'0022);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeDeviceEvent(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0002'0023);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeReserveId(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0002'0024);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeQueue(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0002'0025);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypePipe(std::uint32_t resultID, EAccessQualifier qualifier)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::Pipes);
		RequireCapExtAccessQualifier(qualifier, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'0026);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(qualifier));
	}

	void CodeBuffer::OpTypeForwardPointer(std::uint32_t pointerTypeID, EStorageClass storageClass)
	{
		PROFILE_FUNC;

		if (pointerTypeID == 0)
			return;

		requireCapability(ECapability::Addresses);
		requireCapability(ECapability::PhysicalStorageBufferAddresses);
		RequireCapExtStorageClass(storageClass, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'0027);
		m_Buffer.emplace_back(pointerTypeID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(storageClass));
	}

	void CodeBuffer::OpConstantTrue(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0029);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpConstantFalse(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'002A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpConstant(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& value)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'002B + static_cast<std::uint32_t>((value.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), value.begin(), value.end());
	}

	void CodeBuffer::OpConstantComposite(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& constituentsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'002C + static_cast<std::uint32_t>((constituentsID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), constituentsID.begin(), constituentsID.end());
	}

	void CodeBuffer::OpConstantSampler(std::uint32_t resultTypeID, std::uint32_t resultID, ESamplerAddressingMode samplerAddressingMode, std::uint32_t param, ESamplerFilterMode samplerFilterMode)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::LiteralSampler);
		RequireCapExtSamplerAddressingMode(samplerAddressingMode, m_Capabilities, m_Extensions);
		RequireCapExtSamplerFilterMode(samplerFilterMode, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'002D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(samplerAddressingMode));
		m_Buffer.emplace_back(param);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(samplerFilterMode));
	}

	void CodeBuffer::OpConstantNull(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'002E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSpecConstantTrue(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0030);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSpecConstantFalse(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0031);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSpecConstant(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& value)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0032 + static_cast<std::uint32_t>((value.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), value.begin(), value.end());
	}

	void CodeBuffer::OpSpecConstantComposite(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& constituentsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0033 + static_cast<std::uint32_t>((constituentsID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), constituentsID.begin(), constituentsID.end());
	}

	void CodeBuffer::OpSpecConstantOp(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t opcode)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0034);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(opcode);
	}

	void CodeBuffer::OpFunction(std::uint32_t resultTypeID, std::uint32_t resultID, EFunctionControl functionControl, std::uint32_t functionTypeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || functionTypeID == 0)
			return;

		RequireCapExtFunctionControl(functionControl, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'0036);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(functionControl));
		m_Buffer.emplace_back(functionTypeID);
	}

	void CodeBuffer::OpFunctionParameter(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0037);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpFunctionEnd()
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'0038);
	}

	void CodeBuffer::OpFunctionCall(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t functionID, const std::vector<std::uint32_t>& argumentsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || functionID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0039 + static_cast<std::uint32_t>((argumentsID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(functionID);
		m_Buffer.insert(m_Buffer.end(), argumentsID.begin(), argumentsID.end());
	}

	void CodeBuffer::OpVariable(std::uint32_t resultTypeID, std::uint32_t resultID, EStorageClass storageClass, std::uint32_t initializerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		RequireCapExtStorageClass(storageClass, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0004'003B + static_cast<std::uint32_t>((!!initializerID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(storageClass));
		if (initializerID)
			m_Buffer.emplace_back(initializerID);
	}

	void CodeBuffer::OpImageTexelPointer(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t sampleID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || coordinateID == 0 || sampleID == 0)
			return;

		m_Buffer.emplace_back(0x0006'003C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(sampleID);
	}

	void CodeBuffer::OpLoad(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, EMemoryAccess memoryAccess)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0)
			return;

		if (memoryAccess != EMemoryAccess::None)
			RequireCapExtMemoryAccess(memoryAccess, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0004'003D + static_cast<std::uint32_t>(((memoryAccess != EMemoryAccess::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		if (memoryAccess != EMemoryAccess::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryAccess));
	}

	void CodeBuffer::OpStore(std::uint32_t pointerID, std::uint32_t objectID, EMemoryAccess memoryAccess)
	{
		PROFILE_FUNC;

		if (pointerID == 0 || objectID == 0)
			return;

		if (memoryAccess != EMemoryAccess::None)
			RequireCapExtMemoryAccess(memoryAccess, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'003E + static_cast<std::uint32_t>(((memoryAccess != EMemoryAccess::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(objectID);
		if (memoryAccess != EMemoryAccess::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryAccess));
	}

	void CodeBuffer::OpCopyMemory(std::uint32_t targetID, std::uint32_t sourceID, EMemoryAccess memoryAccess, EMemoryAccess memoryAccess0)
	{
		PROFILE_FUNC;

		if (targetID == 0 || sourceID == 0)
			return;

		if (memoryAccess != EMemoryAccess::None)
			RequireCapExtMemoryAccess(memoryAccess, m_Capabilities, m_Extensions);
		if (memoryAccess0 != EMemoryAccess::None)
			RequireCapExtMemoryAccess(memoryAccess0, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'003F + static_cast<std::uint32_t>(((memoryAccess != EMemoryAccess::None ? 1 : 0) + (memoryAccess0 != EMemoryAccess::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(targetID);
		m_Buffer.emplace_back(sourceID);
		if (memoryAccess != EMemoryAccess::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryAccess));
		if (memoryAccess0 != EMemoryAccess::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryAccess0));
	}

	void CodeBuffer::OpCopyMemorySized(std::uint32_t targetID, std::uint32_t sourceID, std::uint32_t sizeID, EMemoryAccess memoryAccess, EMemoryAccess memoryAccess0)
	{
		PROFILE_FUNC;

		if (targetID == 0 || sourceID == 0 || sizeID == 0)
			return;

		requireCapability(ECapability::Addresses);
		if (memoryAccess != EMemoryAccess::None)
			RequireCapExtMemoryAccess(memoryAccess, m_Capabilities, m_Extensions);
		if (memoryAccess0 != EMemoryAccess::None)
			RequireCapExtMemoryAccess(memoryAccess0, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0004'0040 + static_cast<std::uint32_t>(((memoryAccess != EMemoryAccess::None ? 1 : 0) + (memoryAccess0 != EMemoryAccess::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(targetID);
		m_Buffer.emplace_back(sourceID);
		m_Buffer.emplace_back(sizeID);
		if (memoryAccess != EMemoryAccess::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryAccess));
		if (memoryAccess0 != EMemoryAccess::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryAccess0));
	}

	void CodeBuffer::OpAccessChain(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, const std::vector<std::uint32_t>& indexesID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0041 + static_cast<std::uint32_t>((indexesID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.insert(m_Buffer.end(), indexesID.begin(), indexesID.end());
	}

	void CodeBuffer::OpInBoundsAccessChain(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, const std::vector<std::uint32_t>& indexesID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0042 + static_cast<std::uint32_t>((indexesID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.insert(m_Buffer.end(), indexesID.begin(), indexesID.end());
	}

	void CodeBuffer::OpPtrAccessChain(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t elementID, const std::vector<std::uint32_t>& indexesID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0 || elementID == 0)
			return;

		requireCapability(ECapability::Addresses);
		requireCapability(ECapability::VariablePointers);
		requireCapability(ECapability::VariablePointersStorageBuffer);
		requireCapability(ECapability::PhysicalStorageBufferAddresses);

		m_Buffer.emplace_back(0x0005'0043 + static_cast<std::uint32_t>((indexesID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.emplace_back(elementID);
		m_Buffer.insert(m_Buffer.end(), indexesID.begin(), indexesID.end());
	}

	void CodeBuffer::OpArrayLength(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t structureID, std::uint32_t arraymember)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || structureID == 0)
			return;

		requireCapability(ECapability::Shader);

		m_Buffer.emplace_back(0x0005'0044);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(structureID);
		m_Buffer.emplace_back(arraymember);
	}

	void CodeBuffer::OpGenericPtrMemSemantics(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'0045);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
	}

	void CodeBuffer::OpInBoundsPtrAccessChain(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t elementID, const std::vector<std::uint32_t>& indexesID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0 || elementID == 0)
			return;

		requireCapability(ECapability::Addresses);

		m_Buffer.emplace_back(0x0005'0046 + static_cast<std::uint32_t>((indexesID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.emplace_back(elementID);
		m_Buffer.insert(m_Buffer.end(), indexesID.begin(), indexesID.end());
	}

	void CodeBuffer::OpDecorate(std::uint32_t targetID, EDecoration decoration)
	{
		PROFILE_FUNC;

		if (targetID == 0)
			return;

		RequireCapExtDecoration(decoration, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'0047);
		m_Buffer.emplace_back(targetID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(decoration));
	}

	void CodeBuffer::OpMemberDecorate(std::uint32_t structureTypeID, std::uint32_t member, EDecoration decoration)
	{
		PROFILE_FUNC;

		if (structureTypeID == 0)
			return;

		RequireCapExtDecoration(decoration, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0004'0048);
		m_Buffer.emplace_back(structureTypeID);
		m_Buffer.emplace_back(member);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(decoration));
	}

	void CodeBuffer::OpDecorationGroup(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0002'0049);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpGroupDecorate(std::uint32_t decorationGroupID, const std::vector<std::uint32_t>& targetsID)
	{
		PROFILE_FUNC;

		if (decorationGroupID == 0)
			return;

		m_Buffer.emplace_back(0x0002'004A + static_cast<std::uint32_t>((targetsID.size()) << 16));
		m_Buffer.emplace_back(decorationGroupID);
		m_Buffer.insert(m_Buffer.end(), targetsID.begin(), targetsID.end());
	}

	void CodeBuffer::OpGroupMemberDecorate(std::uint32_t decorationGroupID, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& targets)
	{
		PROFILE_FUNC;

		if (decorationGroupID == 0)
			return;

		m_Buffer.emplace_back(0x0002'004B + static_cast<std::uint32_t>((targets.size() * 2) << 16));
		m_Buffer.emplace_back(decorationGroupID);
		m_Buffer.reserve(m_Buffer.size() + targets.size() * 2);
		for (auto& v : targets)
		{
			m_Buffer.emplace_back(v.first);
			m_Buffer.emplace_back(v.second);
		}
	}

	void CodeBuffer::OpVectorExtractDynamic(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID, std::uint32_t indexID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vectorID == 0 || indexID == 0)
			return;

		m_Buffer.emplace_back(0x0005'004D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vectorID);
		m_Buffer.emplace_back(indexID);
	}

	void CodeBuffer::OpVectorInsertDynamic(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID, std::uint32_t componentID, std::uint32_t indexID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vectorID == 0 || componentID == 0 || indexID == 0)
			return;

		m_Buffer.emplace_back(0x0006'004E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vectorID);
		m_Buffer.emplace_back(componentID);
		m_Buffer.emplace_back(indexID);
	}

	void CodeBuffer::OpVectorShuffle(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, const std::vector<std::uint32_t>& components)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vector1ID == 0 || vector2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'004F + static_cast<std::uint32_t>((components.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vector1ID);
		m_Buffer.emplace_back(vector2ID);
		m_Buffer.insert(m_Buffer.end(), components.begin(), components.end());
	}

	void CodeBuffer::OpCompositeConstruct(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& constituentsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'0050 + static_cast<std::uint32_t>((constituentsID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), constituentsID.begin(), constituentsID.end());
	}

	void CodeBuffer::OpCompositeExtract(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t compositeID, const std::vector<std::uint32_t>& indexes)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || compositeID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0051 + static_cast<std::uint32_t>((indexes.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(compositeID);
		m_Buffer.insert(m_Buffer.end(), indexes.begin(), indexes.end());
	}

	void CodeBuffer::OpCompositeInsert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t objectID, std::uint32_t compositeID, const std::vector<std::uint32_t>& indexes)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || objectID == 0 || compositeID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0052 + static_cast<std::uint32_t>((indexes.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(objectID);
		m_Buffer.emplace_back(compositeID);
		m_Buffer.insert(m_Buffer.end(), indexes.begin(), indexes.end());
	}

	void CodeBuffer::OpCopyObject(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0053);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpTranspose(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t matrixID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || matrixID == 0)
			return;

		requireCapability(ECapability::Matrix);

		m_Buffer.emplace_back(0x0004'0054);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(matrixID);
	}

	void CodeBuffer::OpSampledImage(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t samplerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || samplerID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0056);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(samplerID);
	}

	void CodeBuffer::OpImageSampleImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::Shader);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'0057 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSampleExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0)
			return;

		RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0058);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSampleDrefImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::Shader);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0059 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSampleDrefExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::Shader);
		RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0007'005A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSampleProjImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::Shader);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'005B + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSampleProjExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::Shader);
		RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'005C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSampleProjDrefImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::Shader);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'005D + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSampleProjDrefExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::Shader);
		RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0007'005E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageFetch(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || coordinateID == 0)
			return;

		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'005F + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageGather(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t componentID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || componentID == 0)
			return;

		requireCapability(ECapability::Shader);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0060 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(componentID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageDrefGather(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::Shader);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0061 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageRead(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || coordinateID == 0)
			return;

		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'0062 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageWrite(std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t texelID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (imageID == 0 || coordinateID == 0 || texelID == 0)
			return;

		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0004'0063 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(texelID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImage(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0064);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
	}

	void CodeBuffer::OpImageQueryFormat(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'0065);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
	}

	void CodeBuffer::OpImageQueryOrder(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'0066);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
	}

	void CodeBuffer::OpImageQuerySizeLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t levelofDetailID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || levelofDetailID == 0)
			return;

		requireCapability(ECapability::Kernel);
		requireCapability(ECapability::ImageQuery);

		m_Buffer.emplace_back(0x0005'0067);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(levelofDetailID);
	}

	void CodeBuffer::OpImageQuerySize(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0)
			return;

		requireCapability(ECapability::Kernel);
		requireCapability(ECapability::ImageQuery);

		m_Buffer.emplace_back(0x0004'0068);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
	}

	void CodeBuffer::OpImageQueryLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::ImageQuery);

		m_Buffer.emplace_back(0x0005'0069);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
	}

	void CodeBuffer::OpImageQueryLevels(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0)
			return;

		requireCapability(ECapability::Kernel);
		requireCapability(ECapability::ImageQuery);

		m_Buffer.emplace_back(0x0004'006A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
	}

	void CodeBuffer::OpImageQuerySamples(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0)
			return;

		requireCapability(ECapability::Kernel);
		requireCapability(ECapability::ImageQuery);

		m_Buffer.emplace_back(0x0004'006B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
	}

	void CodeBuffer::OpConvertFToU(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t floatValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || floatValueID == 0)
			return;

		m_Buffer.emplace_back(0x0004'006D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(floatValueID);
	}

	void CodeBuffer::OpConvertFToS(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t floatValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || floatValueID == 0)
			return;

		m_Buffer.emplace_back(0x0004'006E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(floatValueID);
	}

	void CodeBuffer::OpConvertSToF(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t signedValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || signedValueID == 0)
			return;

		m_Buffer.emplace_back(0x0004'006F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(signedValueID);
	}

	void CodeBuffer::OpConvertUToF(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t unsignedValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || unsignedValueID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0070);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(unsignedValueID);
	}

	void CodeBuffer::OpUConvert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t unsignedValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || unsignedValueID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0071);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(unsignedValueID);
	}

	void CodeBuffer::OpSConvert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t signedValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || signedValueID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0072);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(signedValueID);
	}

	void CodeBuffer::OpFConvert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t floatValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || floatValueID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0073);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(floatValueID);
	}

	void CodeBuffer::OpQuantizeToF16(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0074);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpConvertPtrToU(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0)
			return;

		requireCapability(ECapability::Addresses);
		requireCapability(ECapability::PhysicalStorageBufferAddresses);

		m_Buffer.emplace_back(0x0004'0075);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
	}

	void CodeBuffer::OpSatConvertSToU(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t signedValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || signedValueID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'0076);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(signedValueID);
	}

	void CodeBuffer::OpSatConvertUToS(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t unsignedValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || unsignedValueID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'0077);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(unsignedValueID);
	}

	void CodeBuffer::OpConvertUToPtr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t integerValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || integerValueID == 0)
			return;

		requireCapability(ECapability::Addresses);
		requireCapability(ECapability::PhysicalStorageBufferAddresses);

		m_Buffer.emplace_back(0x0004'0078);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(integerValueID);
	}

	void CodeBuffer::OpPtrCastToGeneric(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'0079);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
	}

	void CodeBuffer::OpGenericCastToPtr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'007A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
	}

	void CodeBuffer::OpGenericCastToPtrExplicit(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, EStorageClass storage)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0)
			return;

		requireCapability(ECapability::Kernel);
		RequireCapExtStorageClass(storage, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'007B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(storage));
	}

	void CodeBuffer::OpBitcast(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		m_Buffer.emplace_back(0x0004'007C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpSNegate(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		m_Buffer.emplace_back(0x0004'007E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpFNegate(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		m_Buffer.emplace_back(0x0004'007F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpIAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0080);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0081);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpISub(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0082);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFSub(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0083);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpIMul(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0084);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFMul(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0085);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUDiv(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0086);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpSDiv(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0087);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFDiv(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0088);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUMod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0089);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpSRem(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'008A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpSMod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'008B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFRem(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'008C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFMod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'008D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpVectorTimesScalar(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID, std::uint32_t scalarID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vectorID == 0 || scalarID == 0)
			return;

		m_Buffer.emplace_back(0x0005'008E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vectorID);
		m_Buffer.emplace_back(scalarID);
	}

	void CodeBuffer::OpMatrixTimesScalar(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t matrixID, std::uint32_t scalarID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || matrixID == 0 || scalarID == 0)
			return;

		requireCapability(ECapability::Matrix);

		m_Buffer.emplace_back(0x0005'008F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(matrixID);
		m_Buffer.emplace_back(scalarID);
	}

	void CodeBuffer::OpVectorTimesMatrix(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID, std::uint32_t matrixID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vectorID == 0 || matrixID == 0)
			return;

		requireCapability(ECapability::Matrix);

		m_Buffer.emplace_back(0x0005'0090);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vectorID);
		m_Buffer.emplace_back(matrixID);
	}

	void CodeBuffer::OpMatrixTimesVector(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t matrixID, std::uint32_t vectorID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || matrixID == 0 || vectorID == 0)
			return;

		requireCapability(ECapability::Matrix);

		m_Buffer.emplace_back(0x0005'0091);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(matrixID);
		m_Buffer.emplace_back(vectorID);
	}

	void CodeBuffer::OpMatrixTimesMatrix(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t leftMatrixID, std::uint32_t rightMatrixID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || leftMatrixID == 0 || rightMatrixID == 0)
			return;

		requireCapability(ECapability::Matrix);

		m_Buffer.emplace_back(0x0005'0092);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(leftMatrixID);
		m_Buffer.emplace_back(rightMatrixID);
	}

	void CodeBuffer::OpOuterProduct(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vector1ID == 0 || vector2ID == 0)
			return;

		requireCapability(ECapability::Matrix);

		m_Buffer.emplace_back(0x0005'0093);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vector1ID);
		m_Buffer.emplace_back(vector2ID);
	}

	void CodeBuffer::OpDot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vector1ID == 0 || vector2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0094);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vector1ID);
		m_Buffer.emplace_back(vector2ID);
	}

	void CodeBuffer::OpIAddCarry(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0095);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpISubBorrow(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0096);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUMulExtended(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0097);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpSMulExtended(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0098);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpAny(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vectorID == 0)
			return;

		m_Buffer.emplace_back(0x0004'009A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vectorID);
	}

	void CodeBuffer::OpAll(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vectorID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vectorID == 0)
			return;

		m_Buffer.emplace_back(0x0004'009B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vectorID);
	}

	void CodeBuffer::OpIsNan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || xID == 0)
			return;

		m_Buffer.emplace_back(0x0004'009C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpIsInf(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || xID == 0)
			return;

		m_Buffer.emplace_back(0x0004'009D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpIsFinite(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'009E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpIsNormal(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'009F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpSignBitSet(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'00A0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpLessOrGreater(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID, std::uint32_t yID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || xID == 0 || yID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0005'00A1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(xID);
		m_Buffer.emplace_back(yID);
	}

	void CodeBuffer::OpOrdered(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID, std::uint32_t yID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || xID == 0 || yID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0005'00A2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(xID);
		m_Buffer.emplace_back(yID);
	}

	void CodeBuffer::OpUnordered(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t xID, std::uint32_t yID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || xID == 0 || yID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0005'00A3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(xID);
		m_Buffer.emplace_back(yID);
	}

	void CodeBuffer::OpLogicalEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00A4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpLogicalNotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00A5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpLogicalOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00A6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpLogicalAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00A7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpLogicalNot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		m_Buffer.emplace_back(0x0004'00A8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpSelect(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t conditionID, std::uint32_t object1ID, std::uint32_t object2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || conditionID == 0 || object1ID == 0 || object2ID == 0)
			return;

		m_Buffer.emplace_back(0x0006'00A9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(conditionID);
		m_Buffer.emplace_back(object1ID);
		m_Buffer.emplace_back(object2ID);
	}

	void CodeBuffer::OpIEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00AA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpINotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00AB);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUGreaterThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00AC);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpSGreaterThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00AD);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUGreaterThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00AE);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpSGreaterThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00AF);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpULessThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpSLessThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpULessThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpSLessThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFOrdEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFUnordEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFOrdNotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFUnordNotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFOrdLessThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFUnordLessThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00B9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFOrdGreaterThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00BA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFUnordGreaterThan(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00BB);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFOrdLessThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00BC);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFUnordLessThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00BD);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFOrdGreaterThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00BE);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpFUnordGreaterThanEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00BF);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpShiftRightLogical(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t shiftID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0 || shiftID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00C2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.emplace_back(shiftID);
	}

	void CodeBuffer::OpShiftRightArithmetic(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t shiftID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0 || shiftID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00C3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.emplace_back(shiftID);
	}

	void CodeBuffer::OpShiftLeftLogical(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t shiftID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0 || shiftID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00C4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.emplace_back(shiftID);
	}

	void CodeBuffer::OpBitwiseOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00C5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpBitwiseXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00C6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpBitwiseAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00C7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpNot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		m_Buffer.emplace_back(0x0004'00C8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpBitFieldInsert(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t insertID, std::uint32_t offsetID, std::uint32_t countID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0 || insertID == 0 || offsetID == 0 || countID == 0)
			return;

		requireCapability(ECapability::Shader);
		requireCapability(ECapability::BitInstructions);

		m_Buffer.emplace_back(0x0007'00C9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.emplace_back(insertID);
		m_Buffer.emplace_back(offsetID);
		m_Buffer.emplace_back(countID);
	}

	void CodeBuffer::OpBitFieldSExtract(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t offsetID, std::uint32_t countID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0 || offsetID == 0 || countID == 0)
			return;

		requireCapability(ECapability::Shader);
		requireCapability(ECapability::BitInstructions);

		m_Buffer.emplace_back(0x0006'00CA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.emplace_back(offsetID);
		m_Buffer.emplace_back(countID);
	}

	void CodeBuffer::OpBitFieldUExtract(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID, std::uint32_t offsetID, std::uint32_t countID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0 || offsetID == 0 || countID == 0)
			return;

		requireCapability(ECapability::Shader);
		requireCapability(ECapability::BitInstructions);

		m_Buffer.emplace_back(0x0006'00CB);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
		m_Buffer.emplace_back(offsetID);
		m_Buffer.emplace_back(countID);
	}

	void CodeBuffer::OpBitReverse(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0)
			return;

		requireCapability(ECapability::Shader);
		requireCapability(ECapability::BitInstructions);

		m_Buffer.emplace_back(0x0004'00CC);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
	}

	void CodeBuffer::OpBitCount(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t baseID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || baseID == 0)
			return;

		m_Buffer.emplace_back(0x0004'00CD);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(baseID);
	}

	void CodeBuffer::OpDPdx(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pID == 0)
			return;

		requireCapability(ECapability::Shader);

		m_Buffer.emplace_back(0x0004'00CF);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pID);
	}

	void CodeBuffer::OpDPdy(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pID == 0)
			return;

		requireCapability(ECapability::Shader);

		m_Buffer.emplace_back(0x0004'00D0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pID);
	}

	void CodeBuffer::OpFwidth(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pID == 0)
			return;

		requireCapability(ECapability::Shader);

		m_Buffer.emplace_back(0x0004'00D1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pID);
	}

	void CodeBuffer::OpDPdxFine(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pID == 0)
			return;

		requireCapability(ECapability::DerivativeControl);

		m_Buffer.emplace_back(0x0004'00D2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pID);
	}

	void CodeBuffer::OpDPdyFine(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pID == 0)
			return;

		requireCapability(ECapability::DerivativeControl);

		m_Buffer.emplace_back(0x0004'00D3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pID);
	}

	void CodeBuffer::OpFwidthFine(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pID == 0)
			return;

		requireCapability(ECapability::DerivativeControl);

		m_Buffer.emplace_back(0x0004'00D4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pID);
	}

	void CodeBuffer::OpDPdxCoarse(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pID == 0)
			return;

		requireCapability(ECapability::DerivativeControl);

		m_Buffer.emplace_back(0x0004'00D5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pID);
	}

	void CodeBuffer::OpDPdyCoarse(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pID == 0)
			return;

		requireCapability(ECapability::DerivativeControl);

		m_Buffer.emplace_back(0x0004'00D6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pID);
	}

	void CodeBuffer::OpFwidthCoarse(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pID == 0)
			return;

		requireCapability(ECapability::DerivativeControl);

		m_Buffer.emplace_back(0x0004'00D7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pID);
	}

	void CodeBuffer::OpEmitVertex()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Geometry);

		m_Buffer.emplace_back(0x0001'00DA);
	}

	void CodeBuffer::OpEndPrimitive()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Geometry);

		m_Buffer.emplace_back(0x0001'00DB);
	}

	void CodeBuffer::OpEmitStreamVertex(std::uint32_t streamID)
	{
		PROFILE_FUNC;

		if (streamID == 0)
			return;

		requireCapability(ECapability::GeometryStreams);

		m_Buffer.emplace_back(0x0002'00DC);
		m_Buffer.emplace_back(streamID);
	}

	void CodeBuffer::OpEndStreamPrimitive(std::uint32_t streamID)
	{
		PROFILE_FUNC;

		if (streamID == 0)
			return;

		requireCapability(ECapability::GeometryStreams);

		m_Buffer.emplace_back(0x0002'00DD);
		m_Buffer.emplace_back(streamID);
	}

	void CodeBuffer::OpControlBarrier(std::uint32_t executionID, std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (executionID == 0 || memoryID == 0 || semanticsID == 0)
			return;

		m_Buffer.emplace_back(0x0004'00E0);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpMemoryBarrier(std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (memoryID == 0 || semanticsID == 0)
			return;

		m_Buffer.emplace_back(0x0003'00E1);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpAtomicLoad(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0)
			return;

		m_Buffer.emplace_back(0x0006'00E3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpAtomicStore(std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0005'00E4);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicExchange(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00E5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicCompareExchange(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t equalID, std::uint32_t unequalID, std::uint32_t valueID, std::uint32_t comparatorID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || equalID == 0 || unequalID == 0 || valueID == 0 || comparatorID == 0)
			return;

		m_Buffer.emplace_back(0x0009'00E6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(equalID);
		m_Buffer.emplace_back(unequalID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(comparatorID);
	}

	void CodeBuffer::OpAtomicCompareExchangeWeak(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t equalID, std::uint32_t unequalID, std::uint32_t valueID, std::uint32_t comparatorID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || equalID == 0 || unequalID == 0 || valueID == 0 || comparatorID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0009'00E7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(equalID);
		m_Buffer.emplace_back(unequalID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(comparatorID);
	}

	void CodeBuffer::OpAtomicIIncrement(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0)
			return;

		m_Buffer.emplace_back(0x0006'00E8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpAtomicIDecrement(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0)
			return;

		m_Buffer.emplace_back(0x0006'00E9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpAtomicIAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00EA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicISub(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00EB);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicSMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00EC);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicUMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00ED);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicSMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00EE);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicUMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00EF);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00F0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00F1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		m_Buffer.emplace_back(0x0007'00F2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpPhi(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& variableParents)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		m_Buffer.emplace_back(0x0003'00F5 + static_cast<std::uint32_t>((variableParents.size() * 2) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.reserve(m_Buffer.size() + variableParents.size() * 2);
		for (auto& v : variableParents)
		{
			m_Buffer.emplace_back(v.first);
			m_Buffer.emplace_back(v.second);
		}
	}

	void CodeBuffer::OpLoopMerge(std::uint32_t mergeBlockID, std::uint32_t continueTargetID, ELoopControl loopControl)
	{
		PROFILE_FUNC;

		if (mergeBlockID == 0 || continueTargetID == 0)
			return;

		RequireCapExtLoopControl(loopControl, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0004'00F6);
		m_Buffer.emplace_back(mergeBlockID);
		m_Buffer.emplace_back(continueTargetID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(loopControl));
	}

	void CodeBuffer::OpSelectionMerge(std::uint32_t mergeBlockID, ESelectionControl selectionControl)
	{
		PROFILE_FUNC;

		if (mergeBlockID == 0)
			return;

		RequireCapExtSelectionControl(selectionControl, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'00F7);
		m_Buffer.emplace_back(mergeBlockID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(selectionControl));
	}

	void CodeBuffer::OpLabel(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		m_Buffer.emplace_back(0x0002'00F8);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpBranch(std::uint32_t targetLabelID)
	{
		PROFILE_FUNC;

		if (targetLabelID == 0)
			return;

		m_Buffer.emplace_back(0x0002'00F9);
		m_Buffer.emplace_back(targetLabelID);
	}

	void CodeBuffer::OpBranchConditional(std::uint32_t conditionID, std::uint32_t trueLabelID, std::uint32_t falseLabelID, const std::vector<std::uint32_t>& branchweights)
	{
		PROFILE_FUNC;

		if (conditionID == 0 || trueLabelID == 0 || falseLabelID == 0)
			return;

		m_Buffer.emplace_back(0x0004'00FA + static_cast<std::uint32_t>((branchweights.size()) << 16));
		m_Buffer.emplace_back(conditionID);
		m_Buffer.emplace_back(trueLabelID);
		m_Buffer.emplace_back(falseLabelID);
		m_Buffer.insert(m_Buffer.end(), branchweights.begin(), branchweights.end());
	}

	void CodeBuffer::OpSwitch(std::uint32_t selectorID, std::uint32_t defID, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& targets)
	{
		PROFILE_FUNC;

		if (selectorID == 0 || defID == 0)
			return;

		m_Buffer.emplace_back(0x0003'00FB + static_cast<std::uint32_t>((targets.size() * 2) << 16));
		m_Buffer.emplace_back(selectorID);
		m_Buffer.emplace_back(defID);
		m_Buffer.reserve(m_Buffer.size() + targets.size() * 2);
		for (auto& v : targets)
		{
			m_Buffer.emplace_back(v.first);
			m_Buffer.emplace_back(v.second);
		}
	}

	void CodeBuffer::OpKill()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Shader);

		m_Buffer.emplace_back(0x0001'00FC);
	}

	void CodeBuffer::OpReturn()
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'00FD);
	}

	void CodeBuffer::OpReturnValue(std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (valueID == 0)
			return;

		m_Buffer.emplace_back(0x0002'00FE);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpUnreachable()
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'00FF);
	}

	void CodeBuffer::OpLifetimeStart(std::uint32_t pointerID, std::uint32_t size)
	{
		PROFILE_FUNC;

		if (pointerID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0003'0100);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(size);
	}

	void CodeBuffer::OpLifetimeStop(std::uint32_t pointerID, std::uint32_t size)
	{
		PROFILE_FUNC;

		if (pointerID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0003'0101);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(size);
	}

	void CodeBuffer::OpGroupAsyncCopy(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t destinationID, std::uint32_t sourceID, std::uint32_t numElementsID, std::uint32_t strideID, std::uint32_t eventID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || destinationID == 0 || sourceID == 0 || numElementsID == 0 || strideID == 0 || eventID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0009'0103);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(destinationID);
		m_Buffer.emplace_back(sourceID);
		m_Buffer.emplace_back(numElementsID);
		m_Buffer.emplace_back(strideID);
		m_Buffer.emplace_back(eventID);
	}

	void CodeBuffer::OpGroupWaitEvents(std::uint32_t executionID, std::uint32_t numEventsID, std::uint32_t eventsListID)
	{
		PROFILE_FUNC;

		if (executionID == 0 || numEventsID == 0 || eventsListID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'0104);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(numEventsID);
		m_Buffer.emplace_back(eventsListID);
	}

	void CodeBuffer::OpGroupAll(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || predicateID == 0)
			return;

		requireCapability(ECapability::Groups);

		m_Buffer.emplace_back(0x0005'0105);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(predicateID);
	}

	void CodeBuffer::OpGroupAny(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || predicateID == 0)
			return;

		requireCapability(ECapability::Groups);

		m_Buffer.emplace_back(0x0005'0106);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(predicateID);
	}

	void CodeBuffer::OpGroupBroadcast(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t localIdID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || localIdID == 0)
			return;

		requireCapability(ECapability::Groups);

		m_Buffer.emplace_back(0x0006'0107);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(localIdID);
	}

	void CodeBuffer::OpGroupIAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0108);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupFAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0109);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupFMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'010A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupUMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'010B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupSMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'010C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupFMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'010D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupUMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'010E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupSMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'010F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpReadPipe(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t pointerID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pipeID == 0 || pointerID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0007'0112);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpWritePipe(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t pointerID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pipeID == 0 || pointerID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0007'0113);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpReservedReadPipe(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t indexID, std::uint32_t pointerID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pipeID == 0 || reserveIdID == 0 || indexID == 0 || pointerID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0009'0114);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(reserveIdID);
		m_Buffer.emplace_back(indexID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpReservedWritePipe(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t indexID, std::uint32_t pointerID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pipeID == 0 || reserveIdID == 0 || indexID == 0 || pointerID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0009'0115);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(reserveIdID);
		m_Buffer.emplace_back(indexID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpReserveReadPipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t numPacketsID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pipeID == 0 || numPacketsID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0007'0116);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(numPacketsID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpReserveWritePipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t numPacketsID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pipeID == 0 || numPacketsID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0007'0117);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(numPacketsID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpCommitReadPipe(std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (pipeID == 0 || reserveIdID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0005'0118);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(reserveIdID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpCommitWritePipe(std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (pipeID == 0 || reserveIdID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0005'0119);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(reserveIdID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpIsValidReserveId(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t reserveIdID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || reserveIdID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0004'011A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(reserveIdID);
	}

	void CodeBuffer::OpGetNumPipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pipeID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0006'011B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpGetMaxPipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pipeID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0006'011C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpGroupReserveReadPipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t pipeID, std::uint32_t numPacketsID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || pipeID == 0 || numPacketsID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0008'011D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(numPacketsID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpGroupReserveWritePipePackets(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t pipeID, std::uint32_t numPacketsID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || pipeID == 0 || numPacketsID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0008'011E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(numPacketsID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpGroupCommitReadPipe(std::uint32_t executionID, std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (executionID == 0 || pipeID == 0 || reserveIdID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0006'011F);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(reserveIdID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpGroupCommitWritePipe(std::uint32_t executionID, std::uint32_t pipeID, std::uint32_t reserveIdID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (executionID == 0 || pipeID == 0 || reserveIdID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0006'0120);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(pipeID);
		m_Buffer.emplace_back(reserveIdID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpEnqueueMarker(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t queueID, std::uint32_t numEventsID, std::uint32_t waitEventsID, std::uint32_t retEventID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || queueID == 0 || numEventsID == 0 || waitEventsID == 0 || retEventID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0007'0123);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(queueID);
		m_Buffer.emplace_back(numEventsID);
		m_Buffer.emplace_back(waitEventsID);
		m_Buffer.emplace_back(retEventID);
	}

	void CodeBuffer::OpEnqueueKernel(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t queueID, std::uint32_t flagsID, std::uint32_t nDRangeID, std::uint32_t numEventsID, std::uint32_t waitEventsID, std::uint32_t retEventID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID, const std::vector<std::uint32_t>& localSizesID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || queueID == 0 || flagsID == 0 || nDRangeID == 0 || numEventsID == 0 || waitEventsID == 0 || retEventID == 0 || invokeID == 0 || paramID == 0 || paramSizeID == 0 || paramAlignID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x000D'0124 + static_cast<std::uint32_t>((localSizesID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(queueID);
		m_Buffer.emplace_back(flagsID);
		m_Buffer.emplace_back(nDRangeID);
		m_Buffer.emplace_back(numEventsID);
		m_Buffer.emplace_back(waitEventsID);
		m_Buffer.emplace_back(retEventID);
		m_Buffer.emplace_back(invokeID);
		m_Buffer.emplace_back(paramID);
		m_Buffer.emplace_back(paramSizeID);
		m_Buffer.emplace_back(paramAlignID);
		m_Buffer.insert(m_Buffer.end(), localSizesID.begin(), localSizesID.end());
	}

	void CodeBuffer::OpGetKernelNDrangeSubGroupCount(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t nDRangeID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || nDRangeID == 0 || invokeID == 0 || paramID == 0 || paramSizeID == 0 || paramAlignID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0008'0125);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(nDRangeID);
		m_Buffer.emplace_back(invokeID);
		m_Buffer.emplace_back(paramID);
		m_Buffer.emplace_back(paramSizeID);
		m_Buffer.emplace_back(paramAlignID);
	}

	void CodeBuffer::OpGetKernelNDrangeMaxSubGroupSize(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t nDRangeID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || nDRangeID == 0 || invokeID == 0 || paramID == 0 || paramSizeID == 0 || paramAlignID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0008'0126);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(nDRangeID);
		m_Buffer.emplace_back(invokeID);
		m_Buffer.emplace_back(paramID);
		m_Buffer.emplace_back(paramSizeID);
		m_Buffer.emplace_back(paramAlignID);
	}

	void CodeBuffer::OpGetKernelWorkGroupSize(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || invokeID == 0 || paramID == 0 || paramSizeID == 0 || paramAlignID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0007'0127);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(invokeID);
		m_Buffer.emplace_back(paramID);
		m_Buffer.emplace_back(paramSizeID);
		m_Buffer.emplace_back(paramAlignID);
	}

	void CodeBuffer::OpGetKernelPreferredWorkGroupSizeMultiple(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || invokeID == 0 || paramID == 0 || paramSizeID == 0 || paramAlignID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0007'0128);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(invokeID);
		m_Buffer.emplace_back(paramID);
		m_Buffer.emplace_back(paramSizeID);
		m_Buffer.emplace_back(paramAlignID);
	}

	void CodeBuffer::OpRetainEvent(std::uint32_t eventID)
	{
		PROFILE_FUNC;

		if (eventID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0002'0129);
		m_Buffer.emplace_back(eventID);
	}

	void CodeBuffer::OpReleaseEvent(std::uint32_t eventID)
	{
		PROFILE_FUNC;

		if (eventID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0002'012A);
		m_Buffer.emplace_back(eventID);
	}

	void CodeBuffer::OpCreateUserEvent(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0003'012B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpIsValidEvent(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t eventID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || eventID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0004'012C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(eventID);
	}

	void CodeBuffer::OpSetUserEventStatus(std::uint32_t eventID, std::uint32_t statusID)
	{
		PROFILE_FUNC;

		if (eventID == 0 || statusID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0003'012D);
		m_Buffer.emplace_back(eventID);
		m_Buffer.emplace_back(statusID);
	}

	void CodeBuffer::OpCaptureEventProfilingInfo(std::uint32_t eventID, std::uint32_t profilingInfoID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (eventID == 0 || profilingInfoID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0004'012E);
		m_Buffer.emplace_back(eventID);
		m_Buffer.emplace_back(profilingInfoID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpGetDefaultQueue(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0003'012F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpBuildNDRange(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t globalWorkSizeID, std::uint32_t localWorkSizeID, std::uint32_t globalWorkOffsetID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || globalWorkSizeID == 0 || localWorkSizeID == 0 || globalWorkOffsetID == 0)
			return;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0006'0130);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(globalWorkSizeID);
		m_Buffer.emplace_back(localWorkSizeID);
		m_Buffer.emplace_back(globalWorkOffsetID);
	}

	void CodeBuffer::OpImageSparseSampleImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'0131 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseSampleExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0132);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseSampleDrefImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0133 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseSampleDrefExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0007'0134);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseSampleProjImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'0135 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseSampleProjExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0136);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseSampleProjDrefImplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0137 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseSampleProjDrefExplicitLod(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0007'0138);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseFetch(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'0139 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseGather(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t componentID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || componentID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'013A + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(componentID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseDrefGather(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t drefID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || drefID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'013B + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(drefID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpImageSparseTexelsResident(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t residentCodeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || residentCodeID == 0)
			return;

		requireCapability(ECapability::SparseResidency);

		m_Buffer.emplace_back(0x0004'013C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(residentCodeID);
	}

	void CodeBuffer::OpNoLine()
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'013D);
	}

	void CodeBuffer::OpAtomicFlagTestAndSet(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0006'013E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpAtomicFlagClear(std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (pointerID == 0 || memoryID == 0 || semanticsID == 0)
			return;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'013F);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpImageSparseRead(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::SparseResidency);
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'0140 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpSizeOf(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0)
			return;

		requireCapability(ECapability::Addresses);

		m_Buffer.emplace_back(0x0004'0141);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
	}

	void CodeBuffer::OpTypePipeStorage(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::PipeStorage);

		m_Buffer.emplace_back(0x0002'0142);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpConstantPipeStorage(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packetSize, std::uint32_t packetAlignment, std::uint32_t capacity)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::PipeStorage);

		m_Buffer.emplace_back(0x0006'0143);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(packetSize);
		m_Buffer.emplace_back(packetAlignment);
		m_Buffer.emplace_back(capacity);
	}

	void CodeBuffer::OpCreatePipeFromPipeStorage(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pipeStorageID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pipeStorageID == 0)
			return;

		requireCapability(ECapability::PipeStorage);

		m_Buffer.emplace_back(0x0004'0144);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pipeStorageID);
	}

	void CodeBuffer::OpGetKernelLocalSizeForSubgroupCount(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t subgroupCountID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || subgroupCountID == 0 || invokeID == 0 || paramID == 0 || paramSizeID == 0 || paramAlignID == 0)
			return;

		requireCapability(ECapability::SubgroupDispatch);

		m_Buffer.emplace_back(0x0008'0145);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(subgroupCountID);
		m_Buffer.emplace_back(invokeID);
		m_Buffer.emplace_back(paramID);
		m_Buffer.emplace_back(paramSizeID);
		m_Buffer.emplace_back(paramAlignID);
	}

	void CodeBuffer::OpGetKernelMaxNumSubgroups(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t invokeID, std::uint32_t paramID, std::uint32_t paramSizeID, std::uint32_t paramAlignID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || invokeID == 0 || paramID == 0 || paramSizeID == 0 || paramAlignID == 0)
			return;

		requireCapability(ECapability::SubgroupDispatch);

		m_Buffer.emplace_back(0x0007'0146);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(invokeID);
		m_Buffer.emplace_back(paramID);
		m_Buffer.emplace_back(paramSizeID);
		m_Buffer.emplace_back(paramAlignID);
	}

	void CodeBuffer::OpTypeNamedBarrier(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::NamedBarrier);

		m_Buffer.emplace_back(0x0002'0147);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpNamedBarrierInitialize(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t subgroupCountID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || subgroupCountID == 0)
			return;

		requireCapability(ECapability::NamedBarrier);

		m_Buffer.emplace_back(0x0004'0148);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(subgroupCountID);
	}

	void CodeBuffer::OpMemoryNamedBarrier(std::uint32_t namedBarrierID, std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (namedBarrierID == 0 || memoryID == 0 || semanticsID == 0)
			return;

		requireCapability(ECapability::NamedBarrier);

		m_Buffer.emplace_back(0x0004'0149);
		m_Buffer.emplace_back(namedBarrierID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpModuleProcessed(std::string_view process)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'014A + static_cast<std::uint32_t>((literalStringLength(process)) << 16));
		pushLiteralString(process);
	}

	void CodeBuffer::OpExecutionModeId(std::uint32_t entryPointID, EExecutionMode mode)
	{
		PROFILE_FUNC;

		if (entryPointID == 0)
			return;

		RequireCapExtExecutionMode(mode, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'014B);
		m_Buffer.emplace_back(entryPointID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(mode));
	}

	void CodeBuffer::OpDecorateId(std::uint32_t targetID, EDecoration decoration)
	{
		PROFILE_FUNC;

		if (targetID == 0)
			return;

		requireExtension("SPV_GOOGLE_hlsl_functionality1");
		RequireCapExtDecoration(decoration, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'014C);
		m_Buffer.emplace_back(targetID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(decoration));
	}

	void CodeBuffer::OpGroupNonUniformElect(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0)
			return;

		requireCapability(ECapability::GroupNonUniform);

		m_Buffer.emplace_back(0x0004'014D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
	}

	void CodeBuffer::OpGroupNonUniformAll(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || predicateID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformVote);

		m_Buffer.emplace_back(0x0005'014E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(predicateID);
	}

	void CodeBuffer::OpGroupNonUniformAny(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || predicateID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformVote);

		m_Buffer.emplace_back(0x0005'014F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(predicateID);
	}

	void CodeBuffer::OpGroupNonUniformAllEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformVote);

		m_Buffer.emplace_back(0x0005'0150);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpGroupNonUniformBroadcast(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || ID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformBallot);

		m_Buffer.emplace_back(0x0006'0151);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(ID);
	}

	void CodeBuffer::OpGroupNonUniformBroadcastFirst(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformBallot);

		m_Buffer.emplace_back(0x0005'0152);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpGroupNonUniformBallot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t predicateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || predicateID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformBallot);

		m_Buffer.emplace_back(0x0005'0153);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(predicateID);
	}

	void CodeBuffer::OpGroupNonUniformInverseBallot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformBallot);

		m_Buffer.emplace_back(0x0005'0154);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpGroupNonUniformBallotBitExtract(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t indexID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || indexID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformBallot);

		m_Buffer.emplace_back(0x0006'0155);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(indexID);
	}

	void CodeBuffer::OpGroupNonUniformBallotBitCount(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformBallot);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0156);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpGroupNonUniformBallotFindLSB(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformBallot);

		m_Buffer.emplace_back(0x0005'0157);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpGroupNonUniformBallotFindMSB(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformBallot);

		m_Buffer.emplace_back(0x0005'0158);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpGroupNonUniformShuffle(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || ID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformShuffle);

		m_Buffer.emplace_back(0x0006'0159);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(ID);
	}

	void CodeBuffer::OpGroupNonUniformShuffleXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t maskID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || maskID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformShuffle);

		m_Buffer.emplace_back(0x0006'015A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(maskID);
	}

	void CodeBuffer::OpGroupNonUniformShuffleUp(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t deltaID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || deltaID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformShuffleRelative);

		m_Buffer.emplace_back(0x0006'015B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(deltaID);
	}

	void CodeBuffer::OpGroupNonUniformShuffleDown(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t deltaID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || deltaID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformShuffleRelative);

		m_Buffer.emplace_back(0x0006'015C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(deltaID);
	}

	void CodeBuffer::OpGroupNonUniformIAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'015D + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformFAdd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'015E + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformIMul(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'015F + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformFMul(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0160 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformSMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0161 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformUMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0162 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformFMin(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0163 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformSMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0164 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformUMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0165 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformFMax(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0166 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformBitwiseAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0167 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformBitwiseOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0168 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformBitwiseXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'0169 + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformLogicalAnd(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'016A + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformLogicalOr(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'016B + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformLogicalXor(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t valueID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformArithmetic);
		requireCapability(ECapability::GroupNonUniformClustered);
		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'016C + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(valueID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpGroupNonUniformQuadBroadcast(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t indexID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || indexID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformQuad);

		m_Buffer.emplace_back(0x0006'016D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(indexID);
	}

	void CodeBuffer::OpGroupNonUniformQuadSwap(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t directionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || directionID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformQuad);

		m_Buffer.emplace_back(0x0006'016E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(directionID);
	}

	void CodeBuffer::OpCopyLogical(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		m_Buffer.emplace_back(0x0004'0190);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpPtrEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0191);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpPtrNotEqual(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		m_Buffer.emplace_back(0x0005'0192);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpPtrDiff(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::Addresses);
		requireCapability(ECapability::VariablePointers);
		requireCapability(ECapability::VariablePointersStorageBuffer);

		m_Buffer.emplace_back(0x0005'0193);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpTerminateInvocation()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Shader);
		requireExtension("SPV_KHR_terminate_invocation");

		m_Buffer.emplace_back(0x0001'1140);
	}

	void CodeBuffer::OpSubgroupBallotKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t predicateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || predicateID == 0)
			return;

		requireCapability(ECapability::SubgroupBallotKHR);
		requireExtension("SPV_KHR_shader_ballot");

		m_Buffer.emplace_back(0x0004'1145);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(predicateID);
	}

	void CodeBuffer::OpSubgroupFirstInvocationKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::SubgroupBallotKHR);
		requireExtension("SPV_KHR_shader_ballot");

		m_Buffer.emplace_back(0x0004'1146);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpSubgroupAllKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t predicateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || predicateID == 0)
			return;

		requireCapability(ECapability::SubgroupVoteKHR);
		requireExtension("SPV_KHR_subgroup_vote");

		m_Buffer.emplace_back(0x0004'114C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(predicateID);
	}

	void CodeBuffer::OpSubgroupAnyKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t predicateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || predicateID == 0)
			return;

		requireCapability(ECapability::SubgroupVoteKHR);
		requireExtension("SPV_KHR_subgroup_vote");

		m_Buffer.emplace_back(0x0004'114D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(predicateID);
	}

	void CodeBuffer::OpSubgroupAllEqualKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t predicateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || predicateID == 0)
			return;

		requireCapability(ECapability::SubgroupVoteKHR);
		requireExtension("SPV_KHR_subgroup_vote");

		m_Buffer.emplace_back(0x0004'114E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(predicateID);
	}

	void CodeBuffer::OpGroupNonUniformRotateKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, std::uint32_t valueID, std::uint32_t deltaID, std::uint32_t clusterSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || valueID == 0 || deltaID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformRotateKHR);

		m_Buffer.emplace_back(0x0006'114F + static_cast<std::uint32_t>((!!clusterSizeID) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(deltaID);
		if (clusterSizeID)
			m_Buffer.emplace_back(clusterSizeID);
	}

	void CodeBuffer::OpSubgroupReadInvocationKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID, std::uint32_t indexID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || valueID == 0 || indexID == 0)
			return;

		requireCapability(ECapability::SubgroupBallotKHR);
		requireExtension("SPV_KHR_shader_ballot");

		m_Buffer.emplace_back(0x0005'1150);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(indexID);
	}

	void CodeBuffer::OpTraceRayKHR(std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t sBTOffsetID, std::uint32_t sBTStrideID, std::uint32_t missIndexID, std::uint32_t rayOriginID, std::uint32_t rayTminID, std::uint32_t rayDirectionID, std::uint32_t rayTmaxID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (accelID == 0 || rayFlagsID == 0 || cullMaskID == 0 || sBTOffsetID == 0 || sBTStrideID == 0 || missIndexID == 0 || rayOriginID == 0 || rayTminID == 0 || rayDirectionID == 0 || rayTmaxID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::RayTracingKHR);
		requireExtension("SPV_KHR_ray_tracing");

		m_Buffer.emplace_back(0x000C'115D);
		m_Buffer.emplace_back(accelID);
		m_Buffer.emplace_back(rayFlagsID);
		m_Buffer.emplace_back(cullMaskID);
		m_Buffer.emplace_back(sBTOffsetID);
		m_Buffer.emplace_back(sBTStrideID);
		m_Buffer.emplace_back(missIndexID);
		m_Buffer.emplace_back(rayOriginID);
		m_Buffer.emplace_back(rayTminID);
		m_Buffer.emplace_back(rayDirectionID);
		m_Buffer.emplace_back(rayTmaxID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpExecuteCallableKHR(std::uint32_t sBTIndexID, std::uint32_t callableDataID)
	{
		PROFILE_FUNC;

		if (sBTIndexID == 0 || callableDataID == 0)
			return;

		requireCapability(ECapability::RayTracingKHR);
		requireExtension("SPV_KHR_ray_tracing");

		m_Buffer.emplace_back(0x0003'115E);
		m_Buffer.emplace_back(sBTIndexID);
		m_Buffer.emplace_back(callableDataID);
	}

	void CodeBuffer::OpConvertUToAccelerationStructureKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t accelID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || accelID == 0)
			return;

		requireCapability(ECapability::RayTracingKHR);
		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_tracing");
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0004'115F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(accelID);
	}

	void CodeBuffer::OpIgnoreIntersectionKHR()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::RayTracingKHR);
		requireExtension("SPV_KHR_ray_tracing");

		m_Buffer.emplace_back(0x0001'1160);
	}

	void CodeBuffer::OpTerminateRayKHR()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::RayTracingKHR);
		requireExtension("SPV_KHR_ray_tracing");

		m_Buffer.emplace_back(0x0001'1161);
	}

	void CodeBuffer::OpSDot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::optional<EPackedVectorFormat> packedVectorFormat)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vector1ID == 0 || vector2ID == 0)
			return;

		requireCapability(ECapability::DotProduct);

		m_Buffer.emplace_back(0x0005'1162 + static_cast<std::uint32_t>(((packedVectorFormat ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vector1ID);
		m_Buffer.emplace_back(vector2ID);
		if (packedVectorFormat)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(*packedVectorFormat));
	}

	void CodeBuffer::OpUDot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::optional<EPackedVectorFormat> packedVectorFormat)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vector1ID == 0 || vector2ID == 0)
			return;

		requireCapability(ECapability::DotProduct);

		m_Buffer.emplace_back(0x0005'1163 + static_cast<std::uint32_t>(((packedVectorFormat ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vector1ID);
		m_Buffer.emplace_back(vector2ID);
		if (packedVectorFormat)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(*packedVectorFormat));
	}

	void CodeBuffer::OpSUDot(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::optional<EPackedVectorFormat> packedVectorFormat)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vector1ID == 0 || vector2ID == 0)
			return;

		requireCapability(ECapability::DotProduct);

		m_Buffer.emplace_back(0x0005'1164 + static_cast<std::uint32_t>(((packedVectorFormat ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vector1ID);
		m_Buffer.emplace_back(vector2ID);
		if (packedVectorFormat)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(*packedVectorFormat));
	}

	void CodeBuffer::OpSDotAccSat(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::uint32_t accumulatorID, std::optional<EPackedVectorFormat> packedVectorFormat)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vector1ID == 0 || vector2ID == 0 || accumulatorID == 0)
			return;

		requireCapability(ECapability::DotProduct);

		m_Buffer.emplace_back(0x0006'1165 + static_cast<std::uint32_t>(((packedVectorFormat ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vector1ID);
		m_Buffer.emplace_back(vector2ID);
		m_Buffer.emplace_back(accumulatorID);
		if (packedVectorFormat)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(*packedVectorFormat));
	}

	void CodeBuffer::OpUDotAccSat(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::uint32_t accumulatorID, std::optional<EPackedVectorFormat> packedVectorFormat)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vector1ID == 0 || vector2ID == 0 || accumulatorID == 0)
			return;

		requireCapability(ECapability::DotProduct);

		m_Buffer.emplace_back(0x0006'1166 + static_cast<std::uint32_t>(((packedVectorFormat ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vector1ID);
		m_Buffer.emplace_back(vector2ID);
		m_Buffer.emplace_back(accumulatorID);
		if (packedVectorFormat)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(*packedVectorFormat));
	}

	void CodeBuffer::OpSUDotAccSat(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t vector1ID, std::uint32_t vector2ID, std::uint32_t accumulatorID, std::optional<EPackedVectorFormat> packedVectorFormat)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || vector1ID == 0 || vector2ID == 0 || accumulatorID == 0)
			return;

		requireCapability(ECapability::DotProduct);

		m_Buffer.emplace_back(0x0006'1167 + static_cast<std::uint32_t>(((packedVectorFormat ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(vector1ID);
		m_Buffer.emplace_back(vector2ID);
		m_Buffer.emplace_back(accumulatorID);
		if (packedVectorFormat)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(*packedVectorFormat));
	}

	void CodeBuffer::OpTypeRayQueryKHR(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0002'1178);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpRayQueryInitializeKHR(std::uint32_t rayQueryID, std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t rayOriginID, std::uint32_t rayTMinID, std::uint32_t rayDirectionID, std::uint32_t rayTMaxID)
	{
		PROFILE_FUNC;

		if (rayQueryID == 0 || accelID == 0 || rayFlagsID == 0 || cullMaskID == 0 || rayOriginID == 0 || rayTMinID == 0 || rayDirectionID == 0 || rayTMaxID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0009'1179);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(accelID);
		m_Buffer.emplace_back(rayFlagsID);
		m_Buffer.emplace_back(cullMaskID);
		m_Buffer.emplace_back(rayOriginID);
		m_Buffer.emplace_back(rayTMinID);
		m_Buffer.emplace_back(rayDirectionID);
		m_Buffer.emplace_back(rayTMaxID);
	}

	void CodeBuffer::OpRayQueryTerminateKHR(std::uint32_t rayQueryID)
	{
		PROFILE_FUNC;

		if (rayQueryID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0002'117A);
		m_Buffer.emplace_back(rayQueryID);
	}

	void CodeBuffer::OpRayQueryGenerateIntersectionKHR(std::uint32_t rayQueryID, std::uint32_t hitTID)
	{
		PROFILE_FUNC;

		if (rayQueryID == 0 || hitTID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0003'117B);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(hitTID);
	}

	void CodeBuffer::OpRayQueryConfirmIntersectionKHR(std::uint32_t rayQueryID)
	{
		PROFILE_FUNC;

		if (rayQueryID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0002'117C);
		m_Buffer.emplace_back(rayQueryID);
	}

	void CodeBuffer::OpRayQueryProceedKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0004'117D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionTypeKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'117F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpGroupIAddNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		requireExtension("SPV_AMD_shader_ballot");
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1388);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupFAddNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		requireExtension("SPV_AMD_shader_ballot");
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1389);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupFMinNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		requireExtension("SPV_AMD_shader_ballot");
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'138A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupUMinNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		requireExtension("SPV_AMD_shader_ballot");
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'138B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupSMinNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		requireExtension("SPV_AMD_shader_ballot");
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'138C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupFMaxNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		requireExtension("SPV_AMD_shader_ballot");
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'138D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupUMaxNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		requireExtension("SPV_AMD_shader_ballot");
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'138E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupSMaxNonUniformAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::Groups);
		requireExtension("SPV_AMD_shader_ballot");
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'138F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpFragmentMaskFetchAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::FragmentMaskAMD);
		requireExtension("SPV_AMD_shader_fragment_mask");

		m_Buffer.emplace_back(0x0005'1393);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
	}

	void CodeBuffer::OpFragmentFetchAMD(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t fragmentIndexID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || coordinateID == 0 || fragmentIndexID == 0)
			return;

		requireCapability(ECapability::FragmentMaskAMD);
		requireExtension("SPV_AMD_shader_fragment_mask");

		m_Buffer.emplace_back(0x0006'1394);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(fragmentIndexID);
	}

	void CodeBuffer::OpReadClockKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t scopeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || scopeID == 0)
			return;

		requireCapability(ECapability::ShaderClockKHR);
		requireExtension("SPV_KHR_shader_clock");

		m_Buffer.emplace_back(0x0004'13C0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(scopeID);
	}

	void CodeBuffer::OpImageSampleFootprintNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sampledImageID, std::uint32_t coordinateID, std::uint32_t granularityID, std::uint32_t coarseID, EImageOperands imageOperands)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sampledImageID == 0 || coordinateID == 0 || granularityID == 0 || coarseID == 0)
			return;

		requireCapability(ECapability::ImageFootprintNV);
		requireExtension("SPV_NV_shader_image_footprint");
		if (imageOperands != EImageOperands::None)
			RequireCapExtImageOperands(imageOperands, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0007'14A3 + static_cast<std::uint32_t>(((imageOperands != EImageOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(granularityID);
		m_Buffer.emplace_back(coarseID);
		if (imageOperands != EImageOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
	}

	void CodeBuffer::OpEmitMeshTasksEXT(std::uint32_t groupCountXID, std::uint32_t groupCountYID, std::uint32_t groupCountZID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (groupCountXID == 0 || groupCountYID == 0 || groupCountZID == 0)
			return;

		requireCapability(ECapability::MeshShadingEXT);

		m_Buffer.emplace_back(0x0004'14AE + static_cast<std::uint32_t>((!!payloadID) << 16));
		m_Buffer.emplace_back(groupCountXID);
		m_Buffer.emplace_back(groupCountYID);
		m_Buffer.emplace_back(groupCountZID);
		if (payloadID)
			m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSetMeshOutputsEXT(std::uint32_t vertexCountID, std::uint32_t primitiveCountID)
	{
		PROFILE_FUNC;

		if (vertexCountID == 0 || primitiveCountID == 0)
			return;

		requireCapability(ECapability::MeshShadingEXT);

		m_Buffer.emplace_back(0x0003'14AF);
		m_Buffer.emplace_back(vertexCountID);
		m_Buffer.emplace_back(primitiveCountID);
	}

	void CodeBuffer::OpGroupNonUniformPartitionNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::GroupNonUniformPartitionedNV);
		requireExtension("SPV_NV_shader_subgroup_partitioned");

		m_Buffer.emplace_back(0x0004'14B0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpWritePackedPrimitiveIndices4x8NV(std::uint32_t indexOffsetID, std::uint32_t packedIndicesID)
	{
		PROFILE_FUNC;

		if (indexOffsetID == 0 || packedIndicesID == 0)
			return;

		requireCapability(ECapability::MeshShadingNV);
		requireExtension("SPV_NV_mesh_shader");

		m_Buffer.emplace_back(0x0003'14B3);
		m_Buffer.emplace_back(indexOffsetID);
		m_Buffer.emplace_back(packedIndicesID);
	}

	void CodeBuffer::OpIgnoreIntersectionNV()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::RayTracingNV);
		requireExtension("SPV_NV_ray_tracing");

		m_Buffer.emplace_back(0x0001'14D7);
	}

	void CodeBuffer::OpTerminateRayNV()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::RayTracingNV);
		requireExtension("SPV_NV_ray_tracing");

		m_Buffer.emplace_back(0x0001'14D8);
	}

	void CodeBuffer::OpTraceNV(std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t sBTOffsetID, std::uint32_t sBTStrideID, std::uint32_t missIndexID, std::uint32_t rayOriginID, std::uint32_t rayTminID, std::uint32_t rayDirectionID, std::uint32_t rayTmaxID, std::uint32_t payloadIdID)
	{
		PROFILE_FUNC;

		if (accelID == 0 || rayFlagsID == 0 || cullMaskID == 0 || sBTOffsetID == 0 || sBTStrideID == 0 || missIndexID == 0 || rayOriginID == 0 || rayTminID == 0 || rayDirectionID == 0 || rayTmaxID == 0 || payloadIdID == 0)
			return;

		requireCapability(ECapability::RayTracingNV);
		requireExtension("SPV_NV_ray_tracing");

		m_Buffer.emplace_back(0x000C'14D9);
		m_Buffer.emplace_back(accelID);
		m_Buffer.emplace_back(rayFlagsID);
		m_Buffer.emplace_back(cullMaskID);
		m_Buffer.emplace_back(sBTOffsetID);
		m_Buffer.emplace_back(sBTStrideID);
		m_Buffer.emplace_back(missIndexID);
		m_Buffer.emplace_back(rayOriginID);
		m_Buffer.emplace_back(rayTminID);
		m_Buffer.emplace_back(rayDirectionID);
		m_Buffer.emplace_back(rayTmaxID);
		m_Buffer.emplace_back(payloadIdID);
	}

	void CodeBuffer::OpTraceMotionNV(std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t sBTOffsetID, std::uint32_t sBTStrideID, std::uint32_t missIndexID, std::uint32_t rayOriginID, std::uint32_t rayTminID, std::uint32_t rayDirectionID, std::uint32_t rayTmaxID, std::uint32_t timeID, std::uint32_t payloadIdID)
	{
		PROFILE_FUNC;

		if (accelID == 0 || rayFlagsID == 0 || cullMaskID == 0 || sBTOffsetID == 0 || sBTStrideID == 0 || missIndexID == 0 || rayOriginID == 0 || rayTminID == 0 || rayDirectionID == 0 || rayTmaxID == 0 || timeID == 0 || payloadIdID == 0)
			return;

		requireCapability(ECapability::RayTracingMotionBlurNV);
		requireExtension("SPV_NV_ray_tracing_motion_blur");

		m_Buffer.emplace_back(0x000D'14DA);
		m_Buffer.emplace_back(accelID);
		m_Buffer.emplace_back(rayFlagsID);
		m_Buffer.emplace_back(cullMaskID);
		m_Buffer.emplace_back(sBTOffsetID);
		m_Buffer.emplace_back(sBTStrideID);
		m_Buffer.emplace_back(missIndexID);
		m_Buffer.emplace_back(rayOriginID);
		m_Buffer.emplace_back(rayTminID);
		m_Buffer.emplace_back(rayDirectionID);
		m_Buffer.emplace_back(rayTmaxID);
		m_Buffer.emplace_back(timeID);
		m_Buffer.emplace_back(payloadIdID);
	}

	void CodeBuffer::OpTraceRayMotionNV(std::uint32_t accelID, std::uint32_t rayFlagsID, std::uint32_t cullMaskID, std::uint32_t sBTOffsetID, std::uint32_t sBTStrideID, std::uint32_t missIndexID, std::uint32_t rayOriginID, std::uint32_t rayTminID, std::uint32_t rayDirectionID, std::uint32_t rayTmaxID, std::uint32_t timeID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (accelID == 0 || rayFlagsID == 0 || cullMaskID == 0 || sBTOffsetID == 0 || sBTStrideID == 0 || missIndexID == 0 || rayOriginID == 0 || rayTminID == 0 || rayDirectionID == 0 || rayTmaxID == 0 || timeID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::RayTracingMotionBlurNV);
		requireExtension("SPV_NV_ray_tracing_motion_blur");

		m_Buffer.emplace_back(0x000D'14DB);
		m_Buffer.emplace_back(accelID);
		m_Buffer.emplace_back(rayFlagsID);
		m_Buffer.emplace_back(cullMaskID);
		m_Buffer.emplace_back(sBTOffsetID);
		m_Buffer.emplace_back(sBTStrideID);
		m_Buffer.emplace_back(missIndexID);
		m_Buffer.emplace_back(rayOriginID);
		m_Buffer.emplace_back(rayTminID);
		m_Buffer.emplace_back(rayDirectionID);
		m_Buffer.emplace_back(rayTmaxID);
		m_Buffer.emplace_back(timeID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpExecuteCallableNV(std::uint32_t sBTIndexID, std::uint32_t callableDataIdID)
	{
		PROFILE_FUNC;

		if (sBTIndexID == 0 || callableDataIdID == 0)
			return;

		requireCapability(ECapability::RayTracingNV);
		requireExtension("SPV_NV_ray_tracing");

		m_Buffer.emplace_back(0x0003'14E0);
		m_Buffer.emplace_back(sBTIndexID);
		m_Buffer.emplace_back(callableDataIdID);
	}

	void CodeBuffer::OpTypeCooperativeMatrixNV(std::uint32_t resultID, std::uint32_t componentTypeID, std::uint32_t executionID, std::uint32_t rowsID, std::uint32_t columnsID)
	{
		PROFILE_FUNC;

		if (resultID == 0 || componentTypeID == 0 || executionID == 0 || rowsID == 0 || columnsID == 0)
			return;

		requireCapability(ECapability::CooperativeMatrixNV);
		requireExtension("SPV_NV_cooperative_matrix");

		m_Buffer.emplace_back(0x0006'14EE);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(componentTypeID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(rowsID);
		m_Buffer.emplace_back(columnsID);
	}

	void CodeBuffer::OpCooperativeMatrixLoadNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t strideID, std::uint32_t columnMajorID, EMemoryAccess memoryAccess)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || strideID == 0 || columnMajorID == 0)
			return;

		requireCapability(ECapability::CooperativeMatrixNV);
		requireExtension("SPV_NV_cooperative_matrix");
		if (memoryAccess != EMemoryAccess::None)
			RequireCapExtMemoryAccess(memoryAccess, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'14EF + static_cast<std::uint32_t>(((memoryAccess != EMemoryAccess::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(strideID);
		m_Buffer.emplace_back(columnMajorID);
		if (memoryAccess != EMemoryAccess::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryAccess));
	}

	void CodeBuffer::OpCooperativeMatrixStoreNV(std::uint32_t pointerID, std::uint32_t objectID, std::uint32_t strideID, std::uint32_t columnMajorID, EMemoryAccess memoryAccess)
	{
		PROFILE_FUNC;

		if (pointerID == 0 || objectID == 0 || strideID == 0 || columnMajorID == 0)
			return;

		requireCapability(ECapability::CooperativeMatrixNV);
		requireExtension("SPV_NV_cooperative_matrix");
		if (memoryAccess != EMemoryAccess::None)
			RequireCapExtMemoryAccess(memoryAccess, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0005'14F0 + static_cast<std::uint32_t>(((memoryAccess != EMemoryAccess::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(objectID);
		m_Buffer.emplace_back(strideID);
		m_Buffer.emplace_back(columnMajorID);
		if (memoryAccess != EMemoryAccess::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryAccess));
	}

	void CodeBuffer::OpCooperativeMatrixMulAddNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t bID, std::uint32_t cID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0 || cID == 0)
			return;

		requireCapability(ECapability::CooperativeMatrixNV);
		requireExtension("SPV_NV_cooperative_matrix");

		m_Buffer.emplace_back(0x0006'14F1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(cID);
	}

	void CodeBuffer::OpCooperativeMatrixLengthNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t typeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || typeID == 0)
			return;

		requireCapability(ECapability::CooperativeMatrixNV);
		requireExtension("SPV_NV_cooperative_matrix");

		m_Buffer.emplace_back(0x0004'14F2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(typeID);
	}

	void CodeBuffer::OpBeginInvocationInterlockEXT()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::FragmentShaderSampleInterlockEXT);
		requireCapability(ECapability::FragmentShaderPixelInterlockEXT);
		requireCapability(ECapability::FragmentShaderShadingRateInterlockEXT);
		requireExtension("SPV_EXT_fragment_shader_interlock");

		m_Buffer.emplace_back(0x0001'14F4);
	}

	void CodeBuffer::OpEndInvocationInterlockEXT()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::FragmentShaderSampleInterlockEXT);
		requireCapability(ECapability::FragmentShaderPixelInterlockEXT);
		requireCapability(ECapability::FragmentShaderShadingRateInterlockEXT);
		requireExtension("SPV_EXT_fragment_shader_interlock");

		m_Buffer.emplace_back(0x0001'14F5);
	}

	void CodeBuffer::OpDemoteToHelperInvocation()
	{
		PROFILE_FUNC;

		requireCapability(ECapability::DemoteToHelperInvocation);

		m_Buffer.emplace_back(0x0001'1504);
	}

	void CodeBuffer::OpIsHelperInvocationEXT(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::DemoteToHelperInvocation);
		requireExtension("SPV_EXT_demote_to_helper_invocation");

		m_Buffer.emplace_back(0x0003'1505);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpConvertUToImageNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		requireCapability(ECapability::BindlessTextureNV);

		m_Buffer.emplace_back(0x0004'150F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpConvertUToSamplerNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		requireCapability(ECapability::BindlessTextureNV);

		m_Buffer.emplace_back(0x0004'1510);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpConvertImageToUNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		requireCapability(ECapability::BindlessTextureNV);

		m_Buffer.emplace_back(0x0004'1511);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpConvertSamplerToUNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		requireCapability(ECapability::BindlessTextureNV);

		m_Buffer.emplace_back(0x0004'1512);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpConvertUToSampledImageNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		requireCapability(ECapability::BindlessTextureNV);

		m_Buffer.emplace_back(0x0004'1513);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpConvertSampledImageToUNV(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		requireCapability(ECapability::BindlessTextureNV);

		m_Buffer.emplace_back(0x0004'1514);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpSamplerImageAddressingModeNV(std::uint32_t bitWidth)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::BindlessTextureNV);

		m_Buffer.emplace_back(0x0002'1515);
		m_Buffer.emplace_back(bitWidth);
	}

	void CodeBuffer::OpSubgroupShuffleINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t dataID, std::uint32_t invocationIdID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || dataID == 0 || invocationIdID == 0)
			return;

		requireCapability(ECapability::SubgroupShuffleINTEL);

		m_Buffer.emplace_back(0x0005'15C3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(dataID);
		m_Buffer.emplace_back(invocationIdID);
	}

	void CodeBuffer::OpSubgroupShuffleDownINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t currentID, std::uint32_t nextID, std::uint32_t deltaID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || currentID == 0 || nextID == 0 || deltaID == 0)
			return;

		requireCapability(ECapability::SubgroupShuffleINTEL);

		m_Buffer.emplace_back(0x0006'15C4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(currentID);
		m_Buffer.emplace_back(nextID);
		m_Buffer.emplace_back(deltaID);
	}

	void CodeBuffer::OpSubgroupShuffleUpINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t previousID, std::uint32_t currentID, std::uint32_t deltaID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || previousID == 0 || currentID == 0 || deltaID == 0)
			return;

		requireCapability(ECapability::SubgroupShuffleINTEL);

		m_Buffer.emplace_back(0x0006'15C5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(previousID);
		m_Buffer.emplace_back(currentID);
		m_Buffer.emplace_back(deltaID);
	}

	void CodeBuffer::OpSubgroupShuffleXorINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t dataID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || dataID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::SubgroupShuffleINTEL);

		m_Buffer.emplace_back(0x0005'15C6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(dataID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpSubgroupBlockReadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t ptrID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || ptrID == 0)
			return;

		requireCapability(ECapability::SubgroupBufferBlockIOINTEL);

		m_Buffer.emplace_back(0x0004'15C7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(ptrID);
	}

	void CodeBuffer::OpSubgroupBlockWriteINTEL(std::uint32_t ptrID, std::uint32_t dataID)
	{
		PROFILE_FUNC;

		if (ptrID == 0 || dataID == 0)
			return;

		requireCapability(ECapability::SubgroupBufferBlockIOINTEL);

		m_Buffer.emplace_back(0x0003'15C8);
		m_Buffer.emplace_back(ptrID);
		m_Buffer.emplace_back(dataID);
	}

	void CodeBuffer::OpSubgroupImageBlockReadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || coordinateID == 0)
			return;

		requireCapability(ECapability::SubgroupImageBlockIOINTEL);

		m_Buffer.emplace_back(0x0005'15C9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
	}

	void CodeBuffer::OpSubgroupImageBlockWriteINTEL(std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t dataID)
	{
		PROFILE_FUNC;

		if (imageID == 0 || coordinateID == 0 || dataID == 0)
			return;

		requireCapability(ECapability::SubgroupImageBlockIOINTEL);

		m_Buffer.emplace_back(0x0004'15CA);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(dataID);
	}

	void CodeBuffer::OpSubgroupImageMediaBlockReadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t widthID, std::uint32_t heightID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageID == 0 || coordinateID == 0 || widthID == 0 || heightID == 0)
			return;

		requireCapability(ECapability::SubgroupImageMediaBlockIOINTEL);

		m_Buffer.emplace_back(0x0007'15CC);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(widthID);
		m_Buffer.emplace_back(heightID);
	}

	void CodeBuffer::OpSubgroupImageMediaBlockWriteINTEL(std::uint32_t imageID, std::uint32_t coordinateID, std::uint32_t widthID, std::uint32_t heightID, std::uint32_t dataID)
	{
		PROFILE_FUNC;

		if (imageID == 0 || coordinateID == 0 || widthID == 0 || heightID == 0 || dataID == 0)
			return;

		requireCapability(ECapability::SubgroupImageMediaBlockIOINTEL);

		m_Buffer.emplace_back(0x0006'15CD);
		m_Buffer.emplace_back(imageID);
		m_Buffer.emplace_back(coordinateID);
		m_Buffer.emplace_back(widthID);
		m_Buffer.emplace_back(heightID);
		m_Buffer.emplace_back(dataID);
	}

	void CodeBuffer::OpUCountLeadingZerosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0004'15D1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpUCountTrailingZerosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operandID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operandID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0004'15D2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operandID);
	}

	void CodeBuffer::OpAbsISubINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15D3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpAbsUSubINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15D4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpIAddSatINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15D5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUAddSatINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15D6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpIAverageINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15D7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUAverageINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15D8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpIAverageRoundedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15D9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUAverageRoundedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15DA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpISubSatINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15DB);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUSubSatINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15DC);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpIMul32x16INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15DD);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpUMul32x16INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t operand1ID, std::uint32_t operand2ID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || operand1ID == 0 || operand2ID == 0)
			return;

		requireCapability(ECapability::IntegerFunctions2INTEL);

		m_Buffer.emplace_back(0x0005'15DE);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1ID);
		m_Buffer.emplace_back(operand2ID);
	}

	void CodeBuffer::OpConstantFunctionPointerINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t functionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || functionID == 0)
			return;

		requireCapability(ECapability::FunctionPointersINTEL);
		requireExtension("SPV_INTEL_function_pointers");

		m_Buffer.emplace_back(0x0004'15E0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(functionID);
	}

	void CodeBuffer::OpFunctionPointerCallINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, const std::vector<std::uint32_t>& operandsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::FunctionPointersINTEL);
		requireExtension("SPV_INTEL_function_pointers");

		m_Buffer.emplace_back(0x0003'15E1 + static_cast<std::uint32_t>((operandsID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), operandsID.begin(), operandsID.end());
	}

	void CodeBuffer::OpAsmTargetINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::string_view asmtarget)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::AsmINTEL);

		m_Buffer.emplace_back(0x0003'15E9 + static_cast<std::uint32_t>((literalStringLength(asmtarget)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		pushLiteralString(asmtarget);
	}

	void CodeBuffer::OpAsmINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t asmtypeID, std::uint32_t targetID, std::string_view asminstructions, std::string_view constraints)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || asmtypeID == 0 || targetID == 0)
			return;

		requireCapability(ECapability::AsmINTEL);

		m_Buffer.emplace_back(0x0005'15EA + static_cast<std::uint32_t>((literalStringLength(asminstructions) + literalStringLength(constraints)) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(asmtypeID);
		m_Buffer.emplace_back(targetID);
		pushLiteralString(asminstructions);
		pushLiteralString(constraints);
	}

	void CodeBuffer::OpAsmCallINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t asmID, const std::vector<std::uint32_t>& argumentsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || asmID == 0)
			return;

		requireCapability(ECapability::AsmINTEL);

		m_Buffer.emplace_back(0x0004'15EB + static_cast<std::uint32_t>((argumentsID.size()) << 16));
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(asmID);
		m_Buffer.insert(m_Buffer.end(), argumentsID.begin(), argumentsID.end());
	}

	void CodeBuffer::OpAtomicFMinEXT(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::AtomicFloat16MinMaxEXT);
		requireCapability(ECapability::AtomicFloat32MinMaxEXT);
		requireCapability(ECapability::AtomicFloat64MinMaxEXT);

		m_Buffer.emplace_back(0x0007'15EE);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAtomicFMaxEXT(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::AtomicFloat16MinMaxEXT);
		requireCapability(ECapability::AtomicFloat32MinMaxEXT);
		requireCapability(ECapability::AtomicFloat64MinMaxEXT);

		m_Buffer.emplace_back(0x0007'15EF);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpAssumeTrueKHR(std::uint32_t conditionID)
	{
		PROFILE_FUNC;

		if (conditionID == 0)
			return;

		requireCapability(ECapability::ExpectAssumeKHR);
		requireExtension("SPV_KHR_expect_assume");

		m_Buffer.emplace_back(0x0002'15FE);
		m_Buffer.emplace_back(conditionID);
	}

	void CodeBuffer::OpExpectKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t valueID, std::uint32_t expectedValueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || valueID == 0 || expectedValueID == 0)
			return;

		requireCapability(ECapability::ExpectAssumeKHR);
		requireExtension("SPV_KHR_expect_assume");

		m_Buffer.emplace_back(0x0005'15FF);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(valueID);
		m_Buffer.emplace_back(expectedValueID);
	}

	void CodeBuffer::OpVmeImageINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageTypeID, std::uint32_t samplerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageTypeID == 0 || samplerID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1643);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageTypeID);
		m_Buffer.emplace_back(samplerID);
	}

	void CodeBuffer::OpTypeVmeImageINTEL(std::uint32_t resultID, std::uint32_t imageTypeID)
	{
		PROFILE_FUNC;

		if (resultID == 0 || imageTypeID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0003'1644);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageTypeID);
	}

	void CodeBuffer::OpTypeAvcImePayloadINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'1645);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcRefPayloadINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'1646);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcSicPayloadINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'1647);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcMcePayloadINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'1648);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcMceResultINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'1649);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcImeResultINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'164A);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcImeResultSingleReferenceStreamoutINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'164B);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcImeResultDualReferenceStreamoutINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'164C);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcImeSingleReferenceStreaminINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'164D);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcImeDualReferenceStreaminINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'164E);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcRefResultINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'164F);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeAvcSicResultINTEL(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0002'1650);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultInterBaseMultiReferencePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sliceTypeID == 0 || qpID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1651);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sliceTypeID);
		m_Buffer.emplace_back(qpID);
	}

	void CodeBuffer::OpSubgroupAvcMceSetInterBaseMultiReferencePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t referenceBasePenaltyID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || referenceBasePenaltyID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1652);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(referenceBasePenaltyID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultInterShapePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sliceTypeID == 0 || qpID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1653);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sliceTypeID);
		m_Buffer.emplace_back(qpID);
	}

	void CodeBuffer::OpSubgroupAvcMceSetInterShapePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedShapePenaltyID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || packedShapePenaltyID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1654);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(packedShapePenaltyID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultInterDirectionPenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sliceTypeID == 0 || qpID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1655);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sliceTypeID);
		m_Buffer.emplace_back(qpID);
	}

	void CodeBuffer::OpSubgroupAvcMceSetInterDirectionPenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t directionCostID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || directionCostID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1656);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(directionCostID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultIntraLumaShapePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sliceTypeID == 0 || qpID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0005'1657);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sliceTypeID);
		m_Buffer.emplace_back(qpID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultInterMotionVectorCostTableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sliceTypeID == 0 || qpID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1658);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sliceTypeID);
		m_Buffer.emplace_back(qpID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultHighPenaltyCostTableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0003'1659);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultMediumPenaltyCostTableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0003'165A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultLowPenaltyCostTableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0003'165B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSubgroupAvcMceSetMotionVectorCostFunctionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedCostCenterDeltaID, std::uint32_t packedCostTableID, std::uint32_t costPrecisionID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || packedCostCenterDeltaID == 0 || packedCostTableID == 0 || costPrecisionID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'165C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(packedCostCenterDeltaID);
		m_Buffer.emplace_back(packedCostTableID);
		m_Buffer.emplace_back(costPrecisionID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultIntraLumaModePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sliceTypeID, std::uint32_t qpID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sliceTypeID == 0 || qpID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0005'165D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sliceTypeID);
		m_Buffer.emplace_back(qpID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultNonDcLumaIntraPenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0003'165E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetDefaultIntraChromaModeBasePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationChromaINTEL);

		m_Buffer.emplace_back(0x0003'165F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSubgroupAvcMceSetAcOnlyHaarINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1660);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceSetSourceInterlacedFieldPolarityINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t sourceFieldPolarityID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || sourceFieldPolarityID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1661);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sourceFieldPolarityID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceSetSingleReferenceInterlacedFieldPolarityINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t referenceFieldPolarityID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || referenceFieldPolarityID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1662);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(referenceFieldPolarityID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceSetDualReferenceInterlacedFieldPolaritiesINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t forwardReferenceFieldPolarityID, std::uint32_t backwardReferenceFieldPolarityID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || forwardReferenceFieldPolarityID == 0 || backwardReferenceFieldPolarityID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'1663);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(forwardReferenceFieldPolarityID);
		m_Buffer.emplace_back(backwardReferenceFieldPolarityID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceConvertToImePayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1664);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceConvertToImeResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1665);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceConvertToRefPayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1666);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceConvertToRefResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1667);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceConvertToSicPayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1668);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceConvertToSicResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1669);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetMotionVectorsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'166A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetInterDistortionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'166B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetBestInterDistortionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'166C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetInterMajorShapeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'166D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetInterMinorShapeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'166E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetInterDirectionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'166F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetInterMotionVectorCountINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1670);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetInterReferenceIdsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1671);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcMceGetInterReferenceInterlacedFieldPolaritiesINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedReferenceIdsID, std::uint32_t packedReferenceParameterFieldPolaritiesID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || packedReferenceIdsID == 0 || packedReferenceParameterFieldPolaritiesID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'1672);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(packedReferenceIdsID);
		m_Buffer.emplace_back(packedReferenceParameterFieldPolaritiesID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeInitializeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcCoordID, std::uint32_t partitionMaskID, std::uint32_t sADAdjustmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcCoordID == 0 || partitionMaskID == 0 || sADAdjustmentID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'1673);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcCoordID);
		m_Buffer.emplace_back(partitionMaskID);
		m_Buffer.emplace_back(sADAdjustmentID);
	}

	void CodeBuffer::OpSubgroupAvcImeSetSingleReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t refOffsetID, std::uint32_t searchWindowConfigID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || refOffsetID == 0 || searchWindowConfigID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'1674);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(refOffsetID);
		m_Buffer.emplace_back(searchWindowConfigID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeSetDualReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t fwdRefOffsetID, std::uint32_t bwdRefOffsetID, std::uint32_t SearchWindowConfigID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || fwdRefOffsetID == 0 || bwdRefOffsetID == 0 || SearchWindowConfigID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'1675);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(fwdRefOffsetID);
		m_Buffer.emplace_back(bwdRefOffsetID);
		m_Buffer.emplace_back(SearchWindowConfigID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeRefWindowSizeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t searchWindowConfigID, std::uint32_t dualRefID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || searchWindowConfigID == 0 || dualRefID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1676);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(searchWindowConfigID);
		m_Buffer.emplace_back(dualRefID);
	}

	void CodeBuffer::OpSubgroupAvcImeAdjustRefOffsetINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t refOffsetID, std::uint32_t srcCoordID, std::uint32_t refWindowSizeID, std::uint32_t imageSizeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || refOffsetID == 0 || srcCoordID == 0 || refWindowSizeID == 0 || imageSizeID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'1677);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(refOffsetID);
		m_Buffer.emplace_back(srcCoordID);
		m_Buffer.emplace_back(refWindowSizeID);
		m_Buffer.emplace_back(imageSizeID);
	}

	void CodeBuffer::OpSubgroupAvcImeConvertToMcePayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1678);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeSetMaxMotionVectorCountINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t maxMotionVectorCountID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || maxMotionVectorCountID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1679);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(maxMotionVectorCountID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeSetUnidirectionalMixDisableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'167A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeSetEarlySearchTerminationThresholdINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t thresholdID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || thresholdID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'167B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(thresholdID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeSetWeightedSadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedSadWeightsID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || packedSadWeightsID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'167C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(packedSadWeightsID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeEvaluateWithSingleReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || refImageID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'167D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(refImageID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeEvaluateWithDualReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || fwdRefImageID == 0 || bwdRefImageID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'167E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(fwdRefImageID);
		m_Buffer.emplace_back(bwdRefImageID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeEvaluateWithSingleReferenceStreaminINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID, std::uint32_t streaminComponentsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || refImageID == 0 || payloadID == 0 || streaminComponentsID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'167F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(refImageID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(streaminComponentsID);
	}

	void CodeBuffer::OpSubgroupAvcImeEvaluateWithDualReferenceStreaminINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID, std::uint32_t streaminComponentsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || fwdRefImageID == 0 || bwdRefImageID == 0 || payloadID == 0 || streaminComponentsID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0008'1680);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(fwdRefImageID);
		m_Buffer.emplace_back(bwdRefImageID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(streaminComponentsID);
	}

	void CodeBuffer::OpSubgroupAvcImeEvaluateWithSingleReferenceStreamoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || refImageID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'1681);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(refImageID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeEvaluateWithDualReferenceStreamoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || fwdRefImageID == 0 || bwdRefImageID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'1682);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(fwdRefImageID);
		m_Buffer.emplace_back(bwdRefImageID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeEvaluateWithSingleReferenceStreaminoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID, std::uint32_t streaminComponentsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || refImageID == 0 || payloadID == 0 || streaminComponentsID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'1683);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(refImageID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(streaminComponentsID);
	}

	void CodeBuffer::OpSubgroupAvcImeEvaluateWithDualReferenceStreaminoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID, std::uint32_t streaminComponentsID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || fwdRefImageID == 0 || bwdRefImageID == 0 || payloadID == 0 || streaminComponentsID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0008'1684);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(fwdRefImageID);
		m_Buffer.emplace_back(bwdRefImageID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(streaminComponentsID);
	}

	void CodeBuffer::OpSubgroupAvcImeConvertToMceResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1685);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetSingleReferenceStreaminINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1686);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetDualReferenceStreaminINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1687);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeStripSingleReferenceStreamoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1688);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeStripDualReferenceStreamoutINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1689);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeMotionVectorsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0 || majorShapeID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'168A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(majorShapeID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeDistortionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0 || majorShapeID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'168B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(majorShapeID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeReferenceIdsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0 || majorShapeID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'168C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(majorShapeID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeMotionVectorsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID, std::uint32_t directionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0 || majorShapeID == 0 || directionID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'168D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(majorShapeID);
		m_Buffer.emplace_back(directionID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeDistortionsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID, std::uint32_t directionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0 || majorShapeID == 0 || directionID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'168E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(majorShapeID);
		m_Buffer.emplace_back(directionID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeReferenceIdsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID, std::uint32_t majorShapeID, std::uint32_t directionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0 || majorShapeID == 0 || directionID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'168F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
		m_Buffer.emplace_back(majorShapeID);
		m_Buffer.emplace_back(directionID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetBorderReachedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t imageSelectID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || imageSelectID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'1690);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageSelectID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetTruncatedSearchIndicationINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1691);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetUnidirectionalEarlySearchTerminationINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1692);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetWeightingPatternMinimumMotionVectorINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1693);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcImeGetWeightingPatternMinimumDistortionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1694);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcFmeInitializeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcCoordID, std::uint32_t motionVectorsID, std::uint32_t majorShapesID, std::uint32_t minorShapesID, std::uint32_t directionID, std::uint32_t pixelResolutionID, std::uint32_t sadAdjustmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcCoordID == 0 || motionVectorsID == 0 || majorShapesID == 0 || minorShapesID == 0 || directionID == 0 || pixelResolutionID == 0 || sadAdjustmentID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x000A'1695);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcCoordID);
		m_Buffer.emplace_back(motionVectorsID);
		m_Buffer.emplace_back(majorShapesID);
		m_Buffer.emplace_back(minorShapesID);
		m_Buffer.emplace_back(directionID);
		m_Buffer.emplace_back(pixelResolutionID);
		m_Buffer.emplace_back(sadAdjustmentID);
	}

	void CodeBuffer::OpSubgroupAvcBmeInitializeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcCoordID, std::uint32_t motionVectorsID, std::uint32_t majorShapesID, std::uint32_t minorShapesID, std::uint32_t directionID, std::uint32_t pixelResolutionID, std::uint32_t bidirectionalWeightID, std::uint32_t sadAdjustmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcCoordID == 0 || motionVectorsID == 0 || majorShapesID == 0 || minorShapesID == 0 || directionID == 0 || pixelResolutionID == 0 || bidirectionalWeightID == 0 || sadAdjustmentID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x000B'1696);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcCoordID);
		m_Buffer.emplace_back(motionVectorsID);
		m_Buffer.emplace_back(majorShapesID);
		m_Buffer.emplace_back(minorShapesID);
		m_Buffer.emplace_back(directionID);
		m_Buffer.emplace_back(pixelResolutionID);
		m_Buffer.emplace_back(bidirectionalWeightID);
		m_Buffer.emplace_back(sadAdjustmentID);
	}

	void CodeBuffer::OpSubgroupAvcRefConvertToMcePayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1697);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcRefSetBidirectionalMixDisableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1698);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcRefSetBilinearFilterEnableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'1699);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcRefEvaluateWithSingleReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || refImageID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'169A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(refImageID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcRefEvaluateWithDualReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || fwdRefImageID == 0 || bwdRefImageID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'169B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(fwdRefImageID);
		m_Buffer.emplace_back(bwdRefImageID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcRefEvaluateWithMultiReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t packedReferenceIdsID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || packedReferenceIdsID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'169C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(packedReferenceIdsID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcRefEvaluateWithMultiReferenceInterlacedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t packedReferenceIdsID, std::uint32_t packedReferenceFieldPolaritiesID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || packedReferenceIdsID == 0 || packedReferenceFieldPolaritiesID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'169D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(packedReferenceIdsID);
		m_Buffer.emplace_back(packedReferenceFieldPolaritiesID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcRefConvertToMceResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'169E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicInitializeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcCoordID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcCoordID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'169F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcCoordID);
	}

	void CodeBuffer::OpSubgroupAvcSicConfigureSkcINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t skipBlockPartitionTypeID, std::uint32_t skipMotionVectorMaskID, std::uint32_t motionVectorsID, std::uint32_t bidirectionalWeightID, std::uint32_t sadAdjustmentID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || skipBlockPartitionTypeID == 0 || skipMotionVectorMaskID == 0 || motionVectorsID == 0 || bidirectionalWeightID == 0 || sadAdjustmentID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0009'16A0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(skipBlockPartitionTypeID);
		m_Buffer.emplace_back(skipMotionVectorMaskID);
		m_Buffer.emplace_back(motionVectorsID);
		m_Buffer.emplace_back(bidirectionalWeightID);
		m_Buffer.emplace_back(sadAdjustmentID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicConfigureIpeLumaINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t lumaIntraPartitionMaskID, std::uint32_t intraNeighbourAvailabiltyID, std::uint32_t leftEdgeLumaPixelsID, std::uint32_t upperLeftCornerLumaPixelID, std::uint32_t upperEdgeLumaPixelsID, std::uint32_t upperRightEdgeLumaPixelsID, std::uint32_t sadAdjustmentID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || lumaIntraPartitionMaskID == 0 || intraNeighbourAvailabiltyID == 0 || leftEdgeLumaPixelsID == 0 || upperLeftCornerLumaPixelID == 0 || upperEdgeLumaPixelsID == 0 || upperRightEdgeLumaPixelsID == 0 || sadAdjustmentID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x000B'16A1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(lumaIntraPartitionMaskID);
		m_Buffer.emplace_back(intraNeighbourAvailabiltyID);
		m_Buffer.emplace_back(leftEdgeLumaPixelsID);
		m_Buffer.emplace_back(upperLeftCornerLumaPixelID);
		m_Buffer.emplace_back(upperEdgeLumaPixelsID);
		m_Buffer.emplace_back(upperRightEdgeLumaPixelsID);
		m_Buffer.emplace_back(sadAdjustmentID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicConfigureIpeLumaChromaINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t lumaIntraPartitionMaskID, std::uint32_t intraNeighbourAvailabiltyID, std::uint32_t leftEdgeLumaPixelsID, std::uint32_t upperLeftCornerLumaPixelID, std::uint32_t upperEdgeLumaPixelsID, std::uint32_t upperRightEdgeLumaPixelsID, std::uint32_t leftEdgeChromaPixelsID, std::uint32_t upperLeftCornerChromaPixelID, std::uint32_t upperEdgeChromaPixelsID, std::uint32_t sadAdjustmentID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || lumaIntraPartitionMaskID == 0 || intraNeighbourAvailabiltyID == 0 || leftEdgeLumaPixelsID == 0 || upperLeftCornerLumaPixelID == 0 || upperEdgeLumaPixelsID == 0 || upperRightEdgeLumaPixelsID == 0 || leftEdgeChromaPixelsID == 0 || upperLeftCornerChromaPixelID == 0 || upperEdgeChromaPixelsID == 0 || sadAdjustmentID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationChromaINTEL);

		m_Buffer.emplace_back(0x000E'16A2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(lumaIntraPartitionMaskID);
		m_Buffer.emplace_back(intraNeighbourAvailabiltyID);
		m_Buffer.emplace_back(leftEdgeLumaPixelsID);
		m_Buffer.emplace_back(upperLeftCornerLumaPixelID);
		m_Buffer.emplace_back(upperEdgeLumaPixelsID);
		m_Buffer.emplace_back(upperRightEdgeLumaPixelsID);
		m_Buffer.emplace_back(leftEdgeChromaPixelsID);
		m_Buffer.emplace_back(upperLeftCornerChromaPixelID);
		m_Buffer.emplace_back(upperEdgeChromaPixelsID);
		m_Buffer.emplace_back(sadAdjustmentID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicGetMotionVectorMaskINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t skipBlockPartitionTypeID, std::uint32_t directionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || skipBlockPartitionTypeID == 0 || directionID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'16A3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(skipBlockPartitionTypeID);
		m_Buffer.emplace_back(directionID);
	}

	void CodeBuffer::OpSubgroupAvcSicConvertToMcePayloadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'16A4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicSetIntraLumaShapePenaltyINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedShapePenaltyID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || packedShapePenaltyID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'16A5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(packedShapePenaltyID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicSetIntraLumaModeCostFunctionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t lumaModePenaltyID, std::uint32_t lumaPackedNeighborModesID, std::uint32_t lumaPackedNonDcPenaltyID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || lumaModePenaltyID == 0 || lumaPackedNeighborModesID == 0 || lumaPackedNonDcPenaltyID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0007'16A6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(lumaModePenaltyID);
		m_Buffer.emplace_back(lumaPackedNeighborModesID);
		m_Buffer.emplace_back(lumaPackedNonDcPenaltyID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicSetIntraChromaModeCostFunctionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t chromaModeBasePenaltyID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || chromaModeBasePenaltyID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationChromaINTEL);

		m_Buffer.emplace_back(0x0005'16A7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(chromaModeBasePenaltyID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicSetBilinearFilterEnableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'16A8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicSetSkcForwardTransformEnableINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packedSadCoefficientsID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || packedSadCoefficientsID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'16A9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(packedSadCoefficientsID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicSetBlockBasedRawSkipSadINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t blockBasedSkipTypeID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || blockBasedSkipTypeID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0005'16AA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(blockBasedSkipTypeID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicEvaluateIpeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0005'16AB);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicEvaluateWithSingleReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t refImageID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || refImageID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'16AC);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(refImageID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicEvaluateWithDualReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t fwdRefImageID, std::uint32_t bwdRefImageID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || fwdRefImageID == 0 || bwdRefImageID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'16AD);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(fwdRefImageID);
		m_Buffer.emplace_back(bwdRefImageID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicEvaluateWithMultiReferenceINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t packedReferenceIdsID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || packedReferenceIdsID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0006'16AE);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(packedReferenceIdsID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicEvaluateWithMultiReferenceInterlacedINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t srcImageID, std::uint32_t packedReferenceIdsID, std::uint32_t packedReferenceFieldPolaritiesID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || srcImageID == 0 || packedReferenceIdsID == 0 || packedReferenceFieldPolaritiesID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0007'16AF);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(srcImageID);
		m_Buffer.emplace_back(packedReferenceIdsID);
		m_Buffer.emplace_back(packedReferenceFieldPolaritiesID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicConvertToMceResultINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'16B0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicGetIpeLumaShapeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0004'16B1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicGetBestIpeLumaDistortionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0004'16B2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicGetBestIpeChromaDistortionINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'16B3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicGetPackedIpeLumaModesINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0004'16B4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicGetIpeChromaModeINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationChromaINTEL);

		m_Buffer.emplace_back(0x0004'16B5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicGetPackedSkcLumaCountThresholdINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0004'16B6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicGetPackedSkcLumaSumThresholdINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);
		requireCapability(ECapability::SubgroupAvcMotionEstimationIntraINTEL);

		m_Buffer.emplace_back(0x0004'16B7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpSubgroupAvcSicGetInterRawSadsINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t payloadID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || payloadID == 0)
			return;

		requireCapability(ECapability::SubgroupAvcMotionEstimationINTEL);

		m_Buffer.emplace_back(0x0004'16B8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(payloadID);
	}

	void CodeBuffer::OpVariableLengthArrayINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t lenghtID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || lenghtID == 0)
			return;

		requireCapability(ECapability::VariableLengthArrayINTEL);

		m_Buffer.emplace_back(0x0004'16BA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(lenghtID);
	}

	void CodeBuffer::OpSaveMemoryINTEL(std::uint32_t resultTypeID, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0)
			return;

		requireCapability(ECapability::VariableLengthArrayINTEL);

		m_Buffer.emplace_back(0x0003'16BB);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpRestoreMemoryINTEL(std::uint32_t ptrID)
	{
		PROFILE_FUNC;

		if (ptrID == 0)
			return;

		requireCapability(ECapability::VariableLengthArrayINTEL);

		m_Buffer.emplace_back(0x0002'16BC);
		m_Buffer.emplace_back(ptrID);
	}

	void CodeBuffer::OpArbitraryFloatSinCosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t fromSign, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000A'16D0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(fromSign);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatCastINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16D1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatCastFromIntINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t mout, std::uint32_t fromSign, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16D2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(fromSign);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatCastToIntINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0008'16D3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatAddINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000B'16D6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatSubINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000B'16D7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatMulINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000B'16D8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatDivINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000B'16D9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatGTINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0007'16DA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
	}

	void CodeBuffer::OpArbitraryFloatGEINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0007'16DB);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
	}

	void CodeBuffer::OpArbitraryFloatLTINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0007'16DC);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
	}

	void CodeBuffer::OpArbitraryFloatLEINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0007'16DD);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
	}

	void CodeBuffer::OpArbitraryFloatEQINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0007'16DE);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
	}

	void CodeBuffer::OpArbitraryFloatRecipINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16DF);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatRSqrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16E0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatCbrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16E1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatHypotINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000B'16E2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatSqrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16E3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatLogINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16E4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatLog2INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16E5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatLog10INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16E6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatLog1pINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16E7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatExpINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16E8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatExp2INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16E9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatExp10INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16EA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatExpm1INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16EB);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatSinINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16EC);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatCosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16ED);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatSinCosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16EE);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatSinPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16EF);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatCosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16F0);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatASinINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16F1);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatASinPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16F2);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatACosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16F3);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatACosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16F4);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatATanINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16F5);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatATanPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x0009'16F6);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatATan2INTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000B'16F7);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatPowINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000B'16F8);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatPowRINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t m2, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000B'16F9);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(m2);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpArbitraryFloatPowNINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t aID, std::uint32_t m1, std::uint32_t bID, std::uint32_t mout, std::uint32_t enableSubnormals, std::uint32_t roundingMode, std::uint32_t roundingAccuracy)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || aID == 0 || bID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFloatingPointINTEL);

		m_Buffer.emplace_back(0x000A'16FA);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aID);
		m_Buffer.emplace_back(m1);
		m_Buffer.emplace_back(bID);
		m_Buffer.emplace_back(mout);
		m_Buffer.emplace_back(enableSubnormals);
		m_Buffer.emplace_back(roundingMode);
		m_Buffer.emplace_back(roundingAccuracy);
	}

	void CodeBuffer::OpLoopControlINTEL(const std::vector<std::uint32_t>& loopControlParameters)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::UnstructuredLoopControlsINTEL);
		requireExtension("SPV_INTEL_unstructured_loop_controls");

		m_Buffer.emplace_back(0x0001'16FF + static_cast<std::uint32_t>((loopControlParameters.size()) << 16));
		m_Buffer.insert(m_Buffer.end(), loopControlParameters.begin(), loopControlParameters.end());
	}

	void CodeBuffer::OpAliasDomainDeclINTEL(std::uint32_t resultID, std::uint32_t nameID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::MemoryAccessAliasingINTEL);
		requireExtension("SPV_INTEL_memory_access_aliasing");

		m_Buffer.emplace_back(0x0002'1717 + static_cast<std::uint32_t>((!!nameID) << 16));
		m_Buffer.emplace_back(resultID);
		if (nameID)
			m_Buffer.emplace_back(nameID);
	}

	void CodeBuffer::OpAliasScopeDeclINTEL(std::uint32_t resultID, std::uint32_t aliasDomainID, std::uint32_t nameID)
	{
		PROFILE_FUNC;

		if (resultID == 0 || aliasDomainID == 0)
			return;

		requireCapability(ECapability::MemoryAccessAliasingINTEL);
		requireExtension("SPV_INTEL_memory_access_aliasing");

		m_Buffer.emplace_back(0x0003'1718 + static_cast<std::uint32_t>((!!nameID) << 16));
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(aliasDomainID);
		if (nameID)
			m_Buffer.emplace_back(nameID);
	}

	void CodeBuffer::OpAliasScopeListDeclINTEL(std::uint32_t resultID, const std::vector<std::uint32_t>& aliasScopesID)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::MemoryAccessAliasingINTEL);
		requireExtension("SPV_INTEL_memory_access_aliasing");

		m_Buffer.emplace_back(0x0002'1719 + static_cast<std::uint32_t>((aliasScopesID.size()) << 16));
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), aliasScopesID.begin(), aliasScopesID.end());
	}

	void CodeBuffer::OpFixedSqrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'1723);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedRecipINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'1724);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedRsqrtINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'1725);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedSinINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'1726);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedCosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'1727);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedSinCosINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'1728);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedSinPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'1729);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedCosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'172A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedSinCosPiINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'172B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedLogINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'172C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpFixedExpINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t inputTypeID, std::uint32_t inputID, std::uint32_t s, std::uint32_t i, std::uint32_t rI, std::uint32_t q, std::uint32_t o)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || inputTypeID == 0 || inputID == 0)
			return;

		requireCapability(ECapability::ArbitraryPrecisionFixedPointINTEL);

		m_Buffer.emplace_back(0x000A'172D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(inputTypeID);
		m_Buffer.emplace_back(inputID);
		m_Buffer.emplace_back(s);
		m_Buffer.emplace_back(i);
		m_Buffer.emplace_back(rI);
		m_Buffer.emplace_back(q);
		m_Buffer.emplace_back(o);
	}

	void CodeBuffer::OpPtrCastToCrossWorkgroupINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0)
			return;

		requireCapability(ECapability::USMStorageClassesINTEL);

		m_Buffer.emplace_back(0x0004'172E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
	}

	void CodeBuffer::OpCrossWorkgroupCastToPtrINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0)
			return;

		requireCapability(ECapability::USMStorageClassesINTEL);

		m_Buffer.emplace_back(0x0004'1732);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
	}

	void CodeBuffer::OpReadPipeBlockingINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::BlockingPipesINTEL);
		requireExtension("SPV_INTEL_blocking_pipes");

		m_Buffer.emplace_back(0x0005'173A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpWritePipeBlockingINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t packetSizeID, std::uint32_t packetAlignmentID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || packetSizeID == 0 || packetAlignmentID == 0)
			return;

		requireCapability(ECapability::BlockingPipesINTEL);
		requireExtension("SPV_INTEL_blocking_pipes");

		m_Buffer.emplace_back(0x0005'173B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(packetSizeID);
		m_Buffer.emplace_back(packetAlignmentID);
	}

	void CodeBuffer::OpFPGARegINTEL(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t resultID0, std::uint32_t inputID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || resultID0 == 0 || inputID == 0)
			return;

		requireCapability(ECapability::FPGARegINTEL);
		requireExtension("SPV_INTEL_fpga_reg");

		m_Buffer.emplace_back(0x0005'173D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(resultID0);
		m_Buffer.emplace_back(inputID);
	}

	void CodeBuffer::OpRayQueryGetRayTMinKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0004'1780);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
	}

	void CodeBuffer::OpRayQueryGetRayFlagsKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0004'1781);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionTKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'1782);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionInstanceCustomIndexKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'1783);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionInstanceIdKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'1784);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionInstanceShaderBindingTableRecordOffsetKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'1785);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionGeometryIndexKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'1786);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionPrimitiveIndexKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'1787);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionBarycentricsKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'1788);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionFrontFaceKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'1789);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionCandidateAABBOpaqueKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0004'178A);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionObjectRayDirectionKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'178B);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionObjectRayOriginKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'178C);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetWorldRayDirectionKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0004'178D);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
	}

	void CodeBuffer::OpRayQueryGetWorldRayOriginKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0004'178E);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionObjectToWorldKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'178F);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpRayQueryGetIntersectionWorldToObjectKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t rayQueryID, std::uint32_t intersectionID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || rayQueryID == 0 || intersectionID == 0)
			return;

		requireCapability(ECapability::RayQueryKHR);
		requireExtension("SPV_KHR_ray_query");

		m_Buffer.emplace_back(0x0005'1790);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(rayQueryID);
		m_Buffer.emplace_back(intersectionID);
	}

	void CodeBuffer::OpAtomicFAddEXT(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t pointerID, std::uint32_t memoryID, std::uint32_t semanticsID, std::uint32_t valueID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || pointerID == 0 || memoryID == 0 || semanticsID == 0 || valueID == 0)
			return;

		requireCapability(ECapability::AtomicFloat16AddEXT);
		requireCapability(ECapability::AtomicFloat32AddEXT);
		requireCapability(ECapability::AtomicFloat64AddEXT);
		requireExtension("SPV_EXT_shader_atomic_float_add");

		m_Buffer.emplace_back(0x0007'1793);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointerID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
		m_Buffer.emplace_back(valueID);
	}

	void CodeBuffer::OpTypeBufferSurfaceINTEL(std::uint32_t resultID, EAccessQualifier accessQualifier)
	{
		PROFILE_FUNC;

		if (resultID == 0)
			return;

		requireCapability(ECapability::VectorComputeINTEL);
		RequireCapExtAccessQualifier(accessQualifier, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0003'17C6);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(accessQualifier));
	}

	void CodeBuffer::OpTypeStructContinuedINTEL(const std::vector<std::uint32_t>& membersID)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::LongConstantCompositeINTEL);

		m_Buffer.emplace_back(0x0001'17CA + static_cast<std::uint32_t>((membersID.size()) << 16));
		m_Buffer.insert(m_Buffer.end(), membersID.begin(), membersID.end());
	}

	void CodeBuffer::OpConstantCompositeContinuedINTEL(const std::vector<std::uint32_t>& constituentsID)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::LongConstantCompositeINTEL);

		m_Buffer.emplace_back(0x0001'17CB + static_cast<std::uint32_t>((constituentsID.size()) << 16));
		m_Buffer.insert(m_Buffer.end(), constituentsID.begin(), constituentsID.end());
	}

	void CodeBuffer::OpSpecConstantCompositeContinuedINTEL(const std::vector<std::uint32_t>& constituentsID)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::LongConstantCompositeINTEL);

		m_Buffer.emplace_back(0x0001'17CC + static_cast<std::uint32_t>((constituentsID.size()) << 16));
		m_Buffer.insert(m_Buffer.end(), constituentsID.begin(), constituentsID.end());
	}

	void CodeBuffer::OpControlBarrierArriveINTEL(std::uint32_t executionID, std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (executionID == 0 || memoryID == 0 || semanticsID == 0)
			return;

		requireCapability(ECapability::SplitBarrierINTEL);

		m_Buffer.emplace_back(0x0004'17FE);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpControlBarrierWaitINTEL(std::uint32_t executionID, std::uint32_t memoryID, std::uint32_t semanticsID)
	{
		PROFILE_FUNC;

		if (executionID == 0 || memoryID == 0 || semanticsID == 0)
			return;

		requireCapability(ECapability::SplitBarrierINTEL);

		m_Buffer.emplace_back(0x0004'17FF);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(memoryID);
		m_Buffer.emplace_back(semanticsID);
	}

	void CodeBuffer::OpGroupIMulKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::GroupUniformArithmeticKHR);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1901);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupFMulKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::GroupUniformArithmeticKHR);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1902);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupBitwiseAndKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::GroupUniformArithmeticKHR);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1903);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupBitwiseOrKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::GroupUniformArithmeticKHR);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1904);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupBitwiseXorKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::GroupUniformArithmeticKHR);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1905);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupLogicalAndKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::GroupUniformArithmeticKHR);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1906);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupLogicalOrKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::GroupUniformArithmeticKHR);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1907);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::OpGroupLogicalXorKHR(std::uint32_t resultTypeID, std::uint32_t resultID, std::uint32_t executionID, EGroupOperation operation, std::uint32_t xID)
	{
		PROFILE_FUNC;

		if (resultTypeID == 0 || resultID == 0 || executionID == 0 || xID == 0)
			return;

		requireCapability(ECapability::GroupUniformArithmeticKHR);
		RequireCapExtGroupOperation(operation, m_Capabilities, m_Extensions);

		m_Buffer.emplace_back(0x0006'1908);
		m_Buffer.emplace_back(resultTypeID);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(executionID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(operation));
		m_Buffer.emplace_back(xID);
	}

	void CodeBuffer::pushLiteralString(std::string_view name)
	{
		PROFILE_FUNC;

		m_Buffer.reserve(m_Buffer.size() + literalStringLength(name));
		std::size_t size = (name.size() >> 2) << 2;
		for (std::size_t i = 0; i < size; i += 4)
		{
			std::uint32_t v = name[i] | (name[i + 1] << 8) | (name[i + 2] << 16) | (name[i + 3] << 24);
			m_Buffer.emplace_back(v);
		}
		std::uint32_t last = 0;
		for (std::size_t i = 0, j = size; j < name.size(); ++i, ++j)
			last += name[j] << (i * 8);
		m_Buffer.emplace_back(last);
	}

	std::uint64_t CodeBuffer::literalStringLength(std::string_view name)
	{
		PROFILE_FUNC;
		return 1 + (name.size() >> 2);
	}
} // namespace Frertex::Transpilers::SPIRV
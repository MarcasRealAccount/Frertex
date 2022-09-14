#include "Frertex/Transpilers/SPIRV/CodeBuffer.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Transpilers::SPIRV
{
	void CodeBuffer::OpNop()
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'0000);
	}

	void CodeBuffer::OpUndef(std::uint32_t resultType, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0001);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSizeOf(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Addresses);

		m_Buffer.emplace_back(0x0004'0141);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointer);
	}

	void CodeBuffer::OpAssumeTrueKHR(std::uint32_t condition)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::ExpectAssumeKHR);

		m_Buffer.emplace_back(0x0002'15FE);
		m_Buffer.emplace_back(condition);
	}

	void CodeBuffer::OpExpectKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value, std::uint32_t expectedValue)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::ExpectAssumeKHR);

		m_Buffer.emplace_back(0x0005'15FF);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(value);
		m_Buffer.emplace_back(expectedValue);
	}

	void CodeBuffer::OpSourceContinued(std::string_view continuedSource)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'0002 + static_cast<std::uint32_t>(literalStringLength(continuedSource) << 16));
		pushLiteralString(continuedSource);
	}

	void CodeBuffer::OpSource(ESourceLanguage sourceLanguage, std::uint32_t version, std::uint32_t file, std::string_view source)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0003 + static_cast<std::uint32_t>(((file != 0 ? 1 : 0) + (source.empty() ? 0 : literalStringLength(source)) << 16)));
		m_Buffer.emplace_back(static_cast<std::uint32_t>(sourceLanguage));
		m_Buffer.emplace_back(version);
		if (file != 0)
			m_Buffer.emplace_back(file);
		if (!source.empty())
			pushLiteralString(source);
	}

	void CodeBuffer::OpSourceExtension(std::string_view extension)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'0004 + static_cast<std::uint32_t>(literalStringLength(extension) << 16));
		pushLiteralString(extension);
	}

	void CodeBuffer::OpName(std::uint32_t target, std::string_view name)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'0005 + static_cast<std::uint32_t>(literalStringLength(name) << 16));
		m_Buffer.emplace_back(target);
		pushLiteralString(name);
	}

	void CodeBuffer::OpMemberName(std::uint32_t type, std::uint32_t member, std::string_view name)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0006 + static_cast<std::uint32_t>(literalStringLength(name) << 16));
		m_Buffer.emplace_back(type);
		m_Buffer.emplace_back(member);
		pushLiteralString(name);
	}

	void CodeBuffer::OpString(std::uint32_t resultID, std::string_view string)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'0007 + static_cast<std::uint32_t>(literalStringLength(string) << 16));
		m_Buffer.emplace_back(resultID);
		pushLiteralString(string);
	}

	void CodeBuffer::OpLine(std::uint32_t file, std::uint32_t line, std::uint32_t column)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0008);
		m_Buffer.emplace_back(file);
		m_Buffer.emplace_back(line);
		m_Buffer.emplace_back(column);
	}

	void CodeBuffer::OpNoLine()
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'013D);
	}

	void CodeBuffer::OpModuleProcessed(std::string_view process)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'014A + static_cast<std::uint32_t>(literalStringLength(process) << 16));
		pushLiteralString(process);
	}

	void CodeBuffer::OpDecorate(std::uint32_t target, EDecoration decoration, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0047 + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(target);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(decoration));
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpMemberDecorate(std::uint32_t structureType, std::uint32_t member, EDecoration decoration, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0048 + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(structureType);
		m_Buffer.emplace_back(member);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(decoration));
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpDecorateId(std::uint32_t target, EDecoration decoration, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'014C + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(target);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(decoration));
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpDecorateString(std::uint32_t target, EDecoration decoration, const std::vector<std::string_view>& operands)
	{
		PROFILE_FUNC;

		std::size_t operandSizes = 0;
		for (auto operand : operands)
			operandSizes += literalStringLength(operand);

		m_Buffer.emplace_back(0x0003'1600 + static_cast<std::uint32_t>(operandSizes << 16));
		m_Buffer.emplace_back(target);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(decoration));
		for (auto operand : operands)
			pushLiteralString(operand);
	}

	void CodeBuffer::OpMemberDecorateString(std::uint32_t structureType, std::uint32_t member, EDecoration decoration, const std::vector<std::string_view>& operands)
	{
		PROFILE_FUNC;

		std::size_t operandSizes = 0;
		for (auto operand : operands)
			operandSizes += literalStringLength(operand);

		m_Buffer.emplace_back(0x0004'1601 + static_cast<std::uint32_t>(operandSizes << 16));
		m_Buffer.emplace_back(structureType);
		m_Buffer.emplace_back(member);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(decoration));
		for (auto operand : operands)
			pushLiteralString(operand);
	}

	void CodeBuffer::OpExtension(std::string_view name)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'000A + static_cast<std::uint32_t>(literalStringLength(name) << 16));
		pushLiteralString(name);
	}

	void CodeBuffer::OpExtInstImport(std::uint32_t resultID, std::string_view name)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'000B + static_cast<std::uint32_t>(literalStringLength(name) << 16));
		m_Buffer.emplace_back(resultID);
		pushLiteralString(name);
	}

	void CodeBuffer::OpExtInst(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t set, std::uint32_t instruction, const std::vector<std::uint32_t>& operandIDs)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'000C + static_cast<std::uint32_t>(operandIDs.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(set);
		m_Buffer.emplace_back(instruction);
		m_Buffer.insert(m_Buffer.end(), operandIDs.begin(), operandIDs.end());
	}

	void CodeBuffer::OpMemoryModel(EAddressingMode addressingModel, EMemoryModel memoryModel)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'000E);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(addressingModel));
		m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryModel));
	}

	void CodeBuffer::OpEntryPoint(EExecutionModel executionModel, std::uint32_t entryPoint, std::string_view name, const std::vector<std::uint32_t>& interface)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'000F + static_cast<std::uint32_t>((literalStringLength(name) + interface.size()) << 16));
		m_Buffer.emplace_back(static_cast<std::uint32_t>(executionModel));
		m_Buffer.emplace_back(entryPoint);
		pushLiteralString(name);
		m_Buffer.insert(m_Buffer.end(), interface.begin(), interface.end());
	}

	void CodeBuffer::OpExecutionMode(std::uint32_t entryPoint, EExecutionMode executionMode, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0010 + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(entryPoint);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(executionMode));
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpCapability(ECapability capability)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'0011);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(capability));
	}

	void CodeBuffer::OpExecutionModeId(std::uint32_t entryPoint, EExecutionMode executionMode, const std::vector<std::uint32_t>& operandIDs)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'014B + static_cast<std::uint32_t>(operandIDs.size() << 16));
		m_Buffer.emplace_back(entryPoint);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(executionMode));
		m_Buffer.insert(m_Buffer.end(), operandIDs.begin(), operandIDs.end());
	}

	void CodeBuffer::OpTypeVoid(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'0013);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeBool(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'0014);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeInt(std::uint32_t resultID, std::uint32_t width, std::uint32_t signedness)
	{
		PROFILE_FUNC;

		switch (width)
		{
		case 8:
			requireCapability(ECapability::Int8);
			break;
		case 16:
			requireCapability(ECapability::Int16);
			break;
		case 64:
			requireCapability(ECapability::Int64);
			break;
		}

		m_Buffer.emplace_back(0x0004'0015);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(width);
		m_Buffer.emplace_back(signedness);
	}

	void CodeBuffer::OpTypeFloat(std::uint32_t resultID, std::uint32_t width)
	{
		PROFILE_FUNC;

		switch (width)
		{
		case 16:
			requireCapability(ECapability::Float16);
			break;
		case 64:
			requireCapability(ECapability::Float64);
			break;
		}

		m_Buffer.emplace_back(0x0003'0016);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(width);
	}

	void CodeBuffer::OpTypeVector(std::uint32_t resultID, std::uint32_t componentType, std::uint32_t componentCount)
	{
		PROFILE_FUNC;

		if (componentCount == 16)
			requireCapability(ECapability::Vector16);

		m_Buffer.emplace_back(0x0004'0017);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(componentType);
		m_Buffer.emplace_back(componentCount);
	}

	void CodeBuffer::OpTypeMatrix(std::uint32_t resultID, std::uint32_t columnType, std::uint32_t columnCount)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Matrix);

		m_Buffer.emplace_back(0x0004'0018);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(columnType);
		m_Buffer.emplace_back(columnCount);
	}

	void CodeBuffer::OpTypeImage(std::uint32_t resultID, std::uint32_t sampledType, EDim dim, std::uint32_t depth, std::uint32_t arrayed, std::uint32_t ms, std::uint32_t sampled, EImageFormat imageFormat)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0009'0019);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledType);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(dim));
		m_Buffer.emplace_back(depth);
		m_Buffer.emplace_back(arrayed);
		m_Buffer.emplace_back(ms);
		m_Buffer.emplace_back(sampled);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageFormat));
	}

	void CodeBuffer::OpTypeImage(std::uint32_t resultID, std::uint32_t sampledType, EDim dim, std::uint32_t depth, std::uint32_t arrayed, std::uint32_t ms, std::uint32_t sampled, EImageFormat imageFormat, EAccessQualifier accessQualifier)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x000A'0019);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledType);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(dim));
		m_Buffer.emplace_back(depth);
		m_Buffer.emplace_back(arrayed);
		m_Buffer.emplace_back(ms);
		m_Buffer.emplace_back(sampled);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageFormat));
		m_Buffer.emplace_back(static_cast<std::uint32_t>(accessQualifier));
	}

	void CodeBuffer::OpTypeSampler(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'001A);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeSampledImage(std::uint32_t resultID, std::uint32_t imageType)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'001B);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(imageType);
	}

	void CodeBuffer::OpTypeArray(std::uint32_t resultID, std::uint32_t elementType, std::uint32_t length)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'001C);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(elementType);
		m_Buffer.emplace_back(length);
	}

	void CodeBuffer::OpTypeRuntimeArray(std::uint32_t resultID, std::uint32_t elementType)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Shader);

		m_Buffer.emplace_back(0x0003'001D);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(elementType);
	}

	void CodeBuffer::OpTypeStruct(std::uint32_t resultID, const std::vector<std::uint32_t>& memberTypes)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'001E + static_cast<std::uint32_t>(memberTypes.size() << 16));
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), memberTypes.begin(), memberTypes.end());
	}

	void CodeBuffer::OpTypeOpaque(std::uint32_t resultID, std::string_view name)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0002'001F + static_cast<std::uint32_t>(literalStringLength(name) << 16));
		m_Buffer.emplace_back(resultID);
		pushLiteralString(name);
	}

	void CodeBuffer::OpTypePointer(std::uint32_t resultID, EStorageClass storageClass, std::uint32_t type)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0020);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(storageClass));
		m_Buffer.emplace_back(type);
	}

	void CodeBuffer::OpTypeFunction(std::uint32_t resultID, std::uint32_t returnType, const std::vector<std::uint32_t>& parameterTypes)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0021 + static_cast<std::uint32_t>(parameterTypes.size() << 16));
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(returnType);
		m_Buffer.insert(m_Buffer.end(), parameterTypes.begin(), parameterTypes.end());
	}

	void CodeBuffer::OpTypeEvent(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0002'0022);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeDeviceEvent(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0002'0023);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeReserveId(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0002'0024);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeQueue(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::DeviceEnqueue);

		m_Buffer.emplace_back(0x0002'0025);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypePipe(std::uint32_t resultID, EAccessQualifier accessQualifier)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Pipes);

		m_Buffer.emplace_back(0x0003'0026);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(accessQualifier));
	}

	void CodeBuffer::OpTypeForwardPointer(std::uint32_t pointerType, EStorageClass storageClass)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Addresses);
		requireCapability(ECapability::PhysicalStorageBufferAddresses);

		m_Buffer.emplace_back(0x0003'0027);
		m_Buffer.emplace_back(pointerType);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(storageClass));
	}

	void CodeBuffer::OpTypePipeStorage(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::PipeStorage);

		m_Buffer.emplace_back(0x0002'0142);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeNamedBarrier(std::uint32_t resultID)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::NamedBarrier);

		m_Buffer.emplace_back(0x0002'0147);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpTypeBufferSurfaceINTEL(std::uint32_t resultID, EAccessQualifier accessQualifier)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::VectorComputeINTEL);

		m_Buffer.emplace_back(0x0003'17C6);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(accessQualifier));
	}

	void CodeBuffer::OpTypeStructContinuedINTEL(const std::vector<std::uint32_t>& memberTypes)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::LongConstantCompositeINTEL);

		m_Buffer.emplace_back(0x0001'17CA + static_cast<std::uint32_t>(memberTypes.size() << 16));
		m_Buffer.insert(m_Buffer.end(), memberTypes.begin(), memberTypes.end());
	}

	void CodeBuffer::OpConstantTrue(std::uint32_t resultType, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'0029);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpConstantFalse(std::uint32_t resultType, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'002A);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpConstant(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& value)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'002B + static_cast<std::uint32_t>(value.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), value.begin(), value.end());
	}

	void CodeBuffer::OpConstantComposite(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& constituents)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0002'002C + static_cast<std::uint32_t>(constituents.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), constituents.begin(), constituents.end());
	}

	void CodeBuffer::OpConstantSampler(std::uint32_t resultType, std::uint32_t resultID, ESamplerAddressingMode addressingMode, std::uint32_t param, ESamplerFilterMode filterMode)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::LiteralSampler);

		m_Buffer.emplace_back(0x0006'002D);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(addressingMode));
		m_Buffer.emplace_back(param);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(filterMode));
	}

	void CodeBuffer::OpConstantNull(std::uint32_t resultType, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'002E);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSpecConstantTrue(std::uint32_t resultType, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0030);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSpecConstantFalse(std::uint32_t resultType, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0031);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpSpecConstant(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& value)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0032 + static_cast<std::uint32_t>(value.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), value.begin(), value.end());
	}

	void CodeBuffer::OpSpecConstantComposite(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& constituents)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0033 + static_cast<std::uint32_t>(constituents.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.insert(m_Buffer.end(), constituents.begin(), constituents.end());
	}

	void CodeBuffer::OpSpecConstantOp(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opcode, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0034 + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(opcode);
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpConstantCompositeContinuedINTEL(const std::vector<std::uint32_t>& constituents)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::LongConstantCompositeINTEL);

		m_Buffer.emplace_back(0x0004'17CB + static_cast<std::uint32_t>(constituents.size() << 16));
		m_Buffer.insert(m_Buffer.end(), constituents.begin(), constituents.end());
	}

	void CodeBuffer::OpSpecConstantCompositeContinuedINTEL(const std::vector<std::uint32_t>& constituents)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::LongConstantCompositeINTEL);

		m_Buffer.emplace_back(0x0004'17CC + static_cast<std::uint32_t>(constituents.size() << 16));
		m_Buffer.insert(m_Buffer.end(), constituents.begin(), constituents.end());
	}

	void CodeBuffer::OpVariable(std::uint32_t resultType, std::uint32_t resultID, EStorageClass storageClass, std::uint32_t initializer)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'003B + static_cast<std::uint32_t>(!!initializer << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(storageClass));
		if (initializer)
			m_Buffer.emplace_back(initializer);
	}

	void CodeBuffer::OpImageTexelPointer(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t coordinate, std::uint32_t sample)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0006'003C);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(sample);
	}

	void CodeBuffer::OpLoad(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, EMemoryOperands memoryOperands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'003D);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointer);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryOperands));
	}

	void CodeBuffer::OpStore(std::uint32_t pointer, std::uint32_t object, EMemoryOperands memoryOperands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'003E);
		m_Buffer.emplace_back(pointer);
		m_Buffer.emplace_back(object);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(memoryOperands));
	}

	void CodeBuffer::OpCopyMemory(std::uint32_t target, std::uint32_t source, EMemoryOperands targetMemoryOperands, EMemoryOperands sourceMemoryOperands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'003F + static_cast<std::uint32_t>(((targetMemoryOperands != EMemoryOperands::None ? 1 : 0) + (sourceMemoryOperands != EMemoryOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(target);
		m_Buffer.emplace_back(source);
		if (targetMemoryOperands != EMemoryOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(targetMemoryOperands));
		if (sourceMemoryOperands != EMemoryOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(sourceMemoryOperands));
	}

	void CodeBuffer::OpCopyMemorySized(std::uint32_t target, std::uint32_t source, std::uint32_t size, EMemoryOperands targetMemoryOperands, EMemoryOperands sourceMemoryOperands)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Addresses);

		m_Buffer.emplace_back(0x0004'0040 + static_cast<std::uint32_t>(((targetMemoryOperands != EMemoryOperands::None ? 1 : 0) + (sourceMemoryOperands != EMemoryOperands::None ? 1 : 0)) << 16));
		m_Buffer.emplace_back(target);
		m_Buffer.emplace_back(source);
		m_Buffer.emplace_back(size);
		if (targetMemoryOperands != EMemoryOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(targetMemoryOperands));
		if (sourceMemoryOperands != EMemoryOperands::None)
			m_Buffer.emplace_back(static_cast<std::uint32_t>(sourceMemoryOperands));
	}

	void CodeBuffer::OpAccessChain(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, const std::vector<std::uint32_t>& indices)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0041 + static_cast<std::uint32_t>(indices.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(base);
		m_Buffer.insert(m_Buffer.end(), indices.begin(), indices.end());
	}

	void CodeBuffer::OpInBoundsAccessChain(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, const std::vector<std::uint32_t>& indices)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0042 + static_cast<std::uint32_t>(indices.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(base);
		m_Buffer.insert(m_Buffer.end(), indices.begin(), indices.end());
	}

	void CodeBuffer::OpPtrAccessChain(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t element, const std::vector<std::uint32_t>& indices)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Addresses);
		requireCapability(ECapability::VariablePointers);
		requireCapability(ECapability::VariablePointersStorageBuffer);
		requireCapability(ECapability::PhysicalStorageBufferAddresses);

		m_Buffer.emplace_back(0x0005'0043 + static_cast<std::uint32_t>(indices.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(base);
		m_Buffer.emplace_back(element);
		m_Buffer.insert(m_Buffer.end(), indices.begin(), indices.end());
	}

	void CodeBuffer::OpArrayLength(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t structure, std::uint32_t lastMember)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Shader);

		m_Buffer.emplace_back(0x0005'0044);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(structure);
		m_Buffer.emplace_back(lastMember);
	}

	void CodeBuffer::OpGenericPtrMemSemantics(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Kernel);

		m_Buffer.emplace_back(0x0004'0045);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(pointer);
	}

	void CodeBuffer::OpInBoundsPtrAccessChain(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t element, const std::vector<std::uint32_t>& indices)
	{
		PROFILE_FUNC;

		requireCapability(ECapability::Addresses);

		m_Buffer.emplace_back(0x0005'0046 + static_cast<std::uint32_t>(indices.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(base);
		m_Buffer.emplace_back(element);
		m_Buffer.insert(m_Buffer.end(), indices.begin(), indices.end());
	}

	void CodeBuffer::OpPtrEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'0191);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1);
		m_Buffer.emplace_back(operand2);
	}

	void CodeBuffer::OpPtrNotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'0192);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1);
		m_Buffer.emplace_back(operand2);
	}

	void CodeBuffer::OpPtrDiff(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'0193);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(operand1);
		m_Buffer.emplace_back(operand2);
	}

	void CodeBuffer::OpFunction(std::uint32_t resultType, std::uint32_t resultID, EFunctionControl functionControl, std::uint32_t functionType)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'0036);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(functionControl));
		m_Buffer.emplace_back(functionType);
	}

	void CodeBuffer::OpFunctionParameter(std::uint32_t resultType, std::uint32_t resultID)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0003'0037);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
	}

	void CodeBuffer::OpFunctionEnd()
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0001'0038);
	}

	void CodeBuffer::OpFunctionCall(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t function, const std::vector<std::uint32_t>& arguments)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0039 + static_cast<std::uint32_t>(arguments.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(function);
		m_Buffer.insert(m_Buffer.end(), arguments.begin(), arguments.end());
	}

	void CodeBuffer::OpSampledImage(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t sampler)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'0056);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(image);
		m_Buffer.emplace_back(sampler);
	}

	void CodeBuffer::OpImageSampleImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'0057 + static_cast<std::uint32_t>(imageOperands != EImageOperands::None ? (1 + operands.size()) << 16 : 0));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImage);
		m_Buffer.emplace_back(coordinate);
		if (imageOperands != EImageOperands::None)
		{
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
			m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
		}
	}

	void CodeBuffer::OpImageSampleExplicitLot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0007'0058 + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImage);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
		m_Buffer.emplace_back(operand1);
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpImageSampleDrefImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0006'0059 + static_cast<std::uint32_t>(imageOperands != EImageOperands::None ? (1 + operands.size()) << 16 : 0));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImage);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(Dref);
		if (imageOperands != EImageOperands::None)
		{
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
			m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
		}
	}

	void CodeBuffer::OpImageSampleDrefExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0008'005A + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImage);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(Dref);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
		m_Buffer.emplace_back(operand1);
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpImageSampleProjImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'005B + static_cast<std::uint32_t>(imageOperands != EImageOperands::None ? (1 + operands.size()) << 16 : 0));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImage);
		m_Buffer.emplace_back(coordinate);
		if (imageOperands != EImageOperands::None)
		{
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
			m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
		}
	}

	void CodeBuffer::OpImageSampleProjExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0007'005C + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImage);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
		m_Buffer.emplace_back(operand1);
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpImageSampleProjDrefImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0006'005D + static_cast<std::uint32_t>(imageOperands != EImageOperands::None ? (1 + operands.size()) << 16 : 0));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImage);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(Dref);
		if (imageOperands != EImageOperands::None)
		{
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
			m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
		}
	}

	void CodeBuffer::OpImageSampleProjDrefExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0008'005E + static_cast<std::uint32_t>(operands.size() << 16));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImage);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(Dref);
		m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
		m_Buffer.emplace_back(operand1);
		m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
	}

	void CodeBuffer::OpImageFetch(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'005F + static_cast<std::uint32_t>(imageOperands != EImageOperands::None ? (1 + operands.size()) << 16 : 0));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(image);
		m_Buffer.emplace_back(coordinate);
		if (imageOperands != EImageOperands::None)
		{
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
			m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
		}
	}

	void CodeBuffer::OpImageGather(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, std::uint32_t component, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0006'0060 + static_cast<std::uint32_t>(imageOperands != EImageOperands::None ? (1 + operands.size()) << 16 : 0));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(image);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(component);
		if (imageOperands != EImageOperands::None)
		{
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
			m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
		}
	}

	void CodeBuffer::OpImageDrefGather(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0006'0061 + static_cast<std::uint32_t>(imageOperands != EImageOperands::None ? (1 + operands.size()) << 16 : 0));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(image);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(Dref);
		if (imageOperands != EImageOperands::None)
		{
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
			m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
		}
	}

	void CodeBuffer::OpImageRead(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0005'0062 + static_cast<std::uint32_t>(imageOperands != EImageOperands::None ? (1 + operands.size()) << 16 : 0));
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(image);
		m_Buffer.emplace_back(coordinate);
		if (imageOperands != EImageOperands::None)
		{
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
			m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
		}
	}

	void CodeBuffer::OpImageWrite(std::uint32_t image, std::uint32_t coordinate, std::uint32_t texel, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0063 + static_cast<std::uint32_t>(imageOperands != EImageOperands::None ? (1 + operands.size()) << 16 : 0));
		m_Buffer.emplace_back(image);
		m_Buffer.emplace_back(coordinate);
		m_Buffer.emplace_back(texel);
		if (imageOperands != EImageOperands::None)
		{
			m_Buffer.emplace_back(static_cast<std::uint32_t>(imageOperands));
			m_Buffer.insert(m_Buffer.end(), operands.begin(), operands.end());
		}
	}

	void CodeBuffer::OpImage(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0064);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(sampledImage);
	}

	void CodeBuffer::OpImageQueryFormat(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(0x0004'0065);
		m_Buffer.emplace_back(resultType);
		m_Buffer.emplace_back(resultID);
		m_Buffer.emplace_back(image);
	}

	void CodeBuffer::OpImageQueryOrder(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image)
	{
	}

	void CodeBuffer::OpImageQuerySizeLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t levelOfDetail)
	{
	}

	void CodeBuffer::OpImageQuerySize(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image)
	{
	}

	void CodeBuffer::OpImageQueryLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate)
	{
	}

	void CodeBuffer::OpImageQueryLevels(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image)
	{
	}

	void CodeBuffer::OpImageQuerySamples(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image)
	{
	}

	void CodeBuffer::OpImageSparseSampleImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseSampleExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseSampleDrefImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseSampleDrefExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseSampleProjImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseSampleProjExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseSampleProjDrefImplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseSampleProjDrefExplicitLod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, std::uint32_t operand1, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseFetch(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseGather(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t component, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseDrefGather(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t Dref, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSparseTexelsResident(std::uint32_t resultType, std::uint32_t resultId, std::uint32_t residentCode)
	{
	}

	void CodeBuffer::OpImageSparseRead(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpImageSampleFootprintNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t sampledImage, std::uint32_t coordinate, std::uint32_t granularity, std::uint32_t coarse, EImageOperands imageOperands, const std::vector<std::uint32_t>& operands)
	{
	}

	void CodeBuffer::OpConvertFToU(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t floatValue)
	{
	}

	void CodeBuffer::OpConvertFToS(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t floatValue)
	{
	}

	void CodeBuffer::OpConvertSToF(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t signedValue)
	{
	}

	void CodeBuffer::OpConvertUToF(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t unsignedValue)
	{
	}

	void CodeBuffer::OpUConvert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t unsignedValue)
	{
	}

	void CodeBuffer::OpSConvert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t signedValue)
	{
	}

	void CodeBuffer::OpFConvert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t floatValue)
	{
	}

	void CodeBuffer::OpQuantizeToF16(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value)
	{
	}

	void CodeBuffer::OpConvertPtrToU(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer)
	{
	}

	void CodeBuffer::OpSatConvertSToU(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t signedValue)
	{
	}

	void CodeBuffer::OpSatConvertUToS(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t unsignedValue)
	{
	}

	void CodeBuffer::OpConvertUToPtr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t integerValue)
	{
	}

	void CodeBuffer::OpPtrCastToGeneric(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer)
	{
	}

	void CodeBuffer::OpGenericCastToPtr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer)
	{
	}

	void CodeBuffer::OpGenericCastToPtrExplicit(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, EStorageClass storageClass)
	{
	}

	void CodeBuffer::OpBitcast(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand)
	{
	}

	void CodeBuffer::OpVectorExtractDynamic(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector, std::uint32_t index)
	{
	}

	void CodeBuffer::OpVectorInsertDynamic(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector, std::uint32_t component, std::uint32_t index)
	{
	}

	void CodeBuffer::OpVectorShuffle(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, const std::vector<std::uint32_t>& components)
	{
	}

	void CodeBuffer::OpCompositeConstruct(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::uint32_t>& constituents)
	{
	}

	void CodeBuffer::OpCompositeExtract(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t composite, const std::vector<std::uint32_t>& indices)
	{
	}

	void CodeBuffer::OpCompositeInsert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t object, std::uint32_t composite, const std::vector<std::uint32_t>& indices)
	{
	}

	void CodeBuffer::OpCopyObject(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand)
	{
	}

	void CodeBuffer::OpTranspose(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t matrix)
	{
	}

	void CodeBuffer::OpCopyLogical(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand)
	{
	}

	void CodeBuffer::OpSNegate(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand)
	{
	}

	void CodeBuffer::OpFNegate(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand)
	{
	}

	void CodeBuffer::OpIAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpISub(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFSub(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpIMul(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFMul(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUDiv(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpSDiv(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFDiv(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUMod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpSRem(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpSMod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFRem(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFMod(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpVectorTimesScalar(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector, std::uint32_t scalar)
	{
	}

	void CodeBuffer::OpMatrixTimesScalar(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t matrix, std::uint32_t scalar)
	{
	}

	void CodeBuffer::OpVectorTimesMatrix(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector, std::uint32_t matrix)
	{
	}

	void CodeBuffer::OpMatrixTimesVector(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t matrix, std::uint32_t vector)
	{
	}

	void CodeBuffer::OpMatrixTimesMatrix(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t matrix1, std::uint32_t matrix2)
	{
	}

	void CodeBuffer::OpOuterProduct(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2)
	{
	}

	void CodeBuffer::OpDot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2)
	{
	}

	void CodeBuffer::OpIAddCarry(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpISubBorrow(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUMulExtended(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpSMulExtended(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpSDot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, EPackedVectorFormat packedVectorFormat)
	{
	}

	void CodeBuffer::OpUDot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, EPackedVectorFormat packedVectorFormat)
	{
	}

	void CodeBuffer::OpSUDot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, EPackedVectorFormat packedVectorFormat)
	{
	}

	void CodeBuffer::OpSDotAccSat(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, std::uint32_t accumulator, EPackedVectorFormat packedVectorFormat)
	{
	}

	void CodeBuffer::OpUDotAccSat(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, std::uint32_t accumulator, EPackedVectorFormat packedVectorFormat)
	{
	}

	void CodeBuffer::OpSUDotAccSat(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector1, std::uint32_t vector2, std::uint32_t accumulator, EPackedVectorFormat packedVectorFormat)
	{
	}

	void CodeBuffer::OpShiftRightLogical(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t shift)
	{
	}

	void CodeBuffer::OpShiftRightArithmetic(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t shift)
	{
	}

	void CodeBuffer::OpShiftLeftLogical(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t shift)
	{
	}

	void CodeBuffer::OpShiftLeftArithmetic(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t shift)
	{
	}

	void CodeBuffer::OpBitwiseOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpBitwiseXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpBitwiseAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpNot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand)
	{
	}

	void CodeBuffer::OpBitFieldInsert(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t insert, std::uint32_t offset, std::uint32_t count)
	{
	}

	void CodeBuffer::OpBitFieldSExtract(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t offset, std::uint32_t count)
	{
	}

	void CodeBuffer::OpBitFieldUExtract(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base, std::uint32_t offset, std::uint32_t count)
	{
	}

	void CodeBuffer::OpBitReverse(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base)
	{
	}

	void CodeBuffer::OpBitCount(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t base)
	{
	}

	void CodeBuffer::OpAny(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector)
	{
	}

	void CodeBuffer::OpAll(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t vector)
	{
	}

	void CodeBuffer::OpIsNan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x)
	{
	}

	void CodeBuffer::OpIsInf(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x)
	{
	}

	void CodeBuffer::OpIsFinite(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x)
	{
	}

	void CodeBuffer::OpIsNormal(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x)
	{
	}

	void CodeBuffer::OpSignBitSet(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x)
	{
	}

	void CodeBuffer::OpLessOrGreater(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x, std::uint32_t y)
	{
	}

	void CodeBuffer::OpOrdered(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x, std::uint32_t y)
	{
	}

	void CodeBuffer::OpUnordered(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t x, std::uint32_t y)
	{
	}

	void CodeBuffer::OpLogicalEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpLogicalNotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpLogicalOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpLogicalAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpLogicalNot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand)
	{
	}

	void CodeBuffer::OpSelect(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t condition, std::uint32_t object1, std::uint32_t object2)
	{
	}

	void CodeBuffer::OpIEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpINotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUGreaterThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpSGreaterThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUGreaterThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpSGreaterThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpULessThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpSLessThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpULessThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpSLessThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFOrdEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFUnordEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFOrdNotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFUnordNotEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFOrdLessThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFUnordLessThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFOrdGreaterThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFUnordGreaterThan(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFOrdLessThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFUnordLessThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFOrdGreaterThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpFUnordGreaterThanEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t operand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpDPdx(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P)
	{
	}

	void CodeBuffer::OpDPdy(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P)
	{
	}

	void CodeBuffer::OpFWidth(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P)
	{
	}

	void CodeBuffer::OpDPdxFine(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P)
	{
	}

	void CodeBuffer::OpDPdyFine(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P)
	{
	}

	void CodeBuffer::OpFWidthFine(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P)
	{
	}

	void CodeBuffer::OpDPdxCoarse(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P)
	{
	}

	void CodeBuffer::OpDPdyCoarse(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P)
	{
	}

	void CodeBuffer::OpFWidthCoarse(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t P)
	{
	}

	void CodeBuffer::OpPhi(std::uint32_t resultType, std::uint32_t resultID, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& variableParentPairs)
	{
	}

	void CodeBuffer::OpLoopMerge(std::uint32_t mergeBlock, std::uint32_t continueTarget, ELoopControl loopControl, const std::vector<std::uint32_t>& loopControlParameters)
	{
	}

	void CodeBuffer::OpSelectionMerge(std::uint32_t mergeBlock, ESelectionControl selectionControl)
	{
	}

	void CodeBuffer::OpLabel(std::uint32_t resultID)
	{
	}

	void CodeBuffer::OpBranch(std::uint32_t targetLabel)
	{
	}

	void CodeBuffer::OpBranchConditional(std::uint32_t condition, std::uint32_t trueLabel, std::uint32_t falseLabel, const std::vector<std::uint32_t>& branchWeights)
	{
	}

	void CodeBuffer::OpSwitch(std::uint32_t selector, std::uint32_t def, const std::vector<std::pair<std::uint32_t, std::uint32_t>>& target)
	{
	}

	void CodeBuffer::OpReturn()
	{
	}

	void CodeBuffer::OpReturnValue(std::uint32_t value)
	{
	}

	void CodeBuffer::OpUnreachable()
	{
	}

	void CodeBuffer::OpLifetimeStart(std::uint32_t pointer, std::uint32_t size)
	{
	}

	void CodeBuffer::OpLifetimeStop(std::uint32_t pointer, std::uint32_t size)
	{
	}

	void CodeBuffer::OpTerminateInvocation()
	{
	}

	void CodeBuffer::OpDemoteToHelperInvocation()
	{
	}

	void CodeBuffer::OpAtomicLoad(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpAtomicStore(std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicExchange(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicCompareExchange(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semanticsEqual, std::uint32_t semanticsUnequal, std::uint32_t value, std::uint32_t comparator)
	{
	}

	void CodeBuffer::OpAtomicCompareExchangeWeak(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semanticsEqual, std::uint32_t semanticsUnequal, std::uint32_t value, std::uint32_t comparator)
	{
	}

	void CodeBuffer::OpAtomicIIncrement(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpAtomicIDecrement(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpAtomicIAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicISub(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicSMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicUMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicSMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicUMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicFlagTestAndSet(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpAtomicFlagClear(std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpAtomicFMinEXT(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicFMaxEXT(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpAtomicFAddEXT(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t memory, std::uint32_t semantics, std::uint32_t value)
	{
	}

	void CodeBuffer::OpEmitVertex()
	{
	}

	void CodeBuffer::OpEndPrimitive()
	{
	}

	void CodeBuffer::OpEmitStreamVertex(std::uint32_t stream)
	{
	}

	void CodeBuffer::OpEndStreamPrimitive(std::uint32_t stream)
	{
	}

	void CodeBuffer::OpControlBarrier(std::uint32_t execution, std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpMemoryBarrier(std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpNamedBarrierInitialize(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t subgroupCount)
	{
	}

	void CodeBuffer::OpMemoryNamedBarrier(std::uint32_t namedBarrier, std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpControlBarrierArriveINTEL(std::uint32_t execution, std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpControlBarrierWaitINTEL(std::uint32_t execution, std::uint32_t memory, std::uint32_t semantics)
	{
	}

	void CodeBuffer::OpGroupAsyncCopy(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t destination, std::uint32_t source, std::uint32_t numElements, std::uint32_t stride, std::uint32_t event)
	{
	}

	void CodeBuffer::OpGroupWaitEvents(std::uint32_t execution, std::uint32_t numEvents, std::uint32_t eventsList)
	{
	}

	void CodeBuffer::OpGroupAll(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t predicate)
	{
	}

	void CodeBuffer::OpGroupAny(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t predicate)
	{
	}

	void CodeBuffer::OpGroupBroadcast(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t localID)
	{
	}

	void CodeBuffer::OpGroupIAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupFAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupFMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupUMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupSMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupFMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupUMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupSMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpSubgroupBallotKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value)
	{
	}

	void CodeBuffer::OpSubgroupFirstInvocationKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value)
	{
	}

	void CodeBuffer::OpSubgroupAllKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t predicate)
	{
	}

	void CodeBuffer::OpSubgroupAnyKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t predicate)
	{
	}

	void CodeBuffer::OpSubgroupAllEqualKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t predicate)
	{
	}

	void CodeBuffer::OpGroupNonUniformRotateKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t delta, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpSubgroupReadInvocationKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value, std::uint32_t index)
	{
	}

	void CodeBuffer::OpGroupIAddNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupFAddNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupFMinNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupUMinNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupSMinNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupFMaxNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupUMaxNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupSMaxNonUniformAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpSubgroupSuffleINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t data, std::uint32_t invocationID)
	{
	}

	void CodeBuffer::OpSubgroupSuffleDownINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t current, std::uint32_t next, std::uint32_t data)
	{
	}

	void CodeBuffer::OpSubgroupSuffleUpINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t current, std::uint32_t next, std::uint32_t data)
	{
	}

	void CodeBuffer::OpSubgroupSuffleXorINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t data, std::uint32_t value)
	{
	}

	void CodeBuffer::OpSubgroupBlockReadINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t ptr)
	{
	}

	void CodeBuffer::OpSubgroupBlockWriteINTEL(std::uint32_t ptr, std::uint32_t data)
	{
	}

	void CodeBuffer::OpSubgroupImageBlockReadINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate)
	{
	}

	void CodeBuffer::OpSubgroupImageBlockWriteINTEL(std::uint32_t image, std::uint32_t coordinate, std::uint32_t data)
	{
	}

	void CodeBuffer::OpSubgroupImageMediaBlockReadINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, std::uint32_t width, std::uint32_t height)
	{
	}

	void CodeBuffer::OpSubgroupImageMediaBlockWriteINTEL(std::uint32_t image, std::uint32_t coordinate, std::uint32_t width, std::uint32_t height, std::uint32_t data)
	{
	}

	void CodeBuffer::OpGroupIMulKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupFMulKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupBitwiseAndKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupBitwiseOrKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupBitwiseXorKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupLogicalAndKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupLogicalOrKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpGroupLogicalXorKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t x)
	{
	}

	void CodeBuffer::OpEnqueueMarker(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t queue, std::uint32_t numEvents, std::uint32_t waitEvents, std::uint32_t retEvent)
	{
	}

	void CodeBuffer::OpEnqueueKernel(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t queue, std::uint32_t flags, std::uint32_t ndRange, std::uint32_t numEvents, std::uint32_t waitEvents, std::uint32_t retEvent, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign, const std::vector<std::uint32_t>& localSizes)
	{
	}

	void CodeBuffer::OpGetKernelINDrangeSubGroupCount(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t ndRange, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign)
	{
	}

	void CodeBuffer::OpGetKernelINDrangeMaxSubGroupSize(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t ndRange, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign)
	{
	}

	void CodeBuffer::OpGetKernelWorkGroupSize(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign)
	{
	}

	void CodeBuffer::OpGetKernelPreferredWorkGroupSizeMultiple(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign)
	{
	}

	void CodeBuffer::OpRetainEvent(std::uint32_t event)
	{
	}

	void CodeBuffer::OpReleaseEvent(std::uint32_t event)
	{
	}

	void CodeBuffer::OpCreateUserEvent(std::uint32_t resultType, std::uint32_t resultID)
	{
	}

	void CodeBuffer::OpIsValidEvent(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t event)
	{
	}

	void CodeBuffer::OpSetUserEventStatus(std::uint32_t event, std::uint32_t status)
	{
	}

	void CodeBuffer::OpCaptureEventProfilingInfo(std::uint32_t event, std::uint32_t profilingInfo, std::uint32_t value)
	{
	}

	void CodeBuffer::OpGetDefaultQueue(std::uint32_t resultType, std::uint32_t resultID)
	{
	}

	void CodeBuffer::OpBuildNDRange(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t globalWorkSize, std::uint32_t localWorkSize, std::uint32_t globalWorkOffset)
	{
	}

	void CodeBuffer::OpGetKernelLocalSizeForSubgroupCount(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t subgroupCount, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign)
	{
	}

	void CodeBuffer::OpGetKernelMaxNumSubgroups(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t invoke, std::uint32_t param, std::uint32_t paramSize, std::uint32_t paramAlign)
	{
	}

	void CodeBuffer::OpReadPipe(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t pointer, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpWritePipe(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t pointer, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpReservedReadPipe(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t index, std::uint32_t pointer, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpReservedWritePipe(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t index, std::uint32_t pointer, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpReserveReadPipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t numPackets, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpReserveWritePipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t numPackets, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpCommitReadPipe(std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpCommitWritePipe(std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpIsValidReserveId(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t reserveID)
	{
	}

	void CodeBuffer::OpGetNumPipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpGetMaxPipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipe, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpGroupReserveReadPipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t pipe, std::uint32_t numPackets, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpGroupReserveWritePipePackets(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t pipe, std::uint32_t numPackets, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpGroupCommitReadPipe(std::uint32_t execution, std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpGroupCommitWritePipe(std::uint32_t execution, std::uint32_t pipe, std::uint32_t reserveID, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpConstantPipeStorage(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t packetSize, std::uint32_t packetAlignment, std::uint32_t capacity)
	{
	}

	void CodeBuffer::OpCreatePipeFromPipeStorage(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pipeStorage)
	{
	}

	void CodeBuffer::OpReadPipeBlockingINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpWritePipeBlockingINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t packetSize, std::uint32_t packetAlignment)
	{
	}

	void CodeBuffer::OpGroupNonUniformElect(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution)
	{
	}

	void CodeBuffer::OpGroupNonUniformAll(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t predicate)
	{
	}

	void CodeBuffer::OpGroupNonUniformAny(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t predicate)
	{
	}

	void CodeBuffer::OpGroupNonUniformAllEqual(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value)
	{
	}

	void CodeBuffer::OpGroupNonUniformBroadcast(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t id)
	{
	}

	void CodeBuffer::OpGroupNonUniformBroadcastFirst(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value)
	{
	}

	void CodeBuffer::OpGroupNonUniformBallot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value)
	{
	}

	void CodeBuffer::OpGroupNonUniformInveseBallot(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value)
	{
	}

	void CodeBuffer::OpGroupNonUniformBallotBitExtract(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t index)
	{
	}

	void CodeBuffer::OpGroupNonUniformBallotBitCount(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value)
	{
	}

	void CodeBuffer::OpGroupNonUniformBallotFindLSB(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value)
	{
	}

	void CodeBuffer::OpGroupNonUniformBallotFindMSB(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value)
	{
	}

	void CodeBuffer::OpGroupNonUniformShuffle(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t id)
	{
	}

	void CodeBuffer::OpGroupNonUniformShuffleXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t mask)
	{
	}

	void CodeBuffer::OpGroupNonUniformShuffleUp(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t delta)
	{
	}

	void CodeBuffer::OpGroupNonUniformShuffleDown(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t delta)
	{
	}

	void CodeBuffer::OpGroupNonUniformIAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformFAdd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformIMul(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformFMul(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformSMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformUMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformFMin(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformSMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformUMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformFMax(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformBitwiseAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformBitwiseOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformBitwiseXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformLogicalAnd(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformLogicalOr(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformLogicalXor(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, EGroupOperation groupOperation, std::uint32_t value, std::uint32_t clusterSize)
	{
	}

	void CodeBuffer::OpGroupNonUniformQuadBroadcast(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t index)
	{
	}

	void CodeBuffer::OpGroupNonUniformQuadSwap(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t execution, std::uint32_t value, std::uint32_t direction)
	{
	}

	void CodeBuffer::OpGroupNonUniformPartitionNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t value)
	{
	}

	void CodeBuffer::OpTraceRayKHR(std::uint32_t accel, std::uint32_t rayFlags, std::uint32_t cullMask, std::uint32_t SBTOffset, std::uint32_t SBTStride, std::uint32_t missIndex, std::uint32_t rayOrigin, std::uint32_t rayTmin, std::uint32_t rayDirection, std::uint32_t rayTmax, std::uint32_t rayPayload)
	{
	}

	void CodeBuffer::OpExecuteCallableKHR(std::uint32_t SBTIndex, std::uint32_t callableData)
	{
	}

	void CodeBuffer::OpConvertUToAccelerationStructureKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t accel)
	{
	}

	void CodeBuffer::OpIgnoreIntersectionKHR()
	{
	}

	void CodeBuffer::OpTerminateRayKHR()
	{
	}

	void CodeBuffer::OpTypeRayQueryKHR(std::uint32_t resultID)
	{
	}

	void CodeBuffer::OpRayQueryGenerateIntersectionKHR(std::uint32_t rayQuery, std::uint32_t hit)
	{
	}

	void CodeBuffer::OpRayQueryConfirmIntersectionKHR(std::uint32_t rayQuery)
	{
	}

	void CodeBuffer::OpRayQueryProceedKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQuery)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionTypeKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpFragmentMaskFetchAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate)
	{
	}

	void CodeBuffer::OpFragmentFetchAMD(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t image, std::uint32_t coordinate, std::uint32_t fragmentIndex)
	{
	}

	void CodeBuffer::OpReadClockKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t scope)
	{
	}

	void CodeBuffer::OpWritePackedPrimitiveIndices4x8NV(std::uint32_t indexOffset, std::uint32_t packedIndices)
	{
	}

	void CodeBuffer::OpReportIntersectionKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t hit, std::uint32_t hitKind)
	{
	}

	void CodeBuffer::OpIgnoreIntersectionNV()
	{
	}

	void CodeBuffer::OpTerminateRayNV()
	{
	}

	void CodeBuffer::OpTraceNV(std::uint32_t accel, std::uint32_t rayFlags, std::uint32_t cullMask, std::uint32_t SBTOffset, std::uint32_t SBTStride, std::uint32_t missIndex, std::uint32_t rayOrigin, std::uint32_t rayTmin, std::uint32_t rayDirection, std::uint32_t rayTmax, std::uint32_t rayPayload)
	{
	}

	void CodeBuffer::OpTraceMotionNV(std::uint32_t accel, std::uint32_t rayFlags, std::uint32_t cullMask, std::uint32_t SBTOffset, std::uint32_t SBTStride, std::uint32_t missIndex, std::uint32_t rayOrigin, std::uint32_t rayTmin, std::uint32_t rayDirection, std::uint32_t rayTmax, std::uint32_t time, std::uint32_t rayPayload)
	{
	}

	void CodeBuffer::OpTraceRayMotionNV(std::uint32_t accel, std::uint32_t rayFlags, std::uint32_t cullMask, std::uint32_t SBTOffset, std::uint32_t SBTStride, std::uint32_t missIndex, std::uint32_t rayOrigin, std::uint32_t rayTmin, std::uint32_t rayDirection, std::uint32_t rayTmax, std::uint32_t time, std::uint32_t rayPayload)
	{
	}

	void CodeBuffer::OpTypeAccelerationStructureKHR(std::uint32_t resultID)
	{
	}

	void CodeBuffer::OpExecuteCallableNV(std::uint32_t SBTIndex, std::uint32_t callableDataID)
	{
	}

	void CodeBuffer::OpTypeCooperativeMatrixNV(std::uint32_t resultID, std::uint32_t componentType, std::uint32_t execution, std::uint32_t rows, std::uint32_t columns)
	{
	}

	void CodeBuffer::OpCooperativeMatrixLoadNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t pointer, std::uint32_t stride, std::uint32_t columnMajor, EMemoryOperands memoryOperands)
	{
	}

	void CodeBuffer::OpCooperativeMatrixStoreNV(std::uint32_t pointer, std::uint32_t object, std::uint32_t stride, std::uint32_t columnMajor, EMemoryOperands memoryOperands)
	{
	}

	void CodeBuffer::OpCooperativeMatrixMulAddNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t a, std::uint32_t b, std::uint32_t c)
	{
	}

	void CodeBuffer::OpCooperativeMatrixLengthNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t type)
	{
	}

	void CodeBuffer::OpBeginInvocationInterlockEXT()
	{
	}

	void CodeBuffer::OpEndInvocationInterlockEXT()
	{
	}

	void CodeBuffer::OpIsHelperInvocationEXT(std::uint32_t resultType, std::uint32_t resultID)
	{
	}

	void CodeBuffer::OpConvertUToImageNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand)
	{
	}

	void CodeBuffer::OpConvertUToSamplerNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand)
	{
	}

	void CodeBuffer::OpConvertImageToUNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand)
	{
	}

	void CodeBuffer::OpConvertSamplerToUNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand)
	{
	}

	void CodeBuffer::OpConvertUToSampledImageNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand)
	{
	}

	void CodeBuffer::OpConvertSampledImageToUNV(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand)
	{
	}

	void CodeBuffer::OpSamplerImageAddressingModeNV(std::uint32_t bitWidth)
	{
	}

	void CodeBuffer::OpUCountLeadingZerosINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand)
	{
	}

	void CodeBuffer::OpUCountTrailingZerosINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand)
	{
	}

	void CodeBuffer::OpAbsISubINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpAbsUSubINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpIAddSatINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUAddSatINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpIAverageINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUAverageINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpIAverageRoundedINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUAverageRoundedINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpISubSatINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUSubSatINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpIMul32x16INTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpUMul32x16INTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t opreand1, std::uint32_t operand2)
	{
	}

	void CodeBuffer::OpLoopControlINTEL(const std::vector<std::uint32_t>& loopControlParameters)
	{
	}

	void CodeBuffer::OpFPGARegINTEL(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t result, std::uint32_t input)
	{
	}

	void CodeBuffer::OpRayQueryGetRayTMinKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery)
	{
	}

	void CodeBuffer::OpRayQueryGetRayFlagsKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionInstanceCustomIndexKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionInstanceIdKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionInstanceShaderBindingTableRecordOffsetKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionGeometryIndexKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionPrimitiveIndexKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionBarycentricsKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionFrontFaceKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionCandidateAABBOpaqueKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionObjectRayDirectionKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionObjectRayOriginKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetWorldRayDirectionKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery)
	{
	}

	void CodeBuffer::OpRayQueryGetWorldRayOriginKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionObjectToWorldKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
	}

	void CodeBuffer::OpRayQueryGetIntersectionWorldToObjectKHR(std::uint32_t resultType, std::uint32_t resultID, std::uint32_t rayQeuery, std::uint32_t intersection)
	{
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
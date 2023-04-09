#pragma once

#include "Enumerations.h"

#include <string_view>
#include <vector>

namespace Frertex::Transpilers::SPIRV
{
	struct CodeBuffer
	{
	public:
		void pushOpSource(std::uint32_t language, std::uint32_t version, std::uint32_t fileID = 0);
		void pushOpMemoryModel(EAddressingMode addessingMode, EMemoryModel memoryModel);
		void pushOpEntryPoint(EExecutionModel executionModel, std::uint32_t entrypoint, std::string_view name, const std::vector<std::uint32_t>& interface);
		void pushOpExecutionMode(std::uint32_t entrypoint, EExecutionMode mode, const std::vector<std::uint32_t>& literals = {});
		void pushOpCapability(ECapability capability);

		void pushOpTypeVoid(std::uint32_t result);
		void pushOpTypeBool(std::uint32_t result);
		void pushOpTypeInt(std::uint32_t result, std::uint32_t width, std::uint32_t signedness);
		void pushOpTypeFloat(std::uint32_t result, std::uint32_t width);
		void pushOpTypeVector(std::uint32_t result, std::uint32_t componentType, std::uint32_t componentCount);
		void pushOpTypeMatrix(std::uint32_t result, std::uint32_t columnType, std::uint32_t columnCount);
		void pushOpTypeImage(std::uint32_t result, std::uint32_t sampledType, EDim dim, std::uint32_t depth, std::uint32_t arrayed, std::uint32_t ms, std::uint32_t sampled, std::uint32_t format, EAccessQualifier accessQualifier);
		void pushOpTypeSampler(std::uint32_t result);
		void pushOpTypeSampledImage(std::uint32_t result, std::uint32_t imageType);
		void pushOpTypeArray(std::uint32_t result, std::uint32_t elementType, std::uint32_t length);
		void pushOpTypeRuntimeArray(std::uint32_t result, std::uint32_t elementType);
		void pushOpTypeStruct(std::uint32_t result, const std::vector<std::uint32_t>& memberTypes);
		void pushOpTypeOpaque(std::uint32_t result, std::string_view name);
		void pushOpTypePointer(std::uint32_t result, EStorageClass storageClass, std::uint32_t type);
		void pushOpTypeFunction(std::uint32_t result, std::uint32_t returnType, const std::vector<std::uint32_t>& parameterTypes);
		void pushOpTypeEvent(std::uint32_t result);
		void pushOpTypeDeviceEvent(std::uint32_t result);

		void pushOpConstantTrue(std::uint32_t resultType, std::uint32_t result);
		void pushOpConstantFalse(std::uint32_t resultType, std::uint32_t result);
		void pushOpConstant(std::uint32_t resultType, std::uint32_t result, const std::vector<std::uint32_t>& value);
		void pushOpConstantComposite(std::uint32_t resultType, std::uint32_t result, const std::vector<std::uint32_t>& constituents);
		void pushOpConstantSampler(std::uint32_t resultType, std::uint32_t result, ESamplerAddressingMode addressingMode, std::uint32_t param, ESamplerFilterMode filterMode);
		void pushOpConstantNull(std::uint32_t resultType, std::uint32_t result);
		void pushOpSpecConstantTrue(std::uint32_t resultType, std::uint32_t result);
		void pushOpSpecConstantFalse(std::uint32_t resultType, std::uint32_t result);
		void pushOpSpecConstant(std::uint32_t resultType, std::uint32_t result, const std::vector<std::uint32_t>& value);
		void pushOpSpecConstantComposite(std::uint32_t resultType, std::uint32_t result, const std::vector<std::uint32_t>& constituents);

		void pushOpFunction(std::uint32_t returnType, std::uint32_t result, EFunctionControl functionControl, std::uint32_t functionType);
		void pushOpFunctionParameter(std::uint32_t resultType, std::uint32_t result);
		void pushOpFunctionEnd();
		void pushOpFunctionCall(std::uint32_t resultType, std::uint32_t result, std::uint32_t function, const std::vector<std::uint32_t> arguments = {});

		void pushOpVariable(std::uint32_t resultType, std::uint32_t result, EStorageClass storageClass, std::uint32_t initializer = 0);

		void pushOpDecorate(std::uint32_t target, EDecoration decoration, const std::vector<std::uint32_t>& literals = {});
		void pushOpMemberDecorate(std::uint32_t structureType, std::uint32_t member, EDecoration decoration, const std::vector<std::uint32_t>& literals = {});

		void pushOpLabel(std::uint32_t result);
		void pushOpReturn();
		void pushOpReturnValue(std::uint32_t value);
		void pushOpUnreachable();

		void pushCodeBuffer(const CodeBuffer& buffer);

		const std::vector<std::uint32_t>& getCode() const { return m_Code; }

	private:
		std::vector<std::uint32_t> m_Code;
	};
} // namespace Frertex::Transpilers::SPIRV
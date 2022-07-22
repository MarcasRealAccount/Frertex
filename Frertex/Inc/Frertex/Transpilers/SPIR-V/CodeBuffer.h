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

		void pushOpFunction(std::uint32_t returnType, std::uint32_t result, EFunctionControl functionControl, std::uint32_t functionType);
		void pushOpFunctionParameter(std::uint32_t resultType, std::uint32_t result);
		void pushOpFunctionEnd();

		void pushOpVariable(std::uint32_t resultType, std::uint32_t result, EStorageClass storageClass, std::uint32_t initializer = 0);

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
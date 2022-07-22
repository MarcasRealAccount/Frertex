#pragma once

#include "Frertex/FIL.h"
#include "Frertex/Message.h"
#include "Frertex/Utils/Buffer.h"
#include "Frertex/Utils/Utils.h"
#include "SPIR-V/CodeBuffer.h"

namespace Frertex::Transpilers::SPIRV
{
	struct SPIRV
	{
	public:
		std::uint32_t m_IDBound = 1;
		CodeBuffer    m_Code;
	};

	std::vector<std::uint8_t> GetSPIRVBinary(const SPIRV& spirv);

	void WriteSPIRVToFile(const std::filesystem::path& file, const SPIRV& spirv);

	class SPIRVTranspiler
	{
	public:
		SPIRVTranspiler();

		SPIRV transpile(Utils::CopyMovable<FIL>&& fil);

		void addWarning(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message);
		void addError(SourceSpan span, SourcePoint point, Utils::CopyMovable<std::string>&& message);

		auto& getMessages() const { return m_Messages; }

	private:
		std::vector<Message> m_Messages;
	};
} // namespace Frertex::Transpilers::SPIRV
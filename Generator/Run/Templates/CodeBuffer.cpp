#include "Frertex/Transpilers/SPIRV/CodeBuffer.h"
#include "Frertex/Utils/Profiler.h"

#include <cstdint>

#include <string>

namespace Frertex::Transpilers::SPIRV
{
	$$FOREACH:INSTRUCTION=INSTRUCTION_IMPLS$$
	$$TEMPLATE:"InstructionImpl.h",NAME=INSTRUCTION.NAME,ARGUMENTS=INSTRUCTION.ARGUMENTS,OPCODE=INSTRUCTION.OPCODE,CAPABILITIES=INSTRUCTION.CAPABILITIES,EXTENSIONS=INSTRUCTION.EXTENSIONS$$
	$$END$$

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
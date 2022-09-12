#pragma once

#include <cstdint>

#include <vector>

namespace Frertex::FIL
{
	struct CodeBuffer
	{
	public:
		void NOP();
		void RET();

		void                         reset() { m_Code = {}; }
		std::vector<std::uint32_t>&& get() { return std::move(m_Code); }

	private:
		std::vector<std::uint32_t> m_Code;
	};
} // namespace Frertex::FIL
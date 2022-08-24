#include "Frertex/Utils/Buffer.h"
#include "Frertex/Utils/Profiler.h"

namespace Frertex::Utils
{
	void Buffer::pushU8(std::uint8_t value)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(value);
	}

	void Buffer::pushU16(std::uint16_t value)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(static_cast<std::uint8_t>(value & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
	}

	void Buffer::pushU32(std::uint32_t value)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(static_cast<std::uint8_t>(value & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
	}

	void Buffer::pushU64(std::uint64_t value)
	{
		PROFILE_FUNC;

		m_Buffer.emplace_back(static_cast<std::uint8_t>(value & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 32) & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 40) & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 48) & 0xFF));
		m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 56) & 0xFF));
	}
} // namespace Frertex::Utils
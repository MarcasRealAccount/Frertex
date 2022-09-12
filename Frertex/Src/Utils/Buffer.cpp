#include "Frertex/Utils/Buffer.h"

namespace Frertex::Utils
{
	Buffer::~Buffer()
	{
		if (m_Buffer && m_Allocated)
		{
			delete m_Buffer;
			m_Allocated = false;
		}
	}

	void Buffer::align16()
	{
		m_Offset = ((m_Offset + 1) / 2) * 2;
		if (m_Offset > m_Buffer->size())
			m_Buffer->resize(m_Offset, 0);
	}

	void Buffer::align32()
	{
		m_Offset = ((m_Offset + 3) / 4) * 4;
		if (m_Offset > m_Buffer->size())
			m_Buffer->resize(m_Offset, 0);
	}

	void Buffer::align64()
	{
		m_Offset = ((m_Offset + 7) / 8) * 8;
		if (m_Offset > m_Buffer->size())
			m_Buffer->resize(m_Offset, 0);
	}

	std::uint8_t Buffer::popU8()
	{
		std::uint8_t v = m_Buffer && m_Offset < m_Buffer->size() ? (*m_Buffer)[m_Offset] : 0;
		++m_Offset;
		return v;
	}

	std::uint16_t Buffer::popU16()
	{
		std::uint16_t v = 0;
		v |= popU8();
		v |= static_cast<std::uint16_t>(popU8()) << 8;
		return v;
	}

	std::uint32_t Buffer::popU32()
	{
		std::uint32_t v = 0;
		v |= popU8();
		v |= static_cast<std::uint32_t>(popU8()) << 8;
		v |= static_cast<std::uint32_t>(popU8()) << 16;
		v |= static_cast<std::uint32_t>(popU8()) << 24;
		return v;
	}

	std::uint64_t Buffer::popU64()
	{
		std::uint64_t v = 0;
		v |= popU8();
		v |= static_cast<std::uint64_t>(popU8()) << 8;
		v |= static_cast<std::uint64_t>(popU8()) << 16;
		v |= static_cast<std::uint64_t>(popU8()) << 24;
		v |= static_cast<std::uint64_t>(popU8()) << 32;
		v |= static_cast<std::uint64_t>(popU8()) << 40;
		v |= static_cast<std::uint64_t>(popU8()) << 48;
		v |= static_cast<std::uint64_t>(popU8()) << 56;
		return v;
	}

	void Buffer::popU8s(std::uint8_t* start, std::uint8_t* end)
	{
		for (; start != end; ++start)
			*start = popU8();
	}

	void Buffer::popU16s(std::uint16_t* start, std::uint16_t* end)
	{
		for (; start != end; ++start)
			*start = popU16();
	}

	void Buffer::popU32s(std::uint32_t* start, std::uint32_t* end)
	{
		for (; start != end; ++start)
			*start = popU32();
	}

	void Buffer::popU64s(std::uint64_t* start, std::uint64_t* end)
	{
		for (; start != end; ++start)
			*start = popU64();
	}

	void Buffer::pushU8(std::uint8_t value)
	{
		if (m_Offset < m_Buffer->size())
			(*m_Buffer)[m_Offset] = value;
		else
			m_Buffer->emplace_back(value);
		++m_Offset;
	}

	void Buffer::pushU16(std::uint16_t value)
	{
		pushU8(static_cast<std::uint8_t>(value));
		pushU8(static_cast<std::uint8_t>(value >> 8));
	}

	void Buffer::pushU32(std::uint32_t value)
	{
		pushU8(static_cast<std::uint8_t>(value));
		pushU8(static_cast<std::uint8_t>(value >> 8));
		pushU8(static_cast<std::uint8_t>(value >> 16));
		pushU8(static_cast<std::uint8_t>(value >> 24));
	}

	void Buffer::pushU64(std::uint64_t value)
	{
		pushU8(static_cast<std::uint8_t>(value));
		pushU8(static_cast<std::uint8_t>(value >> 8));
		pushU8(static_cast<std::uint8_t>(value >> 16));
		pushU8(static_cast<std::uint8_t>(value >> 24));
		pushU8(static_cast<std::uint8_t>(value >> 32));
		pushU8(static_cast<std::uint8_t>(value >> 40));
		pushU8(static_cast<std::uint8_t>(value >> 48));
		pushU8(static_cast<std::uint8_t>(value >> 56));
	}

	void Buffer::pushU8s(const std::uint8_t* start, const std::uint8_t* end)
	{
		m_Buffer->resize(m_Offset + (end - start));
		for (; start != end; ++start)
			pushU8(*start);
	}

	void Buffer::pushU16s(const std::uint16_t* start, const std::uint16_t* end)
	{
		m_Buffer->resize(m_Offset + (end - start) * sizeof(std::uint16_t));
		for (; start != end; ++start)
			pushU16(*start);
	}

	void Buffer::pushU32s(const std::uint32_t* start, const std::uint32_t* end)
	{
		m_Buffer->resize(m_Offset + (end - start) * sizeof(std::uint32_t));
		for (; start != end; ++start)
			pushU32(*start);
	}

	void Buffer::pushU64s(const std::uint64_t* start, const std::uint64_t* end)
	{
		m_Buffer->resize(m_Offset + (end - start) * sizeof(std::uint64_t));
		for (; start != end; ++start)
			pushU64(*start);
	}
} // namespace Frertex::Utils
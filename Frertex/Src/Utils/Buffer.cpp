#include "Frertex/Utils/Buffer.h"

namespace Frertex::Utils
{
	void ReadBuffer::AlignU16()
	{
		m_Offset = ((m_Offset + 1) >> 1) << 1;
	}

	void ReadBuffer::AlignU32()
	{
		m_Offset = ((m_Offset + 3) >> 2) << 2;
	}

	void ReadBuffer::AlignU64()
	{
		m_Offset = ((m_Offset + 7) >> 3) << 3;
	}

	std::uint8_t ReadBuffer::PopU8()
	{
		std::uint8_t value = m_Offset < m_View.size() ? m_View[m_Offset] : 0U;
		++m_Offset;
		return value;
	}

	std::uint16_t ReadBuffer::PopU16()
	{
		// TODO(MarcasRealAccount): Ensure reading in Little Endian
		if (m_Offset + 1 >= m_View.size())
		{
			m_Offset += 2;
			return 0;
		}

		return m_View[m_Offset++] << 8 |
			   m_View[m_Offset++];
	}

	std::uint32_t ReadBuffer::PopU32()
	{
		// TODO(MarcasRealAccount): Ensure reading in Little Endian
		if (m_Offset + 3 >= m_View.size())
		{
			m_Offset += 4;
			return 0;
		}

		return m_View[m_Offset++] << 24 |
			   m_View[m_Offset++] << 16 |
			   m_View[m_Offset++] << 8 |
			   m_View[m_Offset++];
	}

	std::uint64_t ReadBuffer::PopU64()
	{
		// TODO(MarcasRealAccount): Ensure reading in Little Endian
		if (m_Offset + 7 >= m_View.size())
		{
			m_Offset += 8;
			return 0;
		}

		return static_cast<std::uint64_t>(m_View[m_Offset++]) << 56 |
			   static_cast<std::uint64_t>(m_View[m_Offset++]) << 48 |
			   static_cast<std::uint64_t>(m_View[m_Offset++]) << 40 |
			   static_cast<std::uint64_t>(m_View[m_Offset++]) << 32 |
			   static_cast<std::uint64_t>(m_View[m_Offset++]) << 24 |
			   static_cast<std::uint64_t>(m_View[m_Offset++]) << 16 |
			   static_cast<std::uint64_t>(m_View[m_Offset++]) << 8 |
			   static_cast<std::uint64_t>(m_View[m_Offset++]);
	}

	void ReadBuffer::PopU8s(Span<std::uint8_t> values)
	{
		for (std::size_t i = 0; i < values.size(); ++i)
		{
			values[i] = m_Offset < m_View.size() ? m_View[m_Offset] : 0;
			++m_Offset;
		}
	}

	void ReadBuffer::PopU16s(Span<std::uint16_t> values)
	{
		// TODO(MarcasRealAccount): Ensure reading in Little Endian
		for (std::size_t i = 0; i < values.size(); ++i)
		{
			if (m_Offset + 1 >= m_View.size())
			{
				values[i] = 0;
				m_Offset  += 2;
				continue;
			}

			values[i] = m_View[m_Offset++] << 8 |
						m_View[m_Offset++];
		}
	}

	void ReadBuffer::PopU32s(Span<std::uint32_t> values)
	{
		// TODO(MarcasRealAccount): Ensure reading in Little Endian
		for (std::size_t i = 0; i < values.size(); ++i)
		{
			if (m_Offset + 3 >= m_View.size())
			{
				values[i] = 0;
				m_Offset  += 4;
				continue;
			}

			values[i] = m_View[m_Offset++] << 24 |
						m_View[m_Offset++] << 16 |
						m_View[m_Offset++] << 8 |
						m_View[m_Offset++];
		}
	}

	void ReadBuffer::PopU64s(Span<std::uint64_t> values)
	{
		// TODO(MarcasRealAccount): Ensure reading in Little Endian
		for (std::size_t i = 0; i < values.size(); ++i)
		{
			if (m_Offset + 7 >= m_View.size())
			{
				values[i] = 0;
				m_Offset  += 8;
				continue;
			}

			values[i] = static_cast<std::uint64_t>(m_View[m_Offset++]) << 56 |
						static_cast<std::uint64_t>(m_View[m_Offset++]) << 48 |
						static_cast<std::uint64_t>(m_View[m_Offset++]) << 40 |
						static_cast<std::uint64_t>(m_View[m_Offset++]) << 32 |
						static_cast<std::uint64_t>(m_View[m_Offset++]) << 24 |
						static_cast<std::uint64_t>(m_View[m_Offset++]) << 16 |
						static_cast<std::uint64_t>(m_View[m_Offset++]) << 8 |
						static_cast<std::uint64_t>(m_View[m_Offset++]);
		}
	}

	void WriteBuffer::Ensure(std::uint64_t bytes)
	{
		if (bytes == 0)
			return;

		if (m_BackingBuffer)
		{
			m_BackingBuffer->resize(m_BackingBuffer->size() + bytes);
		}
		else
		{
			m_BackingBuffer = new std::vector<std::uint8_t>(bytes);
			m_Allocated     = true;
		}
	}

	void WriteBuffer::AlignU16()
	{
		std::size_t newOffset = ((m_Offset + 1) >> 1) << 1;
		Ensure(newOffset - m_Offset);
		m_Offset = newOffset;
	}

	void WriteBuffer::AlignU32()
	{
		std::size_t newOffset = ((m_Offset + 3) >> 2) << 2;
		Ensure(newOffset - m_Offset);
		m_Offset = newOffset;
	}

	void WriteBuffer::AlignU64()
	{
		std::size_t newOffset = ((m_Offset + 7) >> 3) << 3;
		Ensure(newOffset - m_Offset);
		m_Offset = newOffset;
	}

	void WriteBuffer::PushU8(std::uint8_t value)
	{
		Ensure(1);
		(*m_BackingBuffer)[m_Offset++] = value;
	}

	void WriteBuffer::PushU16(std::uint16_t value)
	{
		// TODO(MarcasRealAccount): Ensure writing out in Little Endian
		Ensure(2);
		(*m_BackingBuffer)[m_Offset++] = (value >> 8) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = value & 0xFF;
	}

	void WriteBuffer::PushU32(std::uint32_t value)
	{
		// TODO(MarcasRealAccount): Ensure writing out in Little Endian
		Ensure(4);
		(*m_BackingBuffer)[m_Offset++] = (value >> 24) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = (value >> 16) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = (value >> 8) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = value & 0xFF;
	}

	void WriteBuffer::PushU64(std::uint64_t value)
	{
		// TODO(MarcasRealAccount): Ensure writing out in Little Endian
		Ensure(8);
		(*m_BackingBuffer)[m_Offset++] = (value >> 56) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = (value >> 48) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = (value >> 40) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = (value >> 32) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = (value >> 24) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = (value >> 16) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = (value >> 8) & 0xFF;
		(*m_BackingBuffer)[m_Offset++] = value & 0xFF;
	}

	void WriteBuffer::PushU8s(View<std::uint8_t> values)
	{
		Ensure(values.size());
		for (std::size_t i = 0; i < values.size(); ++i)
			(*m_BackingBuffer)[m_Offset++] = values[i];
	}

	void WriteBuffer::PushU16s(View<std::uint16_t> values)
	{
		Ensure(values.size() * 2);
		for (std::size_t i = 0; i < values.size(); ++i)
		{
			// TODO(MarcasRealAccount): Ensure writing out in Little Endian
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 8) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = values[i] & 0xFF;
		}
	}

	void WriteBuffer::PushU32s(View<std::uint32_t> values)
	{
		Ensure(values.size() * 4);
		for (std::size_t i = 0; i < values.size(); ++i)
		{
			// TODO(MarcasRealAccount): Ensure writing out in Little Endian
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 24) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 16) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 8) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = values[i] & 0xFF;
		}
	}

	void WriteBuffer::PushU64s(View<std::uint64_t> values)
	{
		Ensure(values.size() * 8);
		for (std::size_t i = 0; i < values.size(); ++i)
		{
			// TODO(MarcasRealAccount): Ensure writing out in Little Endian
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 56) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 48) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 40) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 32) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 24) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 16) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = (values[i] >> 8) & 0xFF;
			(*m_BackingBuffer)[m_Offset++] = values[i] & 0xFF;
		}
	}
} // namespace Frertex::Utils
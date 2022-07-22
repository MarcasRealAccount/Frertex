#pragma once

#include "Profiler.h"

#include <cstdint>

#include <vector>

namespace Frertex::Utils
{
	struct Buffer
	{
	public:
		void pushU8(std::uint8_t value);
		void pushU16(std::uint16_t value);
		void pushU32(std::uint32_t value);
		void pushU64(std::uint64_t value);

		template <class Iterator>
		void pushU8s(Iterator begin, Iterator end);
		template <class Iterator>
		void pushU16s(Iterator begin, Iterator end);
		template <class Iterator>
		void pushU32s(Iterator begin, Iterator end);
		template <class Iterator>
		void pushU64s(Iterator begin, Iterator end);

		void reserve(std::size_t expected) { m_Buffer.reserve(expected); }

		std::vector<std::uint8_t>&& getBuffer() { return std::move(m_Buffer); }

	private:
		std::vector<std::uint8_t> m_Buffer;
	};

	template <class Iterator>
	void Buffer::pushU8s(Iterator begin, Iterator end)
	{
		PROFILE_FUNC;

		if (m_Buffer.capacity() - m_Buffer.size() < static_cast<std::size_t>(end - begin))
			reserve(m_Buffer.capacity() + (end - begin));

		while (begin != end)
		{
			pushU8(*begin);
			++begin;
		}
	}

	template <class Iterator>
	void Buffer::pushU16s(Iterator begin, Iterator end)
	{
		PROFILE_FUNC;

		if (m_Buffer.capacity() - m_Buffer.size() < static_cast<std::size_t>(end - begin))
			reserve(m_Buffer.capacity() + (end - begin));

		while (begin != end)
		{
			pushU16(*begin);
			++begin;
		}
	}

	template <class Iterator>
	void Buffer::pushU32s(Iterator begin, Iterator end)
	{
		PROFILE_FUNC;

		if (m_Buffer.capacity() - m_Buffer.size() < static_cast<std::size_t>(end - begin))
			reserve(m_Buffer.capacity() + (end - begin));

		while (begin != end)
		{
			pushU32(*begin);
			++begin;
		}
	}

	template <class Iterator>
	void Buffer::pushU64s(Iterator begin, Iterator end)
	{
		PROFILE_FUNC;

		if (m_Buffer.capacity() - m_Buffer.size() < static_cast<std::size_t>(end - begin))
			reserve(m_Buffer.capacity() + (end - begin));

		while (begin != end)
		{
			pushU64(*begin);
			++begin;
		}
	}
} // namespace Frertex::Utils
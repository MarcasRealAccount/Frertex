#pragma once

#include <cstddef>
#include <cstdint>

#include <vector>

namespace Frertex::Utils
{
	namespace Details
	{
		template <class Itr, class T>
		concept IteratorBaseType = requires(Itr itr)
		{
			{
				*itr
				} -> std::same_as<T>;
		};
	} // namespace Details

	struct Buffer
	{
	public:
		Buffer(std::vector<std::uint8_t>& buffer)
		    : m_Buffer(&buffer), m_Allocated(false) {}

		Buffer()
		    : m_Buffer(new std::vector<std::uint8_t>()), m_Allocated(true) {}

		Buffer(Buffer&& move) noexcept
		    : m_Buffer(move.m_Buffer), m_Allocated(move.m_Allocated)
		{
			move.m_Buffer    = nullptr;
			move.m_Allocated = false;
		}

		~Buffer();

		Buffer& operator=(Buffer&& move) noexcept
		{
			m_Buffer         = move.m_Buffer;
			m_Allocated      = move.m_Allocated;
			move.m_Buffer    = nullptr;
			move.m_Allocated = false;
			return *this;
		}

		std::vector<std::uint8_t> get()
		{
			if (!m_Buffer)
				return {};

			if (m_Allocated)
			{
				std::vector<std::uint8_t> buffer = std::move(*m_Buffer);
				m_Buffer                         = nullptr;
				m_Allocated                      = false;
				return buffer;
			}
			else
			{
				return *m_Buffer;
			}
		}

		void align16();
		void align32();
		void align64();

		std::uint8_t  popU8();
		std::uint16_t popU16();
		std::uint32_t popU32();
		std::uint64_t popU64();

		std::int8_t  popS8() { return static_cast<std::int8_t>(popU8()); }
		std::int16_t popS16() { return static_cast<std::int16_t>(popU16()); }
		std::int32_t popS32() { return static_cast<std::int32_t>(popU32()); }
		std::int64_t popS64() { return static_cast<std::int64_t>(popU64()); }

		void popU8s(std::uint8_t* start, std::uint8_t* end);
		void popU16s(std::uint16_t* start, std::uint16_t* end);
		void popU32s(std::uint32_t* start, std::uint32_t* end);
		void popU64s(std::uint64_t* start, std::uint64_t* end);

		void popS8s(std::int8_t* start, std::int8_t* end) { popU8s(reinterpret_cast<std::uint8_t*>(start), reinterpret_cast<std::uint8_t*>(end)); }
		void popS16s(std::int16_t* start, std::int16_t* end) { popU16s(reinterpret_cast<std::uint16_t*>(start), reinterpret_cast<std::uint16_t*>(end)); }
		void popS32s(std::int32_t* start, std::int32_t* end) { popU32s(reinterpret_cast<std::uint32_t*>(start), reinterpret_cast<std::uint32_t*>(end)); }
		void popS64s(std::int64_t* start, std::int64_t* end) { popU64s(reinterpret_cast<std::uint64_t*>(start), reinterpret_cast<std::uint64_t*>(end)); }

		template <class Itr>
		void popU8s(Itr start, Itr end)
		{
			for (; start != end; ++start)
				*start = popU8();
		}

		template <class Itr>
		void popU16s(Itr start, Itr end)
		{
			for (; start != end; ++start)
				*start = popU16();
		}

		template <class Itr>
		void popU32s(Itr start, Itr end)
		{
			for (; start != end; ++start)
				*start = popU32();
		}

		template <class Itr>
		void popU64s(Itr start, Itr end)
		{
			for (; start != end; ++start)
				*start = popU64();
		}

		void pushU8(std::uint8_t value);
		void pushU16(std::uint16_t value);
		void pushU32(std::uint32_t value);
		void pushU64(std::uint64_t value);

		void pushS8(std::int8_t value) { pushU8(static_cast<std::uint8_t>(value)); }
		void pushS16(std::int16_t value) { pushU16(static_cast<std::uint16_t>(value)); }
		void pushS32(std::int32_t value) { pushU32(static_cast<std::uint32_t>(value)); }
		void pushS64(std::int64_t value) { pushU64(static_cast<std::uint64_t>(value)); }

		void pushU8s(const std::uint8_t* start, const std::uint8_t* end);
		void pushU16s(const std::uint16_t* start, const std::uint16_t* end);
		void pushU32s(const std::uint32_t* start, const std::uint32_t* end);
		void pushU64s(const std::uint64_t* start, const std::uint64_t* end);

		void pushS8s(const std::int8_t* start, const std::int8_t* end) { pushU8s(reinterpret_cast<const std::uint8_t*>(start), reinterpret_cast<const std::uint8_t*>(end)); }
		void pushS16s(const std::int16_t* start, const std::int16_t* end) { pushU8s(reinterpret_cast<const std::uint8_t*>(start), reinterpret_cast<const std::uint8_t*>(end)); }
		void pushS32s(const std::int32_t* start, const std::int32_t* end) { pushU8s(reinterpret_cast<const std::uint8_t*>(start), reinterpret_cast<const std::uint8_t*>(end)); }
		void pushS64s(const std::int64_t* start, const std::int64_t* end) { pushU8s(reinterpret_cast<const std::uint8_t*>(start), reinterpret_cast<const std::uint8_t*>(end)); }

		template <class Itr>
		void pushU8s(Itr start, Itr end)
		{
			m_Buffer->resize(m_Offset + (end - start));
			for (; start != end; ++start)
				pushU8(*start);
		}

		template <class Itr>
		void pushU16s(Itr start, Itr end)
		{
			m_Buffer->resize(m_Offset + (end - start) * sizeof(std::uint16_t));
			for (; start != end; ++start)
				pushU16(*start);
		}

		template <class Itr>
		void pushU32s(Itr start, Itr end)
		{
			m_Buffer->resize(m_Offset + (end - start) * sizeof(std::uint32_t));
			for (; start != end; ++start)
				pushU32(*start);
		}

		template <class Itr>
		void pushU64s(Itr start, Itr end)
		{
			m_Buffer->resize(m_Offset + (end - start) * sizeof(std::uint64_t));
			for (; start != end; ++start)
				pushU64(*start);
		}

	private:
		std::vector<std::uint8_t>* m_Buffer;
		bool                       m_Allocated;

		std::size_t m_Offset = 0;
	};
} // namespace Frertex::Utils
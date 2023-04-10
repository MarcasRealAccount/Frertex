#pragma once

#include "View.h"

#include <cstdint>

#include <vector>

namespace Frertex::Utils
{
	struct ReadBuffer
	{
	public:
		ReadBuffer()
			: m_View(),
			  m_Offset(0) {}

		ReadBuffer(View<std::uint8_t> view)
			: m_View(view),
			  m_Offset(0)
		{
		}

		std::uint64_t Offset() const { return m_Offset; }

		void AlignU16();
		void AlignU32();
		void AlignU64();

		std::uint8_t  PopU8();
		std::uint16_t PopU16();
		std::uint32_t PopU32();
		std::uint64_t PopU64();

		inline std::int8_t PopI8() { static_cast<std::int8_t>(PopU8()); }

		inline std::int16_t PopI16() { static_cast<std::int16_t>(PopU16()); }

		inline std::int32_t PopI32() { static_cast<std::int32_t>(PopU32()); }

		inline std::int64_t PopI64() { static_cast<std::int16_t>(PopU64()); }

		void PopU8s(Span<std::uint8_t> values);
		void PopU16s(Span<std::uint16_t> values);
		void PopU32s(Span<std::uint32_t> values);
		void PopU64s(Span<std::uint64_t> values);

		inline void PopI8s(Span<std::int8_t> value) { PopU8s({ reinterpret_cast<std::uint8_t*>(value.begin()), reinterpret_cast<std::uint8_t*>(value.end()) }); }

		inline void PopI16s(Span<std::int16_t> value) { PopU16s({ reinterpret_cast<std::uint16_t*>(value.begin()), reinterpret_cast<std::uint16_t*>(value.end()) }); }

		inline void PopI32s(Span<std::int32_t> value) { PopU32s({ reinterpret_cast<std::uint32_t*>(value.begin()), reinterpret_cast<std::uint32_t*>(value.end()) }); }

		inline void PopI64s(Span<std::int64_t> value) { PopU64s({ reinterpret_cast<std::uint64_t*>(value.begin()), reinterpret_cast<std::uint64_t*>(value.end()) }); }

	private:
		View<std::uint8_t> m_View;
		std::uint64_t      m_Offset;
	};

	struct WriteBuffer
	{
	public:
		WriteBuffer()
			: m_BackingBuffer(nullptr),
			  m_Offset(0),
			  m_Allocated(false) {}

		WriteBuffer(View<std::uint8_t> view)
			: m_BackingBuffer(new std::vector<std::uint8_t>(view.size())),
			  m_Offset(0),
			  m_Allocated(true)
		{
			std::memcpy(m_BackingBuffer->data(), view.begin(), view.size());
		}

		WriteBuffer(std::vector<std::uint8_t>& buffer)
			: m_BackingBuffer(&buffer),
			  m_Offset(0),
			  m_Allocated(false) {}

		WriteBuffer(std::vector<std::uint8_t>* buffer)
			: m_BackingBuffer(buffer),
			  m_Offset(0),
			  m_Allocated(false) {}

		WriteBuffer(WriteBuffer&& move) noexcept
			: m_BackingBuffer(move.m_BackingBuffer),
			  m_Offset(move.m_Offset),
			  m_Allocated(move.m_Allocated)
		{
			move.m_BackingBuffer = nullptr;
			move.m_Offset        = 0;
			move.m_Allocated     = false;
		}

		WriteBuffer& operator=(WriteBuffer&& move) noexcept
		{
			m_BackingBuffer      = move.m_BackingBuffer;
			m_Offset             = move.m_Offset;
			m_Allocated          = move.m_Allocated;
			move.m_BackingBuffer = nullptr;
			move.m_Offset        = 0;
			move.m_Allocated     = false;
			return *this;
		}

		~WriteBuffer()
		{
			if (m_Allocated)
			{
				delete m_BackingBuffer;
				m_BackingBuffer = nullptr;
				m_Offset        = 0;
				m_Allocated     = false;
			}
		}

		std::uint64_t Offset() const { return m_Offset; }

		void Ensure(std::uint64_t bytes);

		void AlignU16();
		void AlignU32();
		void AlignU64();

		void PushU8(std::uint8_t value);
		void PushU16(std::uint16_t value);
		void PushU32(std::uint32_t value);
		void PushU64(std::uint64_t value);

		inline void PushI8(std::int8_t value) { PushU8(static_cast<std::uint8_t>(value)); }

		inline void PushI16(std::int16_t value) { PushU16(static_cast<std::uint16_t>(value)); }

		inline void PushI32(std::int32_t value) { PushU32(static_cast<std::uint32_t>(value)); }

		inline void PushI64(std::int64_t value) { PushU64(static_cast<std::uint64_t>(value)); }

		void PushU8s(View<std::uint8_t> values);
		void PushU16s(View<std::uint16_t> values);
		void PushU32s(View<std::uint32_t> values);
		void PushU64s(View<std::uint64_t> values);

		inline void PushI8s(View<std::int8_t> value) { PushU8s({ reinterpret_cast<const std::uint8_t*>(value.begin()), reinterpret_cast<const std::uint8_t*>(value.end()) }); }

		inline void PushI16s(View<std::int16_t> value) { PushU16s({ reinterpret_cast<const std::uint16_t*>(value.begin()), reinterpret_cast<const std::uint16_t*>(value.end()) }); }

		inline void PushI32s(View<std::int32_t> value) { PushU32s({ reinterpret_cast<const std::uint32_t*>(value.begin()), reinterpret_cast<const std::uint32_t*>(value.end()) }); }

		inline void PushI64s(View<std::int64_t> value) { PushU64s({ reinterpret_cast<const std::uint64_t*>(value.begin()), reinterpret_cast<const std::uint64_t*>(value.end()) }); }

	private:
		std::vector<std::uint8_t>* m_BackingBuffer;
		std::uint64_t              m_Offset;
		bool                       m_Allocated;
	};
} // namespace Frertex::Utils
#include "Frertex/FIL.h"
#include "Frertex/Utils/Profiler.h"

#include <fstream>

namespace Frertex
{
	struct Buffer
	{
	public:
		void pushU8(std::uint8_t value)
		{
			PROFILE_FUNC;

			m_Buffer.emplace_back(value);
		}

		void pushU16(std::uint16_t value)
		{
			PROFILE_FUNC;

			m_Buffer.emplace_back(static_cast<std::uint8_t>(value & 0xFF));
			m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
		}

		void pushU32(std::uint32_t value)
		{
			PROFILE_FUNC;

			m_Buffer.emplace_back(static_cast<std::uint8_t>(value & 0xFF));
			m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
			m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
			m_Buffer.emplace_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
		}

		void pushU64(std::uint64_t value)
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

		template <class Iterator>
		void pushU8s(Iterator begin, Iterator end)
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
		void pushU16s(Iterator begin, Iterator end)
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
		void pushU32s(Iterator begin, Iterator end)
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
		void pushU64s(Iterator begin, Iterator end)
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

		void reserve(std::size_t expected) { m_Buffer.reserve(expected); }

		std::vector<std::uint8_t>&& getBuffer() { return std::move(m_Buffer); }

	private:
		std::vector<std::uint8_t> m_Buffer;
	};

	std::vector<std::uint8_t> GetFILBinary(const FIL& fil)
	{
		PROFILE_FUNC;

		Buffer binary;
		binary.reserve(4 + 4 + 8 + fil.m_Entrypoints.size() * 12 + 8 + fil.m_Strings.size() + 8 + fil.m_Labels.size() * 32 + 8 + fil.m_LabelRefs.size() * 24 + 8 + fil.m_Code.size() * 4 + 8 + fil.m_Lines.size() * 32);
		binary.pushU32(0x4C49'4600); // Magic "\0FIL"
		binary.pushU32(0x0000'0000); // Version 0.0.0

		binary.pushU64(fil.m_Entrypoints.size());
		for (auto& entrypoint : fil.m_Entrypoints)
		{
			binary.pushU32(static_cast<std::uint32_t>(entrypoint.m_Type));
			binary.pushU32(0); // Reserved
			binary.pushU64(entrypoint.m_LabelID);
		}

		binary.pushU64(fil.m_Strings.size());
		binary.pushU8s(fil.m_Strings.begin(), fil.m_Strings.end());

		binary.pushU64(fil.m_Labels.size());
		for (auto& label : fil.m_Labels)
		{
			binary.pushU64(label.m_NameOffset);
			binary.pushU64(label.m_NameLength);
			binary.pushU64(label.m_CodeOffset);
			binary.pushU64(label.m_Length);
		}

		binary.pushU64(fil.m_LabelRefs.size());
		for (auto& ref : fil.m_LabelRefs)
		{
			binary.pushU64(ref.m_LabelID);
			binary.pushU64(ref.m_CodeOffset);
			binary.pushU64(ref.m_Length);
		}

		binary.pushU64(fil.m_Code.size());
		binary.pushU32s(fil.m_Code.begin(), fil.m_Code.end());

		binary.pushU64(fil.m_Lines.size());
		for (auto& line : fil.m_Lines)
		{
			binary.pushU64(line.m_CodeOffset);
			binary.pushU64(line.m_Line);
			binary.pushU64(line.m_FilenameOffset);
			binary.pushU64(line.m_FilenameLength);
		}

		return binary.getBuffer();
	}

	void WriteFILToFile(const std::filesystem::path& file, const FIL& fil)
	{
		PROFILE_FUNC;

		auto binary = GetFILBinary(fil);

		std::ofstream stream { file, std::ios::binary };
		if (stream)
		{
			stream.write(reinterpret_cast<const char*>(binary.data()), binary.size());
			stream.close();
		}
	}

	void CodeBuffer::pushNOP()
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0001'0000);
	}

	void CodeBuffer::pushRet()
	{
		PROFILE_FUNC;

		m_Code.emplace_back(0x0001'0001);
	}

	void CodeBuffer::pushLabel()
	{
		PROFILE_FUNC;
	}

	void CodeBuffer::pushLabelRef()
	{
		PROFILE_FUNC;
	}

	void CodeBuffer::pushLine()
	{
		PROFILE_FUNC;
	}
} // namespace Frertex
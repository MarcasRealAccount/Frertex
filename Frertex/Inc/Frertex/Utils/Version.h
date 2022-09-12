#pragma once

#include <cstdint>

#include <bit>

namespace Frertex::Utils
{
	struct Version
	{
	public:
		struct SubVersions
		{
		public:
			std::uint32_t patch : 12;
			std::uint32_t minor : 10;
			std::uint32_t major : 10;
		};

	public:
		constexpr Version() : m_Version(0) {}
		constexpr Version(std::uint32_t version) : m_Version(version) {}
		constexpr Version(std::uint32_t major, std::uint32_t minor, std::uint32_t patch) : m_Version(std::bit_cast<std::uint32_t>(SubVersions { patch, minor, major })) {}

		constexpr operator std::uint32_t() const { return m_Version; }
		constexpr operator SubVersions() const { return std::bit_cast<SubVersions>(m_Version); }

	public:
		std::uint32_t m_Version;
	};
} // namespace Frertex::Utils
#pragma once

#include "Core.h"

#include <bit>
#include <chrono>
#include <string>
#include <unordered_map>

namespace Frertex::Utils
{
	template <class T, class U>
	concept ConstructibleFrom = requires(T t) {
		                            U { t };
	                            };

	template <class T>
	struct CopyMovable
	{
	public:
		template <ConstructibleFrom<T> U>
		CopyMovable(const U& value) : m_Value(T { value })
		{
		}
		template <ConstructibleFrom<T> U>
		CopyMovable(U&& value) : m_Value(T { std::move(value) })
		{
		}

		const T& ref() const { return m_Value; }
		T&&      get() { return std::move(m_Value); }

	private:
		T m_Value;
	};

	std::string EscapeString(Utils::CopyMovable<std::string>&& str);

	template <class To, class From>
	[[nodiscard]] constexpr To BitCast(const From& from) noexcept
	{
#if BUILD_IS_SYSTEM_MACOSX
		return __builtin_bit_cast(To, from);
#else
		return std::bit_cast<To, From>(from);
#endif
	}
} // namespace Frertex::Utils

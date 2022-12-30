#pragma once

#include <utility>

namespace TemplateEngine::Utils
{
	template <class T, class U>
	concept ConstructibleFrom = requires(T t)
	{
		U { t };
	};

	template <class T>
	struct RRef
	{
	public:
		RRef() : m_Value({}) {}
		template <ConstructibleFrom<T> U>
		RRef(const U& value)
		    : m_Value(T { value })
		{
		}
		template <ConstructibleFrom<T> U>
		RRef(U&& value)
		    : m_Value(T { std::move(value) })
		{
		}

		const T& ref() const { return m_Value; }
		T&&      get() { return std::move(m_Value); }

	private:
		T m_Value;
	};
} // namespace TemplateEngine::Utils
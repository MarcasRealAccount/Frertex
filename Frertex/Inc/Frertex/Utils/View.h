#pragma once

#include <cstddef>

#include <array>
#include <initializer_list>
#include <vector>

namespace Utils
{
	template <class T>
	struct View
	{
	public:
		using BaseT  = std::remove_const_t<T>;
		using ConstT = const BaseT;

	public:
		View()
			: m_Begin(nullptr),
			  m_End(nullptr)
		{
		}

		template <std::size_t N>
		View(ConstT (&arr)[N])
			: m_Begin(&arr[0]),
			  m_End(&arr[N])
		{
		}

		template <std::size_t N>
		View(const std::array<BaseT, N>& arr)
			: m_Begin(&arr[0]),
			  m_End(&arr[N])
		{
		}

		template <std::size_t N>
		View(const std::array<ConstT, N>& arr)
			: m_Begin(&arr[0]),
			  m_End(&arr[N])
		{
		}

		template <class Alloc>
		View(const std::vector<BaseT, Alloc>& arr)
			: m_Begin(&arr.front()),
			  m_End(&arr.back())
		{
		}

		template <class Alloc>
		View(const std::vector<ConstT, Alloc>& arr)
			: m_Begin(&arr.front()),
			  m_End(&arr.back())
		{
		}

		View(const std::initializer_list<ConstT>& initList)
			: m_Begin(initList.begin()),
			  m_End(initList.end())
		{
		}

		View(ConstT* begin, ConstT* end)
			: m_Begin(begin),
			  m_End(end)
		{
		}

		std::size_t size() const { return m_End - m_Begin; }

		bool empty() const { return size() == 0; }

		ConstT* begin() const { return m_Begin; }

		ConstT* end() const { return m_End; }

		ConstT& operator[](std::size_t index) const { return m_Begin[index]; }

	private:
		ConstT* m_Begin;
		ConstT* m_End;
	};
} // namespace Utils
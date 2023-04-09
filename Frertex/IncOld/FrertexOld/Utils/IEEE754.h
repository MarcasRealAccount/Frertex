#pragma once

#include <cstdint>

#include <bit>

namespace Frertex::Utils::IEEE754
{
	struct Half
	{
	public:
		static constexpr std::size_t   Base            = 2;
		static constexpr std::size_t   SignificandBits = 11;
		static constexpr std::size_t   FractionBits    = SignificandBits - 1;
		static constexpr std::uint16_t FractionMask    = 0b0'00000'1111111111;
		static constexpr std::uint16_t ExponentMask    = 0b0'11111'0000000000;
		static constexpr std::size_t   ExponentBits    = 5;
		static constexpr std::uint16_t ExponentBias    = 15;
		static constexpr std::size_t   ExponentMin     = -14;
		static constexpr std::size_t   ExponentMax     = 15;

	public:
		constexpr Half(std::uint16_t value) : m_Value(value) {}
		constexpr Half(float value)
		{
			std::uint32_t fp32     = std::bit_cast<std::uint32_t>(value);
			bool          sign     = fp32 >> 31;
			std::int8_t   exponent = ((fp32 >> 23) & 0xFF) - 127;
			if (exponent < -14)
			{
				m_Value = sign << (FractionBits + ExponentBits) | 0b00000'0000000000;
				return;
			}
			if (exponent > 15)
			{
				m_Value = sign << (FractionBits + ExponentBits) | 0b11111'0000000000;
				return;
			}

			std::uint16_t fraction = fp32 & 0x7F'FFFF;

			m_Value = sign << (FractionBits + ExponentBits) | (exponent + 15) << FractionBits | (fraction >> 23 - FractionBits);
		}
		constexpr Half(double value)
		{
			std::uint64_t fp64     = std::bit_cast<std::uint64_t>(value);
			bool          sign     = fp64 >> 63;
			std::int16_t  exponent = ((fp64 >> 52) & 0x7FF) - 1023;
			if (exponent < -14)
			{
				m_Value = sign << (FractionBits + ExponentBits) | 0b00000'0000000000;
				return;
			}
			if (exponent > 15)
			{
				m_Value = sign << (FractionBits + ExponentBits) | 0b11111'0000000000;
				return;
			}

			std::uint64_t fraction = fp64 & 0xF'FFFF'FFFF'FFFF;

			m_Value = sign << (FractionBits + ExponentBits) | (exponent + 15) << FractionBits | (fraction >> 52 - FractionBits);
		}

		constexpr std::int32_t toInt() const
		{
			return static_cast<std::int32_t>(toFloat());
		}
		constexpr float toFloat() const
		{
			std::uint8_t  biasedExponent = getBiasedExponent();
			std::uint32_t fraction       = getFraction();
			std::uint32_t fp32           = getSign() << 31;
			if (biasedExponent == 0 && fraction == 0)
				return std::bit_cast<float>(fp32);
			if (biasedExponent == 0b11111 && fraction == 0)
				return std::bit_cast<float>(fp32 | 0b11111111'00000000000000000000000);
			if (biasedExponent == 0b11111 && fraction != 0)
				return std::bit_cast<float>(fp32 | 0b11111111'00000000000000000000000 | fraction << 23 - FractionBits);

			std::uint8_t exponent = biasedExponent - ExponentBias + 127;
			fp32 |= exponent << 23;
			fp32 |= fraction << 23 - FractionBits;
			return std::bit_cast<float>(fp32);
		}
		constexpr double toDouble() const
		{
			std::uint8_t  biasedExponent = getBiasedExponent();
			std::uint64_t fraction       = getFraction();
			std::uint64_t fp64           = static_cast<std::uint64_t>(getSign()) << 63;
			if (biasedExponent == 0 && fraction == 0)
				return std::bit_cast<double>(fp64);
			if (biasedExponent == 0b11111 && fraction == 0)
				return std::bit_cast<double>(fp64 | 0b11111111111'0000000000000000000000000000000000000000000000000000);
			if (biasedExponent == 0b11111 && fraction != 0)
				return std::bit_cast<double>(fp64 | 0b11111111111'0000000000000000000000000000000000000000000000000000 | fraction << 52 - FractionBits);

			std::uint16_t exponent = biasedExponent - ExponentBias + 1023;
			fp64 |= static_cast<std::uint64_t>(exponent) << 52;
			fp64 |= fraction << 52 - FractionBits;
			return std::bit_cast<double>(fp64);
		}

		constexpr operator std::int32_t() const { return toInt(); }
		constexpr operator float() const { return toFloat(); }
		constexpr operator double() const { return toDouble(); }

		constexpr friend Half operator+(Half lhs, Half rhs) { return lhs.toFloat() + rhs.toFloat(); }
		constexpr friend Half operator-(Half lhs, Half rhs) { return lhs.toFloat() - rhs.toFloat(); }
		constexpr friend Half operator*(Half lhs, Half rhs) { return lhs.toFloat() * rhs.toFloat(); }
		constexpr friend Half operator/(Half lhs, Half rhs) { return lhs.toFloat() / rhs.toFloat(); }
		constexpr friend Half operator-(Half value) { return -value.toFloat(); }
		constexpr friend bool operator==(Half lhs, Half rhs) { return lhs.m_Value == rhs.m_Value; }
		constexpr friend bool operator!=(Half lhs, Half rhs) { return !(lhs == rhs); }
		constexpr friend bool operator<(Half lhs, Half rhs)
		{
			if (lhs.getSign())
			{
				if (rhs.getSign())
					return lhs.m_Value > rhs.m_Value;
				else
					return true;
			}
			else
			{
				if (rhs.getSign())
					return false;
				else
					return lhs.m_Value < rhs.m_Value;
			}
		}
		constexpr friend bool operator>(Half lhs, Half rhs) { return rhs < lhs; }
		constexpr friend bool operator<=(Half lhs, Half rhs) { return !(lhs > rhs); }
		constexpr friend bool operator>=(Half lhs, Half rhs) { return !(lhs < rhs); }
		friend Half           sqrt(Half half) { return sqrtf(half.toFloat()); }
		friend Half           remainder(Half lhs, Half rhs) { return remainderf(lhs.toFloat(), rhs.toFloat()); }
		constexpr friend Half min(Half a, Half b) { return a < b ? a : b; }
		constexpr friend Half max(Half a, Half b) { return a > b ? a : b; }
		constexpr friend Half abs(Half value) { return value.getSign() ? -value : value; }

		constexpr std::uint16_t getFraction() const { return m_Value & FractionMask; }
		constexpr std::uint8_t  getBiasedExponent() const { return (m_Value & ExponentMask) >> FractionBits; }
		constexpr std::int8_t   getExponent() const { return getBiasedExponent() - ExponentBias; }
		constexpr bool          getSign() const { return m_Value >> (FractionBits + ExponentBits); }

		constexpr bool isInf() const { return getBiasedExponent() == 0b11111 && getFraction() == 0; }
		constexpr bool isNaN() const { return getBiasedExponent() == 0b11111 && getFraction() != 0; }

	private:
		std::uint16_t m_Value;
	};

	static constexpr auto MinValue = Half(0.000060975552);
	static constexpr auto MaxValue = std::bit_cast<std::uint32_t>(Half(1.0f).toFloat());
} // namespace Frertex::Utils::IEEE754
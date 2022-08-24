#pragma once

#include <cmath>
#include <limits>

namespace Frertex::Math
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
		static constexpr std::size_t   ExponentBias    = 15;
		static constexpr std::size_t   SignBit         = FractionBits + ExponentBits;

	public:
		constexpr Half() noexcept : m_Value(0) {}
		constexpr Half(std::uint16_t value) noexcept : m_Value(value) {}
		explicit constexpr Half(float value) noexcept;
		explicit constexpr Half(double value) noexcept;

		constexpr std::int32_t toInt() const;
		constexpr float        toFloat() const;
		constexpr double       toDouble() const;

		constexpr explicit operator std::int32_t() const { return toInt(); }
		constexpr          operator float() const { return toFloat(); }
		constexpr          operator double() const { return toDouble(); }

		constexpr friend Half operator+(Half lhs, Half rhs) { return Half { lhs.toFloat() + rhs.toFloat() }; }
		constexpr friend Half operator-(Half lhs, Half rhs) { return Half { lhs.toFloat() - rhs.toFloat() }; }
		constexpr friend Half operator*(Half lhs, Half rhs) { return Half { lhs.toFloat() * rhs.toFloat() }; }
		constexpr friend Half operator/(Half lhs, Half rhs) { return Half { lhs.toFloat() / rhs.toFloat() }; }
		constexpr friend Half operator-(Half value) { return Half { -value.toFloat() }; }

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
		constexpr friend bool operator<=(Half lhs, Half rhs) { return !(rhs > lhs); }
		constexpr friend bool operator>=(Half lhs, Half rhs) { return !(rhs < lhs); }

		friend Half           sqrt(Half half) { return Half { sqrtf(half) }; }
		friend Half           remainder(Half x, Half y) { return Half { remainderf(x, y) }; }
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
} // namespace Frertex::Math

template <>
struct std::numeric_limits<Frertex::Math::Half>
{
	static constexpr bool                    is_specialized    = true;
	static constexpr bool                    is_signed         = true;
	static constexpr bool                    is_integer        = false;
	static constexpr bool                    is_exact          = false;
	static constexpr bool                    has_infinity      = true;
	static constexpr bool                    has_quiet_NaN     = true;
	static constexpr bool                    has_signaling_NaN = true;
	static constexpr std::float_denorm_style has_denorm        = std::denorm_present;
	static constexpr bool                    has_denorm_loss   = false;
	static constexpr std::float_round_style  round_style       = std::round_to_nearest;
	static constexpr bool                    is_iec559         = true;
	static constexpr bool                    is_bounded        = true;
	static constexpr bool                    is_modulo         = false;
	static constexpr int                     digits            = 11;
	static constexpr int                     digits10          = 3;
	static constexpr int                     max_digits10      = 4;
	static constexpr int                     radix             = 2;
	static constexpr int                     min_exponent      = -14;
	static constexpr int                     min_exponent10    = -4;
	static constexpr int                     max_exponent      = 15;
	static constexpr int                     max_exponent10    = 4;
	static constexpr bool                    traps             = false;
	static constexpr bool                    tinyness_before   = false;

	static constexpr Frertex::Math::Half min() noexcept { return std::bit_cast<Frertex::Math::Half>(static_cast<std::uint16_t>(0b0'00001'0000000000)); }
	static constexpr Frertex::Math::Half lowest() noexcept { -max(); }
	static constexpr Frertex::Math::Half max() noexcept { return std::bit_cast<Frertex::Math::Half>(static_cast<std::uint16_t>(0b0'11110'1111111111)); }
	static constexpr Frertex::Math::Half epsilon() noexcept { return std::bit_cast<Frertex::Math::Half>(static_cast<std::uint16_t>(0b0'00000'0000000000)); }
	static constexpr Frertex::Math::Half round_error() noexcept { return std::bit_cast<Frertex::Math::Half>(static_cast<std::uint16_t>(0b0'01110'0000000000)); }
	static constexpr Frertex::Math::Half infinity() noexcept { return std::bit_cast<Frertex::Math::Half>(static_cast<std::uint16_t>(0b0'11111'0000000000)); }
	static constexpr Frertex::Math::Half quiet_NaN() noexcept { return std::bit_cast<Frertex::Math::Half>(static_cast<std::uint16_t>(0b0'11111'1000000001)); }
	static constexpr Frertex::Math::Half signaling_NaN() noexcept { return std::bit_cast<Frertex::Math::Half>(static_cast<std::uint16_t>(0b0'11111'0000000001)); }
	static constexpr Frertex::Math::Half denorm_min() noexcept { return std::bit_cast<Frertex::Math::Half>(static_cast<std::uint16_t>(0b0'00000'0000000001)); }
};

namespace Frertex::Math
{
	constexpr Half::Half(float value) noexcept
	{
		std::uint32_t fp32     = std::bit_cast<std::uint32_t>(value);
		bool          sign     = fp32 >> 31;
		std::int8_t   exponent = ((fp32 >> 23) & 0xFF) - 127;
		if (exponent < -14)
		{
			m_Value = sign << (FractionBits + ExponentBits);
			return;
		}
		if (exponent > 15)
		{
			m_Value = sign << (FractionBits + ExponentBits) | 0b11111'0000000000;
			return;
		}

		std::uint32_t fraction = fp32 & 0x7F'FFFF;

		m_Value = sign << (FractionBits + ExponentBits) | (exponent + 15) << FractionBits | (fraction >> (23 - FractionBits));
	}

	constexpr Half::Half(double value) noexcept
	{
		std::uint64_t fp64     = std::bit_cast<std::uint64_t>(value);
		bool          sign     = fp64 >> 63;
		std::int16_t  exponent = ((fp64 >> 52) & 0xFF) - 1023;
		if (exponent < -14)
		{
			m_Value = sign << (FractionBits + ExponentBits);
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

	constexpr std::int32_t Half::toInt() const
	{
		return static_cast<std::int32_t>(toFloat());
	}

	constexpr float Half::toFloat() const
	{
		std::uint8_t  biasedExponent = getBiasedExponent();
		std::uint32_t fraction       = getFraction();
		std::uint32_t fp32           = getSign() << 31;
		if (biasedExponent == 0 && fraction == 0)
			return std::bit_cast<float>(fp32);
		if (biasedExponent == 0b11111)
		{
			if (fraction == 0)
				return std::bit_cast<float>(fp32 | 0b11111111'00000000000000000000000);
			return std::bit_cast<float>(fp32 | 0b11111111'00000000000000000000000 | fraction << 23 - FractionBits);
		}

		std::uint8_t exponent = biasedExponent - ExponentBias + 127;
		fp32 |= exponent << 23;
		fp32 |= fraction << 23 - FractionBits;
		return std::bit_cast<float>(fp32);
	}

	constexpr double Half::toDouble() const
	{
		std::uint8_t  biasedExponent = getBiasedExponent();
		std::uint64_t fraction       = getFraction();
		std::uint64_t fp64           = getSign() << 63;
		if (biasedExponent == 0 && fraction == 0)
			return std::bit_cast<double>(fp64);
		if (biasedExponent == 0b11111)
		{
			if (fraction == 0)
				return std::bit_cast<double>(fp64 | 0b11111111111'0000000000000000000000000000000000000000000000000000);
			return std::bit_cast<double>(fp64 | 0b11111111111'0000000000000000000000000000000000000000000000000000 | fraction << 52 - FractionBits);
		}

		std::uint16_t exponent = biasedExponent - ExponentBias + 1023;
		fp64 |= exponent << 52;
		fp64 |= fraction << 52 - FractionBits;
		return std::bit_cast<double>(fp64);
	}
} // namespace Frertex::Math
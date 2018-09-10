#pragma once

using Value_t = double;

template <int L, int M, int T>
struct Quantity {

	Quantity(Value_t value) : value(value) { };
	Quantity(const Quantity<L, M, T>& rhs) : value(rhs.get_value()) { };
	Quantity(Quantity<L, M, T>&&) = delete;
	Quantity& operator=(const Quantity<L, M, T>& rhs) { value = rhs.get_value(); };
	Quantity& operator=(Quantity<L, M, T>&&) = delete;

	Quantity& operator+=(const Quantity rhs) {
		value += rhs.get_value();
	};
	Quantity& operator-=(const Quantity rhs) {
		value -= rhs.get_value();
	};

	inline Value_t get_value() const noexcept { return value; };

private:
	Value_t value;
};

template<int L, int M, int T>
Quantity<L, M, T> operator+(const Quantity<L, M, T> lhs, const Quantity<L, M, T> rhs) {
	return Quantity<L, M, T>(lhs.get_value() + rhs.get_value());
};

template<int L, int M, int T>
Quantity<L, M, T> operator-(const Quantity<L, M, T> lhs, const Quantity<L, M, T> rhs) {
	return Quantity<L, M, T>.get_value() - rhs.get_value();
};

template<int L, int M, int T>
Quantity<L, M, T> operator*(const Value_t lhs, const Quantity<L, M, T> rhs) {
	return Quantity<L, M, T>(lhs * rhs.get_value());
};

template<int L, int M, int T>
Quantity<L, M, T> operator*(const Quantity<L,M,T> lhs, Value_t rhs) {
	return Quantity<L, M, T>(lhs.get_value() * rhs);
};

template<int LL, int ML, int TL, int LR, int MR, int TR>
Quantity<LL+LR, ML+MR, TL+TR> operator*(const Quantity<LL, ML, TL> lhs, const Quantity<LR, MR, TR> rhs) {
	return Quantity<LL+LR, ML+MR, TL+TR>(lhs.get_value() * rhs.get_value());
};

template<int L, int M, int T>
Quantity<L, M, T> operator/(const Value_t lhs, const Quantity<L, M, T> rhs) {
	return Quantity<L, M, T>(lhs / rhs.get_value());
};

template<int L, int M, int T>
Quantity<L, M, T> operator/(const Quantity<L,M,T> lhs, Value_t rhs) {
	return Quantity<L, M, T>(lhs.get_value() / rhs);
};

template<int LL, int ML, int TL, int LR, int MR, int TR>
Quantity<LL-LR, ML-MR, TL-TR> operator/(const Quantity<LL, ML, TL> lhs, const Quantity<LR, MR, TR> rhs) {
	return Quantity<LL-LR, ML-MR, TL-TR>(lhs.get_value() / rhs.get_value());
};

namespace MKS {
	using Value = Quantity<0, 0, 0>;
	using Length = Quantity<1, 0, 0>;
	using Area = Quantity<2, 0, 0>;
	using Time = Quantity<0, 1, 0>;
	using Frequency = Quantity<0, -1, 0>;
	using Mass = Quantity<0, 0, 1>;
	using Speed = Quantity<1, -1, 0>;
	using Acceleration = Quantity<1, -2, 0>;
}

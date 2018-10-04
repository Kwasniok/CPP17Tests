
#include<assert.h>
#include <iostream>

using namespace std;

template<typename number_t>
class Basic_Complex
{
public:
	Basic_Complex(number_t re, number_t im = 0) : re(re), im(im) {  }
	Basic_Complex& operator+=(const Basic_Complex& rhs)
	{
		re += rhs.re;
		im += rhs.im;
		return *this;
	}
	Basic_Complex& operator-=(const Basic_Complex& rhs)
	{
		re -= rhs.re;
		im -= rhs.im;
		return *this;
	}
	Basic_Complex& operator*=(const Basic_Complex& rhs)
	{
		Basic_Complex z(re * rhs.re - im * rhs.im, re * rhs.im + im * rhs.re);
		*this = z;
		return *this;
	}
	Basic_Complex& operator/=(const Basic_Complex& rhs)
	{
		// calculate the multiplicative inverse of rhs
		Basic_Complex z(rhs.re / (rhs.re * rhs.re + rhs.im * rhs.im), -rhs.im / (rhs.re * rhs.re + rhs.im * rhs.im));
		*this *= z;
		return *this;
	}
	Basic_Complex& conjugate()
	{
		im *= -1;
		return *this;
	}
	number_t re, im;
};

template<typename number_t>
Basic_Complex<number_t> operator+(const Basic_Complex<number_t>& lhs, const Basic_Complex<number_t>& rhs)
{
	Basic_Complex z(lhs);
	z += rhs;
	return z;
}
template<typename number_t>
Basic_Complex<number_t> operator-(const Basic_Complex<number_t>& lhs, const Basic_Complex<number_t>& rhs)
{
	Basic_Complex z(lhs);
	z -= rhs;
	return z;
}
template<typename number_t>
Basic_Complex<number_t> operator*(const Basic_Complex<number_t>& lhs, const Basic_Complex<number_t>& rhs)
{
	Basic_Complex z(lhs);
	z *= rhs;
	return z;
}
template<typename number_t>
Basic_Complex<number_t> operator/(const Basic_Complex<number_t>& lhs, const Basic_Complex<number_t>& rhs)
{
	Basic_Complex z(lhs);
	z /= rhs;
	return z;
}
template<typename number_t>
bool operator==(const Basic_Complex<number_t>& lhs, const Basic_Complex<number_t>& rhs)
{
	return lhs.re == rhs.re && lhs.im == rhs.im;
}
template<typename number_t>
bool operator!=(const Basic_Complex<number_t>& lhs, const Basic_Complex<number_t>& rhs)
{
	return lhs.re != rhs.re || lhs.im != rhs.im;
}

template<typename number_t>
ostream& operator<<(ostream& os, Basic_Complex<number_t> z)
{
	os << "(" << z.re << "," << z.im << ")";
	return os;
}

using Complex = Basic_Complex<double>;

int main()
{
	// Complex tests
	{
		Complex z1(1, 3);
		Complex z2(2, 4);
		Complex z3 = z1 + z2;
		assert(z3 - z2 == z1);
		z3 = z1 * z2;
		assert(z3 / z2 == z1);
		cout << z3;
	}
}

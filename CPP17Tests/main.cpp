
#include <iostream>
#include <cassert>
#include <string>

struct ci_char_traits: public std::char_traits<char> {
	static constexpr bool compare(const char * lhs, const char * rhs, size_t n) {
		// adapted from: http://www.cplusplus.com/reference/string/char_traits/compare/
		// could be optimized
		while (n--) {if (!eq(*lhs,*rhs)) return lt(*lhs,*rhs)?-1:1; ++lhs; ++rhs;}
		return 0;
	}
	static constexpr bool eq(char lhs, char rhs) {
		return to_std_repr(lhs) == to_std_repr(rhs);
	}
	static constexpr bool lt(char lhs, char rhs) {
		return to_std_repr(lhs) < to_std_repr(rhs);
	}
	static constexpr const char* find(const char* s, int n, char a) {
		while (n-- > 0 && to_std_repr(*s) != to_std_repr(a)) { ++s; }
		return n > 0 ? s : 0;
	}
	static constexpr char to_std_repr(const char c) {
		// override for insensitivity
		// converts A-Z to a-z (assuming ASCII)
		// A := 0x41, Z := 0x5A
		// a := 0x61, z := 0x7A
		if (0x41 <= c && c <= 0x5A) return c + 0x20;
		return c;
	}
};

using ci_string = std::basic_string<char, ci_char_traits, std::allocator<char>>;

int main() {
	ci_string s("AbCdE");
	assert( s == "abcde" );
	assert( s == "ABCDE" );
	assert( strcmp( s.c_str(), "AbCdE" ) == 0 );
	assert( strcmp( s.c_str(), "abcde" ) != 0 );
}

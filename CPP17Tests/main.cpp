
#include <iostream>
#include <cassert>
#include <string>

template <int v>
constexpr char to_std_repr(char c) {
	// converts characters to thier standard representation
	// e.g. for case intensitivity 'A' and 'a' are mapped to 'a' etc. (lowercase ist standard representation)
	// default is identity
	return c;
}

template<int v>
struct insensitive_char_traits: public std::char_traits<char> {
	static constexpr bool compare(const char * lhs, const char * rhs, size_t n) {
		// adapted from: http://www.cplusplus.com/reference/string/char_traits/compare/
		// could be optimized
		while (n--) {if (!eq(*lhs,*rhs)) return lt(*lhs,*rhs)?-1:1; ++lhs; ++rhs;}
		return 0;
	}
	static constexpr bool eq(char lhs, char rhs) {
		return to_std_repr<v>(lhs) == to_std_repr<v>(rhs);
	}
	static constexpr bool lt(char lhs, char rhs) {
		return to_std_repr<v>(lhs) < to_std_repr<v>(rhs);
	}
	static constexpr const char* find(const char* s, int n, char a) {
		while (n-- > 0 && to_std_repr<v>(*s) != to_std_repr<v>(a)) { ++s; }
		return n > 0 ? s : nullptr;
	}
};

constexpr int NONE = 0;
constexpr int CASE = 1;
constexpr int DIGIT = 2;

template<>
constexpr char to_std_repr<CASE>(char c) {
	// override for insensitivity of cases
	// converts A-Z to a-z (assuming ASCII)
	// A := 0x41, Z := 0x5A
	// a := 0x61, z := 0x7A
	if (0x41 <= c && c <= 0x5A) return c + 0x20;
	return c;
}

template<>
constexpr char to_std_repr<DIGIT>(const char c) {
	// override for insensitivity of digits
	// converts 0-9 to 0 (assuming ASCII)
	// 0 := 0x30, 9 := 0x39
	if (0x30 <= c && c <= 0x39) return 0x30;
	return c;
}

// define insensitive string classes
using ci_string = std::basic_string<char, insensitive_char_traits<CASE>, std::allocator<char>>;
using di_string = std::basic_string<char, insensitive_char_traits<DIGIT>, std::allocator<char>>;

int main() {
	{
		ci_string s("AbCdE");
		assert(s == "abcde");
		assert(s == "ABCDE");
		assert(strcmp(s.c_str(), "AbCdE") == 0);
		assert(strcmp(s.c_str(), "abcde") != 0);
	}
	{
		di_string s("Hello 12");
		assert(s == "Hello 89");
		assert(s == "Hello 26");
		assert(strcmp(s.c_str(), "Hello 12") == 0);
		assert(strcmp(s.c_str(), "Hello 00") != 0);
	}
}
